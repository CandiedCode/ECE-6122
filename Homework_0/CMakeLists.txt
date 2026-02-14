# Homework_0 CMakeLists.txt
# Inherits C++ standard, compiler options, and GoogleTest from parent

# Create executables for each problem
add_executable(Problem1 Problem1.cpp)
add_executable(Problem2 Problem2.cpp)
add_executable(Problem3 Problem3.cpp)
add_executable(Problem4 Problem4.cpp)

# Create test executable (comment out for now - requires refactoring Problem1.cpp to separate main)
# To enable testing:
# 1. Move functions from Problem1.cpp to a separate Problem1_lib.cpp without main()
# 2. Update Problem1.cpp to call those functions in main()
# 3. Uncomment lines below and update to link Problem1_lib.cpp
#
# add_executable(Problem1_test Problem1_test.cpp Problem1_lib.cpp)
# target_link_libraries(Problem1_test GTest::gtest_main)
# include(GoogleTest)
# gtest_discover_tests(Problem1_test)

# Create a convenience target to build all Homework_0 executables
add_custom_target(Homework_0 DEPENDS Problem1 Problem2 Problem3 Problem4)

