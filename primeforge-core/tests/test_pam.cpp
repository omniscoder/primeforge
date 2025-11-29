#include <cassert>
#include <vector>
#include <string>

#include "primeforge/pam.hpp"

int main() {
  using primeforge::matches_pam;
  using primeforge::find_pam_sites;

  const std::string seq = "AAGTCCAGGTTACCGG";
  assert(matches_pam(seq, 6, "NGG"));
  assert(!matches_pam(seq, 0, "NGG"));

  auto hits = find_pam_sites(seq, "NGG");
  assert(hits.size() == 2);
  assert(hits[0] == 6);
  assert(hits[1] == 13);

  return 0;
}
