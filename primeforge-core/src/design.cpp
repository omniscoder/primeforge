#include "primeforge/design.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>

#include "primeforge/pam.hpp"
#include "primeforge/utils.hpp"

namespace primeforge {
namespace {

struct EditBounds {
  int start{0};
  int end{0}; // exclusive
};

struct PamHit {
  size_t pam_idx{0};  // start index in seq_view
  bool is_reverse{false};
};

struct MotifHit {
  PamHit hit;
  std::string motif;
};

EditBounds bounds_for_edit(const EditVariant &edit) {
  if (std::holds_alternative<EditSubstitution>(edit)) {
    const auto &e = std::get<EditSubstitution>(edit);
    return {e.pos, e.pos + 1};
  }
  if (std::holds_alternative<EditInsertion>(edit)) {
    const auto &e = std::get<EditInsertion>(edit);
    return {e.pos, e.pos};
  }
  const auto &e = std::get<EditDeletion>(edit);
  return {e.start, e.start + e.length};
}

int first_edit_pos(const PrimeEditSpec &spec) {
  if (spec.edits.empty()) return 0;
  int min_pos = std::numeric_limits<int>::max();
  for (const auto &ev : spec.edits) {
    min_pos = std::min(min_pos, bounds_for_edit(ev).start);
  }
  return min_pos == std::numeric_limits<int>::max() ? 0 : min_pos;
}

std::string apply_edits(const PrimeEditSpec &spec) {
  std::string seq = spec.ref_sequence;
  // Apply edits in position order for determinism.
  std::vector<std::pair<int, EditVariant>> ordered;
  ordered.reserve(spec.edits.size());
  for (const auto &e : spec.edits) {
    ordered.emplace_back(bounds_for_edit(e).start, e);
  }
  std::sort(ordered.begin(), ordered.end(), [](const auto &a, const auto &b) {
    return a.first < b.first;
  });

  int offset = 0;  // track length delta to keep positions consistent
  for (const auto &[_, edit] : ordered) {
    if (std::holds_alternative<EditSubstitution>(edit)) {
      const auto &e = std::get<EditSubstitution>(edit);
      int idx = e.pos + offset;
      if (idx >= 0 && idx < static_cast<int>(seq.size())) {
        seq[idx] = e.alt;
      }
    } else if (std::holds_alternative<EditInsertion>(edit)) {
      const auto &e = std::get<EditInsertion>(edit);
      int idx = e.pos + offset;
      if (idx >= 0 && idx <= static_cast<int>(seq.size())) {
        seq.insert(idx, e.inserted);
        offset += static_cast<int>(e.inserted.size());
      }
    } else if (std::holds_alternative<EditDeletion>(edit)) {
      const auto &e = std::get<EditDeletion>(edit);
      int idx = e.start + offset;
      if (idx >= 0 && idx + e.length <= static_cast<int>(seq.size())) {
        seq.erase(idx, e.length);
        offset -= e.length;
      }
    }
  }
  return seq;
}

std::vector<PamHit> collect_pam_hits(const std::string &seq_view, const std::string &motif,
                                     const Device &device) {
  std::vector<PamHit> hits;
  auto plus_hits = find_pam_sites(seq_view, motif, device);
  for (auto h : plus_hits) hits.push_back(PamHit{h, false});

  std::string rc = reverse_complement(seq_view);
  auto rev_hits = find_pam_sites(rc, motif, device);
  const int L = static_cast<int>(seq_view.size());
  for (auto h_rc : rev_hits) {
    size_t mapped = static_cast<size_t>(L - (static_cast<int>(h_rc) + static_cast<int>(motif.size())));
    hits.push_back(PamHit{mapped, true});
  }
  return hits;
}

}  // namespace

CandidateList design_prime_edit(const PrimeEditSpec &edit, const DesignConfig &cfg,
                                const Device &device) {
  CandidateList out;

  const bool reverse = (edit.strand == Strand::Minus);
  const int seq_len = static_cast<int>(edit.ref_sequence.size());

  // Map edit positions into the working orientation.
  const auto map_pos_view = [&](int pos) {
    return reverse ? (seq_len - 1 - pos) : pos;
  };

  const int edit_start_orig = first_edit_pos(edit);

  int edit_min_view = std::numeric_limits<int>::max();
  int edit_max_view = std::numeric_limits<int>::min();
  for (const auto &ev : edit.edits) {
    const auto b = bounds_for_edit(ev);
    int s = map_pos_view(b.start);
    int endpoint = (b.end > b.start) ? (b.end - 1) : b.start;
    int e = map_pos_view(endpoint);  // inclusive endpoint
    edit_min_view = std::min({edit_min_view, s, e});
    edit_max_view = std::max({edit_max_view, s, e});
  }
  if (edit_min_view == std::numeric_limits<int>::max()) {
    edit_min_view = edit_max_view = 0;
  }

  std::string seq_view = reverse ? reverse_complement(edit.ref_sequence) : edit.ref_sequence;
  std::string edited_view =
      reverse ? reverse_complement(apply_edits(edit)) : apply_edits(edit);

  std::vector<MotifHit> all_hits;
  for (const auto &motif : cfg.pam_motifs) {
    auto pam_hits = collect_pam_hits(seq_view, motif, device);
    for (const auto &h : pam_hits) {
      all_hits.push_back(MotifHit{h, motif});
    }
  }

  // Pre-filter reverse hits for possible ngRNAs.
  std::vector<MotifHit> reverse_hits;
  reverse_hits.reserve(all_hits.size());
  for (const auto &mh : all_hits) {
    if (mh.hit.is_reverse) reverse_hits.push_back(mh);
  }
  const std::vector<MotifHit> &ngrna_pool = !reverse_hits.empty() ? reverse_hits : all_hits;

  for (const auto &mh : all_hits) {
    const auto &hit = mh.hit;
    const int motif_len = static_cast<int>(mh.motif.size());
    // pegRNA should align to the working orientation (non-reverse hits).
    if (hit.is_reverse) continue;

    int spacer_start = hit.is_reverse ? static_cast<int>(hit.pam_idx) + motif_len
                                      : static_cast<int>(hit.pam_idx) - 20;
    if (spacer_start < 0) continue;
    if (spacer_start + 20 > static_cast<int>(seq_view.size())) continue;

    int cut_index_view = spacer_start + 17;  // 3bp upstream of PAM relative to spacer start
    if (cut_index_view < 0 || cut_index_view >= static_cast<int>(seq_view.size())) continue;

    int cut_index_out =
        reverse ? (seq_len - 1 - cut_index_view) : cut_index_view;

    // Ensure edit within allowable distance using original coordinates.
    int distance = std::abs(cut_index_out - edit_start_orig);
    bool edit_far = distance > cfg.max_nick_to_edit_distance;

    // Optional companion ngRNA (PE3/PE3b): pick closest opposite-strand PAM.
    std::optional<NickingSgRNA> ngrna;
    if (cfg.design_ngrna && !ngrna_pool.empty()) {
      int best_dist = std::numeric_limits<int>::max();
      for (const auto &ng_hit : ngrna_pool) {
        const int ng_motif_len = static_cast<int>(ng_hit.motif.size());
        int ng_spacer_start = ng_hit.hit.is_reverse
                                  ? static_cast<int>(ng_hit.hit.pam_idx) + ng_motif_len
                                  : static_cast<int>(ng_hit.hit.pam_idx) - 20;
        if (ng_spacer_start < 0) continue;
        if (ng_spacer_start + 20 > static_cast<int>(seq_view.size())) continue;
        int ng_cut_view = ng_spacer_start + 17;
        if (ng_cut_view < 0 || ng_cut_view >= static_cast<int>(seq_view.size())) continue;
        int ng_cut_out = reverse ? (seq_len - 1 - ng_cut_view) : ng_cut_view;
        if (ng_cut_out == cut_index_out) continue;  // avoid duplicating peg cut
        int delta = std::abs(ng_cut_out - cut_index_out);
        if (delta < best_dist && delta <= cfg.max_nick_to_edit_distance) {
          best_dist = delta;
          std::string ng_spacer = seq_view.substr(ng_spacer_start, 20);
          ngrna = NickingSgRNA{ng_spacer, ng_cut_out, /*is_pe3b=*/false};
        }
      }
    }

    for (int pbs_len = cfg.pbs_min_len; pbs_len <= cfg.pbs_max_len; ++pbs_len) {
      if (cut_index_view - pbs_len < 0) continue;
      std::string pbs_source = seq_view.substr(cut_index_view - pbs_len, pbs_len);
      std::string pbs = reverse_complement(pbs_source);

      for (int rtt_len = cfg.rtt_min_len; rtt_len <= cfg.rtt_max_len; ++rtt_len) {
        if (cut_index_view + rtt_len > static_cast<int>(edited_view.size())) continue;
        // Require RTT to cover edit window in view coordinates.
        if (edit_max_view >= cut_index_view + rtt_len) continue;

        std::string rtt = edited_view.substr(cut_index_view, rtt_len);

        std::string spacer = seq_view.substr(spacer_start, 20);
        PegRNA peg{spacer, cut_index_out, pbs, rtt};

        CandidateHeuristics h{};
        h.pbs_gc = gc_content(pbs);
        h.rtt_gc = gc_content(rtt);
        h.edit_distance_from_nick = distance;
        h.flag_edit_far = edit_far;
        h.flag_pbs_gc_extreme = (h.pbs_gc < 0.3 || h.pbs_gc > 0.75);

        PrimeCandidate cand{peg, ngrna, h};
        out.push_back(std::move(cand));
      }
    }
  }

  // Deterministic order: sort by cut index then spacer lexicographically.
  std::sort(out.begin(), out.end(), [](const PrimeCandidate &a, const PrimeCandidate &b) {
    if (a.peg.cut_index == b.peg.cut_index) return a.peg.spacer < b.peg.spacer;
    return a.peg.cut_index < b.peg.cut_index;
  });

  return out;
}

BatchCandidateList design_prime_edits(const std::vector<PrimeEditSpec> &edits,
                                      const DesignConfig &cfg, const Device &device) {
  BatchCandidateList batch;
  batch.reserve(edits.size());
  for (const auto &e : edits) {
    batch.push_back(design_prime_edit(e, cfg, device));
  }
  return batch;
}

}  // namespace primeforge
