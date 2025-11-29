#pragma once

#include "primeforge/types.hpp"

namespace primeforge {

struct Device {
  DeviceType type{DeviceType::CPU};
  int ordinal{0};

  static Device cpu() { return Device{DeviceType::CPU, 0}; }
  static Device cuda(int idx = 0) { return Device{DeviceType::CUDA, idx}; }
};

// Returns true if CUDA runtime is compiled in and a device is available.
bool is_cuda_available();

}  // namespace primeforge
