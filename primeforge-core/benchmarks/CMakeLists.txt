option(PRIMEFORGE_BUILD_BENCHMARKS "Build benchmark executables" OFF)

if(NOT PRIMEFORGE_BUILD_BENCHMARKS)
  return()
endif()

add_executable(bench_pam bench_pam.cpp)
target_link_libraries(bench_pam PRIVATE primeforge-core)

target_compile_definitions(bench_pam PRIVATE
  $<$<BOOL:${PRIMEFORGE_ENABLE_CUDA}>:PRIMEFORGE_ENABLE_CUDA>
)
