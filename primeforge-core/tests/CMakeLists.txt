add_executable(test_pam test_pam.cpp)
target_link_libraries(test_pam PRIVATE primeforge-core)
add_test(NAME test_pam COMMAND test_pam)

add_executable(test_design_basic test_design_basic.cpp)
target_link_libraries(test_design_basic PRIVATE primeforge-core)
add_test(NAME test_design_basic COMMAND test_design_basic)

add_executable(test_e2e test_e2e.cpp)
target_link_libraries(test_e2e PRIVATE primeforge-core)
add_test(NAME test_e2e COMMAND test_e2e)
