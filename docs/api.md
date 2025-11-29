# API sketch

C++
```cpp
DesignConfig cfg;
PrimeEditSpec edit{/*id*/ "example", /*ref_sequence*/ window, /*edits*/ {...}, Strand::Plus};
auto candidates = design_prime_edit(edit, cfg, Device::cpu());
```

Python
```python
from primeforge import DesignConfig, PrimeEditSpec, EditSubstitution, design_prime_edit, Device

cfg = DesignConfig()
edit = PrimeEditSpec(id="example", ref_sequence=window, edits=[EditSubstitution(25, "G", "A")])
cands = design_prime_edit(edit, cfg, device=Device.cpu())
```

Device handling
- CPU is default.
- When built with CUDA, pass `Device.cuda(0)` to request GPU backends; the binding falls back to CPU if CUDA is not compiled in or unavailable.
```
from primeforge import is_cuda_available
print(is_cuda_available())
```
