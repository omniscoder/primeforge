add_library(primeforge-core
  src/utils.cpp
  src/pam.cpp
  src/design.cpp
  src/device.cpp
)

target_include_directories(primeforge-core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(primeforge-core PUBLIC cxx_std_20)

target_compile_options(primeforge-core PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
)

target_link_libraries(primeforge-core PUBLIC)

target_compile_definitions(primeforge-core PUBLIC
  $<$<BOOL:${PRIMEFORGE_ENABLE_CUDA}>:PRIMEFORGE_ENABLE_CUDA>
)

if(PRIMEFORGE_ENABLE_CUDA)
  enable_language(CUDA)
  add_library(primeforge-cuda STATIC
    src/gpu/pam_cuda.cu
  )
  set_target_properties(primeforge-cuda PROPERTIES POSITION_INDEPENDENT_CODE ON)
  target_include_directories(primeforge-cuda PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
  target_compile_features(primeforge-cuda PUBLIC cxx_std_17)
  target_compile_options(primeforge-cuda PRIVATE
    $<$<COMPILE_LANGUAGE:CUDA>:-lineinfo>
  )
  find_package(CUDAToolkit REQUIRED)
  target_link_libraries(primeforge-cuda PUBLIC CUDA::cudart CUDA::cuda_driver)
  target_link_libraries(primeforge-core PUBLIC primeforge-cuda CUDA::cudart CUDA::cuda_driver)
endif()

add_subdirectory(benchmarks)
