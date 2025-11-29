#include "primeforge/pam.hpp"

#include <cctype>
#include <stdexcept>

namespace primeforge {

bool matches_pam(const std::string &seq, size_t offset, const std::string &motif) {
  if (offset + motif.size() > seq.size()) return false;
  for (size_t i = 0; i < motif.size(); ++i) {
    char s = std::toupper(static_cast<unsigned char>(seq[offset + i]));
    char m = std::toupper(static_cast<unsigned char>(motif[i]));
    if (m == 'N') continue;
    if (s != m) return false;
  }
  return true;
}

std::vector<size_t> find_pam_sites_cpu(const std::string &seq, const std::string &motif) {
  std::vector<size_t> hits;
  if (seq.size() < motif.size()) return hits;
  for (size_t i = 0; i + motif.size() <= seq.size(); ++i) {
    if (matches_pam(seq, i, motif)) hits.push_back(i);
  }
  return hits;
}

#ifdef PRIMEFORGE_ENABLE_CUDA
std::vector<size_t> find_pam_sites_cuda(const std::string &seq, const std::string &motif);
#endif

std::vector<size_t> find_pam_sites(const std::string &seq, const std::string &motif) {
  return find_pam_sites_cpu(seq, motif);
}

std::vector<size_t> find_pam_sites(const std::string &seq, const std::string &motif,
                                   const Device &device) {
  if (device.type == DeviceType::CUDA) {
#ifdef PRIMEFORGE_ENABLE_CUDA
    return find_pam_sites_cuda(seq, motif);
#else
    // CUDA requested but not compiled in; fall back.
    return find_pam_sites_cpu(seq, motif);
#endif
  }
  return find_pam_sites_cpu(seq, motif);
}

}  // namespace primeforge
