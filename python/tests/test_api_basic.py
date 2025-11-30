import pytest

pytest.importorskip("primeforge_bindings")

from primeedit import (
    DesignConfig,
    Device,
    EditSubstitution,
    PrimeEditSpec,
    design_prime_edit,
)


def test_design_roundtrip_cpu():
    seq = "ACGTACCGACGTACGTACGTGGGACGTACGTACGTAC"
    edit = PrimeEditSpec(
        id="py-test",
        ref_sequence=seq,
        edits=[EditSubstitution(25, "G", "A")],
    )
    cfg = DesignConfig()
    cfg.pbs_min_len = 10
    cfg.pbs_max_len = 12
    cfg.rtt_min_len = 12
    cfg.rtt_max_len = 18
    cfg.max_nick_to_edit_distance = 25
    cfg.design_ngrna = True

    cands = design_prime_edit(edit, cfg, device=Device.cpu())
    assert len(cands) > 0
    first = cands[0]
    assert len(first.peg.spacer) == 20
    assert first.ngrna is not None
    assert first.heuristics.flag_edit_far is False


def test_device_flag_passthrough():
    # Should not raise even if CUDA not present; falls back inside binding.
    edit = PrimeEditSpec(id="noop", ref_sequence="ACGT" * 10, edits=[])
    cfg = DesignConfig()
    cfg.pam_motifs = ["NGG"]
    cfg.design_ngrna = False
    cands = design_prime_edit(edit, cfg, device=Device.cpu())
    assert isinstance(cands, list)
