"""primeedit Python bindings (stub).

CPU-first deterministic prime editing design. CUDA backends are optional and
will be enabled when the extension is built with `PRIMEFORGE_ENABLE_CUDA`.
"""

from .types import (
    EditSubstitution,
    EditInsertion,
    EditDeletion,
    PrimeEditSpec,
    DesignConfig,
    Device,
    DeviceType,
)
from .api import design_prime_edit, design_prime_edits
from .api import is_cuda_available

__all__ = [
    "EditSubstitution",
    "EditInsertion",
    "EditDeletion",
    "PrimeEditSpec",
    "DesignConfig",
    "Device",
    "DeviceType",
    "design_prime_edit",
    "design_prime_edits",
    "is_cuda_available",
]
