cmake_minimum_required(VERSION 3.20)
project(primeforge LANGUAGES CXX)

option(PRIMEFORGE_ENABLE_CUDA "Enable CUDA backends" OFF)
option(PRIMEFORGE_BUILD_PYTHON "Build Python bindings" OFF)
option(PRIMEFORGE_RUN_BENCH "Run benchmark checks in ctest (requires CUDA when enabled)" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(primeforge-core)

if(PRIMEFORGE_BUILD_PYTHON)
  add_subdirectory(python/primeforge_bindings)
endif()

include(CTest)
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(primeforge-core/tests)
  if(PRIMEFORGE_BUILD_PYTHON)
    find_package(Python3 COMPONENTS Interpreter)
    if(Python3_Interpreter_FOUND)
      execute_process(COMMAND ${Python3_EXECUTABLE} -c "import importlib.util, sys; sys.exit(0 if importlib.util.find_spec('pytest') else 1)" RESULT_VARIABLE PYTEST_FOUND)
      if(PYTEST_FOUND EQUAL 0)
        add_test(NAME python-pytest
          COMMAND ${Python3_EXECUTABLE} -m pytest ${CMAKE_SOURCE_DIR}/python/tests
        )
        set_tests_properties(python-pytest PROPERTIES
          ENVIRONMENT "PYTHONPATH=${CMAKE_SOURCE_DIR}/python:${CMAKE_BINARY_DIR}/python/primeforge_bindings:$ENV{PYTHONPATH}")
      else()
        message(STATUS "pytest not found; skipping python-pytest")
      endif()
    endif()
  endif()

  if(PRIMEFORGE_RUN_BENCH AND PRIMEFORGE_ENABLE_CUDA)
    add_test(NAME bench_pam_cuda
      COMMAND ${CMAKE_COMMAND} -E env DEVICE=cuda ${CMAKE_BINARY_DIR}/primeforge-core/benchmarks/bench_pam 1000000 NGG 2
    )
    set_tests_properties(bench_pam_cuda PROPERTIES
      PASS_REGULAR_EXPRESSION "throughput_mb_s=[1-9]"
      TIMEOUT 60
    )
  endif()
endif()
