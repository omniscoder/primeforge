cmake_minimum_required(VERSION 3.20)

find_package(Python3 COMPONENTS Interpreter Development.Module QUIET)
if(NOT Python3_Interpreter_FOUND OR NOT Python3_Development_Module_FOUND)
  find_package(Python3 COMPONENTS Interpreter Development QUIET)
endif()

if(NOT Python3_Interpreter_FOUND OR (NOT Python3_Development_Module_FOUND AND NOT Python3_Development_FOUND))
  message(WARNING "Python3 interpreter/dev headers not found; skipping primeforge_bindings")
  return()
endif()

find_package(pybind11 CONFIG QUIET)
if(NOT pybind11_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG v2.11.1
  )
  FetchContent_MakeAvailable(pybind11)
endif()

pybind11_add_module(primeforge_bindings
  bindings.cpp
)

target_link_libraries(primeforge_bindings PRIVATE primeforge-core)
target_link_libraries(primeforge_bindings PRIVATE Python3::Module)
if(PRIMEFORGE_ENABLE_CUDA)
  find_package(CUDAToolkit REQUIRED)
  target_link_libraries(primeforge_bindings PRIVATE CUDA::cudart CUDA::cuda_driver)
endif()

target_compile_definitions(primeforge_bindings PRIVATE
  $<$<BOOL:${PRIMEFORGE_ENABLE_CUDA}>:PRIMEFORGE_ENABLE_CUDA>
)

install(TARGETS primeforge_bindings
        LIBRARY DESTINATION primeforge)
