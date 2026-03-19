cmake_minimum_required(VERSION 3.22)
project(ECE6122 LANGUAGES CXX C)

# Set C++ standard globally
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Suppress deprecated register keyword warning from external libraries
add_compile_options(-Wno-register)

# Generate compile_commands.json for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# To universally catch if this is debug/release build
add_compile_definitions(OPT_LEVEL=${CMAKE_CXX_FLAGS})

# Define a common output directory
set(COMMON_OUTPUT_DIR "${CMAKE_BINARY_DIR}/output")
if(WIN32)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${COMMON_OUTPUT_DIR}/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${COMMON_OUTPUT_DIR}/lib")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${COMMON_OUTPUT_DIR}/bin")

    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${COMMON_OUTPUT_DIR}/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${COMMON_OUTPUT_DIR}/lib")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${COMMON_OUTPUT_DIR}/bin")
elseif(UNIX)
    # Set output directories for different types of binaries
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/bin")  # For executables
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/lib")  # For shared libraries
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/lib")  # For static libraries
endif()

add_subdirectory(Homework_3)
add_subdirectory(external)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)
