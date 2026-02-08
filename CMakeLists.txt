cmake_minimum_required(VERSION 3.22)
project(ECE6122 LANGUAGES CXX)

# Set C++ standard globally
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Load Conan-generated files from the build generators directory
# The layout directive in conanfile.txt uses cmake_layout
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/build/Release/generators")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/build/Release/generators")

# Add compiler warnings globally
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Find packages managed by Conan
find_package(GTest REQUIRED)
find_package(SFML REQUIRED COMPONENTS graphics window system)

# Enable testing
enable_testing()

# Add homework subdirectories
add_subdirectory(Homework_0)
add_subdirectory(Homework_1)
