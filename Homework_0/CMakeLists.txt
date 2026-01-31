cmake_minimum_required(VERSION 3.15)
project(Homework_0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Create executables for each problem
add_executable(Problem1 Problem1.cpp)
# add_executable(Problem2 Problem2.cpp)
# add_executable(Problem3 Problem3.cpp)
# add_executable(Problem4 Problem4.cpp)

# Optional: Add compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Problem1 PRIVATE -Wall -Wextra -pedantic)
    # target_compile_options(Problem2 PRIVATE -Wall -Wextra -pedantic)
    # target_compile_options(Problem3 PRIVATE -Wall -Wextra -pedantic)
    # target_compile_options(Problem4 PRIVATE -Wall -Wextra -pedantic)
endif()

# Create test executable
add_executable(Problem1_test Problem1_test.cpp)
target_link_libraries(Problem1_test GTest::gtest_main)

# Add test to CTest
include(GoogleTest)
gtest_discover_tests(Problem1_test)

