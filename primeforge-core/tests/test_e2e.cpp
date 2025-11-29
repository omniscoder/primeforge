#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "primeforge/design.hpp"
#include "primeforge/types.hpp"

using namespace primeforge;

int main() {
  // Scenario: plus-strand substitution with PE3 companion allowed.
  const std::string seq = "TTTACGTACCGACGTACGTCCGTTTGGGACGTACGTACGTACGTT";
  PrimeEditSpec spec{
      .id = "e2e-sub",
      .ref_sequence = seq,
      .edits = {EditSubstitution{25, 'G', 'A'}},
      .strand = Strand::Plus,
  };

  DesignConfig cfg{};
  cfg.pbs_min_len = 10;
  cfg.pbs_max_len = 15;
  cfg.rtt_min_len = 12;
  cfg.rtt_max_len = 25;
  cfg.max_nick_to_edit_distance = 30;
  cfg.design_ngrna = true;

  auto cands = design_prime_edit(spec, cfg, Device::cpu());
  assert(!cands.empty());

  // Deterministic ordering and RTT coverage check.
  const auto &c0 = cands.front();
  assert(c0.peg.spacer.size() == 20);
  assert(c0.peg.rtt.size() >= 12);
  assert(c0.heuristics.edit_distance_from_nick <= cfg.max_nick_to_edit_distance);
  if (cfg.design_ngrna) {
    assert(c0.ngrna.has_value());
    assert(c0.ngrna->spacer.size() == 20);
  }

  // Minus-strand deletion to exercise reverse logic.
  PrimeEditSpec spec_minus{
      .id = "e2e-del",
      .ref_sequence = seq,
      .edits = {EditDeletion{18, 3}},
      .strand = Strand::Minus,
  };
  auto cands_minus = design_prime_edit(spec_minus, cfg, Device::cpu());
  assert(!cands_minus.empty());
  const auto &m0 = cands_minus.front();
  assert(m0.peg.spacer.size() == 20);
  assert(m0.peg.cut_index >= 0);
  assert(m0.peg.rtt.size() >= cfg.rtt_min_len);

  std::cout << "e2e tests passed with " << cands.size() << " + " << cands_minus.size()
            << " candidates\n";
  return 0;
}
