#include "primeforge/device.hpp"

#ifdef PRIMEFORGE_ENABLE_CUDA
#include <cuda_runtime.h>
#endif

namespace primeforge {

bool is_cuda_available() {
#ifdef PRIMEFORGE_ENABLE_CUDA
  int count = 0;
  cudaError_t err = cudaGetDeviceCount(&count);
  return (err == cudaSuccess) && (count > 0);
#else
  return false;
#endif
}

}  // namespace primeforge
