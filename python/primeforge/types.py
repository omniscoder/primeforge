from __future__ import annotations

from dataclasses import dataclass, field
from enum import Enum
from typing import List, Optional, Union


class Strand(str, Enum):
    PLUS = "plus"
    MINUS = "minus"


class DeviceType(str, Enum):
    CPU = "cpu"
    CUDA = "cuda"


@dataclass
class Device:
    type: DeviceType = DeviceType.CPU
    ordinal: int = 0

    @staticmethod
    def cpu() -> "Device":
        return Device(DeviceType.CPU, 0)

    @staticmethod
    def cuda(idx: int = 0) -> "Device":
        return Device(DeviceType.CUDA, idx)


@dataclass
class EditSubstitution:
    pos: int
    ref: str
    alt: str


@dataclass
class EditInsertion:
    pos: int
    inserted: str


@dataclass
class EditDeletion:
    start: int
    length: int


EditVariant = Union[EditSubstitution, EditInsertion, EditDeletion]


@dataclass
class PrimeEditSpec:
    id: str
    ref_sequence: str
    edits: List[EditVariant] = field(default_factory=list)
    strand: Strand = Strand.PLUS


@dataclass
class DesignConfig:
    pbs_min_len: int = 8
    pbs_max_len: int = 17
    rtt_min_len: int = 10
    rtt_max_len: int = 40
    max_nick_to_edit_distance: int = 30
    pam_motifs: List[str] = field(default_factory=lambda: ["NGG"])
    design_ngrna: bool = False


@dataclass
class PegRNA:
    spacer: str
    cut_index: int
    pbs: str
    rtt: str


@dataclass
class NickingSgRNA:
    spacer: str
    cut_index: int
    is_pe3b: bool


@dataclass
class CandidateHeuristics:
    pbs_gc: float
    rtt_gc: float
    edit_distance_from_nick: int
    flag_pbs_gc_extreme: bool
    flag_edit_far: bool


@dataclass
class PrimeCandidate:
    peg: PegRNA
    ngrna: Optional[NickingSgRNA]
    heuristics: CandidateHeuristics

