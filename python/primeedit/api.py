from __future__ import annotations

from typing import List

from .types import (
    Device,
    DeviceType,
    EditDeletion,
    EditInsertion,
    EditSubstitution,
    PrimeCandidate,
    PrimeEditSpec,
    DesignConfig,
    Strand,
)

try:  # pragma: no cover - compiled extension detection
    from primeforge_bindings import (
        design_prime_edit as _c_design,
        design_prime_edits as _c_design_batch,
        is_cuda_available as _c_is_cuda_available,
        Device as _CDevice,
        DeviceType as _CDeviceType,
        EditDeletion as _CEditDeletion,
        EditInsertion as _CEditInsertion,
        EditSubstitution as _CEditSubstitution,
        PrimeEditSpec as _CPrimeEditSpec,
        DesignConfig as _CDesignConfig,
        Strand as _CStrand,
    )
except ImportError:  # pragma: no cover
    _c_design = None
    _c_design_batch = None
    _c_is_cuda_available = lambda: False
    _CDevice = _CDeviceType = _CEditDeletion = _CEditInsertion = _CEditSubstitution = None
    _CPrimeEditSpec = _CDesignConfig = _CStrand = None


def _to_c_device(dev: Device | None):
    d = dev or Device.cpu()
    if _CDevice is None:
        raise RuntimeError("primeforge bindings not built; rebuild with PRIMEFORGE_BUILD_PYTHON=ON")
    if isinstance(d, _CDevice):
        return d
    c_type = _CDeviceType.CPU if d.type == DeviceType.CPU else _CDeviceType.CUDA
    return _CDevice(c_type, d.ordinal)


def _to_c_edit(edit: EditSubstitution | EditInsertion | EditDeletion):
    if isinstance(edit, _CEditSubstitution) or isinstance(edit, _CEditInsertion) or isinstance(edit, _CEditDeletion):
        return edit
    if isinstance(edit, EditSubstitution):
        return _CEditSubstitution(edit.pos, edit.ref, edit.alt)
    if isinstance(edit, EditInsertion):
        return _CEditInsertion(edit.pos, edit.inserted)
    if isinstance(edit, EditDeletion):
        return _CEditDeletion(edit.start, edit.length)
    raise TypeError(f"Unsupported edit type {type(edit)}")


def _to_c_strand(strand: Strand):
    if strand == Strand.MINUS:
        return _CStrand.Minus
    return _CStrand.Plus


def _to_c_design_config(cfg: DesignConfig):
    if isinstance(cfg, _CDesignConfig):
        return cfg
    c_cfg = _CDesignConfig()
    c_cfg.pbs_min_len = cfg.pbs_min_len
    c_cfg.pbs_max_len = cfg.pbs_max_len
    c_cfg.rtt_min_len = cfg.rtt_min_len
    c_cfg.rtt_max_len = cfg.rtt_max_len
    c_cfg.max_nick_to_edit_distance = cfg.max_nick_to_edit_distance
    c_cfg.pam_motifs = cfg.pam_motifs
    c_cfg.design_ngrna = cfg.design_ngrna
    return c_cfg


def _to_c_edit_spec(edit: PrimeEditSpec):
    if isinstance(edit, _CPrimeEditSpec):
        return edit
    edits_c = [_to_c_edit(e) for e in edit.edits]
    return _CPrimeEditSpec(edit.id, edit.ref_sequence, edits_c, _to_c_strand(edit.strand))


def design_prime_edit(edit: PrimeEditSpec, cfg: DesignConfig, device: Device | None = None) -> List[PrimeCandidate]:
    if _c_design is None:
        raise RuntimeError("primeforge bindings not built; rebuild with PRIMEFORGE_BUILD_PYTHON=ON")
    return _c_design(_to_c_edit_spec(edit), _to_c_design_config(cfg), _to_c_device(device))


def design_prime_edits(edits: List[PrimeEditSpec], cfg: DesignConfig, device: Device | None = None) -> List[List[PrimeCandidate]]:
    if _c_design_batch is None:
        raise RuntimeError("primeforge bindings not built; rebuild with PRIMEFORGE_BUILD_PYTHON=ON")
    c_edits = [_to_c_edit_spec(e) for e in edits]
    return _c_design_batch(c_edits, _to_c_design_config(cfg), _to_c_device(device))


def is_cuda_available() -> bool:
    return _c_is_cuda_available()
