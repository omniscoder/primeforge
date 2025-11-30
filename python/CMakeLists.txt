cmake_minimum_required(VERSION 3.20)
project(primeedit LANGUAGES CXX)

# Pass through options; defaults are CPU-only for wheel/conda builds.
option(PRIMEFORGE_ENABLE_CUDA "Enable CUDA backends" OFF)
option(PRIMEFORGE_BUILD_BENCHMARKS "Build benchmark executables" OFF)
option(PRIMEFORGE_RUN_BENCH "Run benchmark checks" OFF)
set(PRIMEFORGE_BUILD_PYTHON ON CACHE BOOL "Build Python bindings")

# Core library (lives one level up)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../primeforge-core ${CMAKE_BINARY_DIR}/primeforge-core)

# Python bindings inside this tree
add_subdirectory(primeforge_bindings)
