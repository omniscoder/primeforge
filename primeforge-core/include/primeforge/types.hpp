#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace primeforge {

enum class Strand { Plus, Minus };

enum class DeviceType { CPU, CUDA };

struct EditSubstitution {
  int pos{};  // 0-based index into ref_sequence
  char ref{};
  char alt{};
};

struct EditInsertion {
  int pos{};  // insertion occurs before this index
  std::string inserted;
};

struct EditDeletion {
  int start{};  // inclusive
  int length{};
};

using EditVariant = std::variant<EditSubstitution, EditInsertion, EditDeletion>;

struct PrimeEditSpec {
  std::string id;
  std::string ref_sequence;            // local window
  std::vector<EditVariant> edits;
  Strand strand{Strand::Plus};
};

struct DesignConfig {
  int pbs_min_len{8};
  int pbs_max_len{17};
  int rtt_min_len{10};
  int rtt_max_len{40};
  int max_nick_to_edit_distance{30};
  std::vector<std::string> pam_motifs{"NGG"};
  bool design_ngrna{false};
};

struct PegRNA {
  std::string spacer;   // 20bp guide excluding PAM
  int cut_index{};      // 0-based cut position in ref_sequence
  std::string pbs;
  std::string rtt;
};

struct NickingSgRNA {
  std::string spacer;
  int cut_index{};
  bool is_pe3b{false};
};

struct CandidateHeuristics {
  double pbs_gc{0.0};
  double rtt_gc{0.0};
  int edit_distance_from_nick{0};
  bool flag_pbs_gc_extreme{false};
  bool flag_edit_far{false};
};

struct PrimeCandidate {
  PegRNA peg;
  std::optional<NickingSgRNA> ngrna;
  CandidateHeuristics heuristics;
};

// Convenience typedefs
using CandidateList = std::vector<PrimeCandidate>;
using BatchCandidateList = std::vector<CandidateList>;

} // namespace primeforge
