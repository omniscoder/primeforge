#include "primeforge/utils.hpp"

#include <algorithm>
#include <cctype>

namespace primeforge {

static char complement(char base) {
  switch (std::toupper(static_cast<unsigned char>(base))) {
    case 'A': return 'T';
    case 'C': return 'G';
    case 'G': return 'C';
    case 'T': return 'A';
    default: return 'N';
  }
}

std::string reverse_complement(const std::string &seq) {
  std::string rc;
  rc.reserve(seq.size());
  for (auto it = seq.rbegin(); it != seq.rend(); ++it) {
    rc.push_back(complement(*it));
  }
  return rc;
}

double gc_content(const std::string &seq) {
  if (seq.empty()) return 0.0;
  int gc = 0;
  for (char c : seq) {
    char u = std::toupper(static_cast<unsigned char>(c));
    if (u == 'G' || u == 'C') ++gc;
  }
  return static_cast<double>(gc) / static_cast<double>(seq.size());
}

}  // namespace primeforge
