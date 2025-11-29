#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "primeforge/design.hpp"
#include "primeforge/pam.hpp"
#include "primeforge/device.hpp"

namespace py = pybind11;
using namespace primeforge;

PYBIND11_MODULE(primeforge_bindings, m) {
  py::enum_<DeviceType>(m, "DeviceType")
      .value("CPU", DeviceType::CPU)
      .value("CUDA", DeviceType::CUDA);

  py::class_<Device>(m, "Device")
      .def(py::init([](DeviceType t, int ordinal) { return Device{t, ordinal}; }))
      .def_static("cpu", &Device::cpu)
      .def_static("cuda", &Device::cuda)
      .def_readwrite("type", &Device::type)
      .def_readwrite("ordinal", &Device::ordinal);

  py::enum_<Strand>(m, "Strand")
      .value("Plus", Strand::Plus)
      .value("Minus", Strand::Minus);

  py::class_<EditSubstitution>(m, "EditSubstitution")
      .def(py::init<int, char, char>())
      .def_readwrite("pos", &EditSubstitution::pos)
      .def_readwrite("ref", &EditSubstitution::ref)
      .def_readwrite("alt", &EditSubstitution::alt);

  py::class_<EditInsertion>(m, "EditInsertion")
      .def(py::init<int, std::string>())
      .def_readwrite("pos", &EditInsertion::pos)
      .def_readwrite("inserted", &EditInsertion::inserted);

  py::class_<EditDeletion>(m, "EditDeletion")
      .def(py::init<int, int>())
      .def_readwrite("start", &EditDeletion::start)
      .def_readwrite("length", &EditDeletion::length);

  py::class_<PrimeEditSpec>(m, "PrimeEditSpec")
      .def(py::init<std::string, std::string, std::vector<EditVariant>, Strand>(),
           py::arg("id"), py::arg("ref_sequence"), py::arg("edits"), py::arg("strand") = Strand::Plus)
      .def_readwrite("id", &PrimeEditSpec::id)
      .def_readwrite("ref_sequence", &PrimeEditSpec::ref_sequence)
      .def_readwrite("edits", &PrimeEditSpec::edits)
      .def_readwrite("strand", &PrimeEditSpec::strand);

  py::class_<DesignConfig>(m, "DesignConfig")
      .def(py::init<>())
      .def_readwrite("pbs_min_len", &DesignConfig::pbs_min_len)
      .def_readwrite("pbs_max_len", &DesignConfig::pbs_max_len)
      .def_readwrite("rtt_min_len", &DesignConfig::rtt_min_len)
      .def_readwrite("rtt_max_len", &DesignConfig::rtt_max_len)
      .def_readwrite("max_nick_to_edit_distance", &DesignConfig::max_nick_to_edit_distance)
      .def_readwrite("pam_motifs", &DesignConfig::pam_motifs)
      .def_readwrite("design_ngrna", &DesignConfig::design_ngrna);

  py::class_<PegRNA>(m, "PegRNA")
      .def_readwrite("spacer", &PegRNA::spacer)
      .def_readwrite("cut_index", &PegRNA::cut_index)
      .def_readwrite("pbs", &PegRNA::pbs)
      .def_readwrite("rtt", &PegRNA::rtt);

  py::class_<NickingSgRNA>(m, "NickingSgRNA")
      .def_readwrite("spacer", &NickingSgRNA::spacer)
      .def_readwrite("cut_index", &NickingSgRNA::cut_index)
      .def_readwrite("is_pe3b", &NickingSgRNA::is_pe3b);

  py::class_<CandidateHeuristics>(m, "CandidateHeuristics")
      .def_readwrite("pbs_gc", &CandidateHeuristics::pbs_gc)
      .def_readwrite("rtt_gc", &CandidateHeuristics::rtt_gc)
      .def_readwrite("edit_distance_from_nick", &CandidateHeuristics::edit_distance_from_nick)
      .def_readwrite("flag_pbs_gc_extreme", &CandidateHeuristics::flag_pbs_gc_extreme)
      .def_readwrite("flag_edit_far", &CandidateHeuristics::flag_edit_far);

  py::class_<PrimeCandidate>(m, "PrimeCandidate")
      .def_readwrite("peg", &PrimeCandidate::peg)
      .def_readwrite("ngrna", &PrimeCandidate::ngrna)
      .def_readwrite("heuristics", &PrimeCandidate::heuristics);

  m.def("design_prime_edit", &design_prime_edit, py::arg("edit"), py::arg("cfg"),
        py::arg("device") = Device::cpu());
  m.def("design_prime_edits", &design_prime_edits, py::arg("edits"), py::arg("cfg"),
        py::arg("device") = Device::cpu());
  m.def("is_cuda_available", &is_cuda_available);
}
