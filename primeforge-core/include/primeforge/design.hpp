#pragma once

#include "primeforge/types.hpp"
#include "primeforge/device.hpp"

namespace primeforge {

CandidateList design_prime_edit(const PrimeEditSpec &edit, const DesignConfig &cfg,
                                const Device &device = Device::cpu());

BatchCandidateList design_prime_edits(const std::vector<PrimeEditSpec> &edits,
                                      const DesignConfig &cfg,
                                      const Device &device = Device::cpu());

}  // namespace primeforge
