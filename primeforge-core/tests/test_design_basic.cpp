#include <cassert>
#include <iostream>

#include "primeforge/design.hpp"

int main() {
  using namespace primeforge;

  const std::string seq = "ACGTACCGACGTACGTACGTGGGACGTACGTACGTAC";
  PrimeEditSpec spec{
      .id = "test-edit",
      .ref_sequence = seq,
      .edits = {EditSubstitution{25, 'G', 'A'}},
      .strand = Strand::Plus,
  };

  DesignConfig cfg{};
  cfg.pbs_min_len = 10;
  cfg.pbs_max_len = 12;
  cfg.rtt_min_len = 12;
  cfg.rtt_max_len = 18;
  cfg.max_nick_to_edit_distance = 25;
  cfg.design_ngrna = true;

  auto cands = design_prime_edit(spec, cfg, Device::cpu());
  assert(!cands.empty());

  const auto &first = cands.front();
  assert(first.peg.spacer.size() == 20);
  assert(first.peg.cut_index == 17);
  assert(first.heuristics.flag_edit_far == false);
  assert(first.heuristics.pbs_gc >= 0.0 && first.heuristics.pbs_gc <= 1.0);

  for (const auto &c : cands) {
    // RTT must cover edit site (position 25) relative to cut 23.
    assert(static_cast<int>(c.peg.rtt.size()) >= 2);
    assert(c.heuristics.edit_distance_from_nick <= cfg.max_nick_to_edit_distance);
    if (cfg.design_ngrna) {
      assert(c.ngrna.has_value());
    }
  }

  return 0;
}
