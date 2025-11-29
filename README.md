# primeforge

Deterministic prime-editing design core in modern C++ with a thin Python API. v0.1 focuses on clean data models, pegRNA/NGG design logic for PE2/PE3, and batch-friendly interfaces. CPU-first, with optional CUDA backends planned for genome-scale PAM scans and batched scoring.

## Status
- Core scaffolding, device abstraction, and CPU utilities in place.
- Deterministic pegRNA enumeration supports plus/minus strands, PBS/RTT ranges, and optional PE3 companion nicking guides.
- CUDA is disabled by default; enable with `-DPRIMEFORGE_ENABLE_CUDA=ON` once GPU kernels land.
- Python bindings are stubbed to mirror the C++ API shape.

## Build (C++)
```bash
cmake -S . -B build -DPRIMEFORGE_ENABLE_CUDA=OFF
cmake --build build
ctest --test-dir build  # runs C++ (and Python if bindings + pytest present)
```

## Python (dev)
```bash
pip install -r python/requirements-dev.txt
# build/install once bindings are implemented
# from repo root:
# cmake -S . -B build -DPRIMEFORGE_BUILD_PYTHON=ON -DPRIMEFORGE_ENABLE_CUDA=OFF
# cmake --build build
# pytest python/tests  (PYTHONPATH=build:$PYTHONPATH)
```

## Benchmarks (optional)
PAM scan throughput (CPU by default, set `DEVICE=cuda` when built with CUDA):
```bash
cmake -S . -B build -DPRIMEFORGE_BUILD_BENCHMARKS=ON -DPRIMEFORGE_ENABLE_CUDA=OFF
cmake --build build --target bench_pam
./build/primeforge-core/benchmarks/bench_pam 5000000 NGG 3  # args: length motif iterations (first is warm-up)
# With CUDA (example for GTX 1060, compute 6.1):
# CUDACXX=/usr/bin/nvcc cmake -S . -B build -DPRIMEFORGE_ENABLE_CUDA=ON -DCMAKE_CUDA_ARCHITECTURES=61 -DPRIMEFORGE_BUILD_BENCHMARKS=ON
# cmake --build build
# DEVICE=cuda ./build/primeforge-core/benchmarks/bench_pam 5000000 NGG 3
# Recent run (GTX 1060, NGG, 5 Mb): CPU ~12 Mb/s, CUDA ~19 Mb/s (post warm-up)
```
To gate benchmarks in CI: add `-DPRIMEFORGE_RUN_BENCH=ON` (requires CUDA build) and ctest will run a short CUDA PAM check.

## Design goals
- Explicit edit model (subs/ins/del) instead of magic strings.
- pegRNA anatomy first-class: spacer, PBS, RTT, nicking guides.
- Deterministic rule-based design; ML hooks added without entangling the core.
- Library-scale batch APIs; zero I/O in the C++ core.

## CUDA roadmap (high level)
- GPU PAM scanning for genome-scale sweeps.
- Batched thermo/secondary-structure heuristics and ML scoring.
- Variant-aware masking for personalized genomes.

See `docs/design_rules.md` for the rule set and `docs/api.md` for the API sketch.
