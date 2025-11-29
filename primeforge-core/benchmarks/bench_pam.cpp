#include <chrono>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "primeforge/pam.hpp"
#include "primeforge/device.hpp"

using namespace primeforge;

std::string random_dna(size_t n) {
  static const char bases[4] = {'A', 'C', 'G', 'T'};
  std::mt19937 rng(42);
  std::uniform_int_distribution<int> dist(0, 3);
  std::string s;
  s.reserve(n);
  for (size_t i = 0; i < n; ++i) s.push_back(bases[dist(rng)]);
  return s;
}

int main(int argc, char** argv) {
  size_t len = (argc > 1) ? std::stoull(argv[1]) : 5'000'000; // 5 Mb default
  std::string motif = (argc > 2) ? argv[2] : "NGG";
  int iters = (argc > 3) ? std::stoi(argv[3]) : 2;  // first run warms up
  std::string seq = random_dna(len);

  const char* device_env = std::getenv("DEVICE");
  bool use_cuda = device_env && std::string(device_env) == "cuda";
  Device dev = use_cuda ? Device::cuda(0) : Device::cpu();

  // Warm-up + measured iterations.
  std::vector<double> times_ms;
  for (int i = 0; i < iters; ++i) {
    auto t0 = std::chrono::high_resolution_clock::now();
    auto hits = find_pam_sites(seq, motif, dev);
    auto t1 = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
    if (i == 0) continue; // warm-up; ignore
    times_ms.push_back(ms);
  }

  double ms = times_ms.empty() ? 0.0 : times_ms.front();
  double mbps = times_ms.empty() ? 0.0 : (static_cast<double>(len) / 1e6) / (ms / 1000.0);
  std::cout << "Device=" << (use_cuda ? "CUDA" : "CPU")
            << " len=" << len
            << " motif=" << motif
            << " time_ms=" << ms
            << " throughput_mb_s=" << mbps
            << " iterations=" << iters
            << "\n";
  return 0;
}
