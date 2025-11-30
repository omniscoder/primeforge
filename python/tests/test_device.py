import pytest

pytest.importorskip("primeforge_bindings")

from primeedit import Device, DeviceType, is_cuda_available


def test_device_helpers():
    cpu = Device.cpu()
    assert cpu.type == DeviceType.CPU
    assert cpu.ordinal == 0
    cuda0 = Device.cuda(0)
    assert cuda0.type == DeviceType.CUDA
    assert cuda0.ordinal == 0


def test_is_cuda_available_runs():
    # Value may be False on CPU-only systems; ensure callable.
    assert is_cuda_available() in (True, False)
