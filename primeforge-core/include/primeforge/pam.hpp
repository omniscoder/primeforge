#pragma once

#include <string>
#include <vector>

#include "primeforge/device.hpp"

namespace primeforge {

// Check if sequence starting at seq[offset] matches motif (e.g., "NGG").
// motif uses 'N' as wildcard. Caller ensures offset+motif.size() <= seq.size().
bool matches_pam(const std::string &seq, size_t offset, const std::string &motif);

// Return PAM start indices for a motif within a sequence.
std::vector<size_t> find_pam_sites(const std::string &seq, const std::string &motif);

// Device-dispatchable PAM scan. GPU version returns same result but may reorder outputs.
std::vector<size_t> find_pam_sites(const std::string &seq, const std::string &motif,
                                   const Device &device);

}  // namespace primeforge
