#pragma once

#include <string>

namespace primeforge {

// Reverse complement a DNA sequence (A/C/G/T only). Non-ACGT chars map to 'N'.
std::string reverse_complement(const std::string &seq);

// Compute GC fraction in [0,1]; empty input returns 0.0.
double gc_content(const std::string &seq);

} // namespace primeforge
