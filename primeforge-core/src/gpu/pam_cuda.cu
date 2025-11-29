#include <cuda_runtime.h>

#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>

namespace primeforge {

namespace {

__device__ inline bool matches_at(const char* seq, int n, const char* motif, int m, int idx) {
  if (idx + m > n) return false;
  for (int j = 0; j < m; ++j) {
    char s = seq[idx + j];
    char mm = motif[j];
    if (mm != 'N' && mm != s) return false;
  }
  return true;
}

__global__ void pam_kernel(const char* seq, int n, const char* motif, int m, int* hits) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx >= n - m + 1) return;
  hits[idx] = matches_at(seq, n, motif, m, idx) ? 1 : 0;
}

inline void cuda_check(cudaError_t err, const char* msg) {
  if (err != cudaSuccess) {
    throw std::runtime_error(std::string(msg) + ": " + cudaGetErrorString(err));
  }
}

}  // namespace

std::vector<size_t> find_pam_sites_cuda(const std::string &seq, const std::string &motif) {
  std::vector<size_t> hits;
  if (seq.size() < motif.size()) return hits;

  // Uppercase copies so kernel is case-insensitive without extra branching.
  std::string seq_host = seq;
  std::string motif_host = motif;
  std::transform(seq_host.begin(), seq_host.end(), seq_host.begin(), [](unsigned char c) { return std::toupper(c); });
  std::transform(motif_host.begin(), motif_host.end(), motif_host.begin(), [](unsigned char c) { return std::toupper(c); });

  const int n = static_cast<int>(seq_host.size());
  const int m = static_cast<int>(motif_host.size());
  const int slots = n - m + 1;

  char *d_seq = nullptr, *d_motif = nullptr;
  int *d_hits = nullptr;
  cuda_check(cudaMalloc(&d_seq, n), "cudaMalloc seq");
  cuda_check(cudaMalloc(&d_motif, m), "cudaMalloc motif");
  cuda_check(cudaMalloc(&d_hits, slots * sizeof(int)), "cudaMalloc hits");

  cuda_check(cudaMemcpy(d_seq, seq_host.data(), n, cudaMemcpyHostToDevice), "cudaMemcpy seq");
  cuda_check(cudaMemcpy(d_motif, motif_host.data(), m, cudaMemcpyHostToDevice), "cudaMemcpy motif");

  int threads = 256;
  int blocks = (slots + threads - 1) / threads;
  pam_kernel<<<blocks, threads>>>(d_seq, n, d_motif, m, d_hits);
  cuda_check(cudaGetLastError(), "pam_kernel launch");

  std::vector<int> h_hits(slots, 0);
  cuda_check(cudaMemcpy(h_hits.data(), d_hits, slots * sizeof(int), cudaMemcpyDeviceToHost),
             "cudaMemcpy hits");

  cudaFree(d_seq);
  cudaFree(d_motif);
  cudaFree(d_hits);

  for (int i = 0; i < slots; ++i) {
    if (h_hits[i] == 1) hits.push_back(static_cast<size_t>(i));
  }
  return hits;
}

}  // namespace primeforge
