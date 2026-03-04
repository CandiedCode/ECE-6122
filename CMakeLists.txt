cmake_minimum_required(VERSION 3.22)
project(ECE6122 LANGUAGES CXX)

# Set C++ standard globally
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Option to use FetchContent for SFML or a local SFML directory
set(SFML_LOCAL_DIR "${CMAKE_SOURCE_DIR}/SFML")

# Default to fetching SFML on macOS, local SFML elsewhere.
# User can override with -DUSE_FETCHED_SFML=ON/OFF.
if(NOT DEFINED USE_FETCHED_SFML)
    if(APPLE)
        set(USE_FETCHED_SFML ON CACHE BOOL "Fetch SFML via FetchContent instead of using local SFML/" FORCE)
    else()
        set(USE_FETCHED_SFML OFF CACHE BOOL "Fetch SFML via FetchContent instead of using local SFML/" FORCE)
    endif()
else()
    # Ensure it appears in cache with a helpful description (no FORCE here)
    set(USE_FETCHED_SFML "${USE_FETCHED_SFML}" CACHE BOOL "Fetch SFML via FetchContent instead of using local SFML/")
endif()

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

# Define build type
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Configuring for Debug build")
    add_compile_definitions(BUILD_TYPE_DEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Configuring for Release build")
    add_compile_definitions(BUILD_TYPE_RELEASE)
endif()

if(USE_FETCHED_SFML)
    message(STATUS "SFML: FetchContent mode enabled (USE_FETCHED_SFML=ON)")
    set(SFML_PROVIDED_BY_ROOT OFF CACHE INTERNAL "SFML provided by root")
else()
    if(EXISTS "${SFML_LOCAL_DIR}/CMakeLists.txt")
        add_subdirectory(SFML)
        set(SFML_PROVIDED_BY_ROOT ON CACHE INTERNAL "SFML provided by root")
    else()
        message(FATAL_ERROR
            "SFML: local folder not found at ${SFML_LOCAL_DIR}. "
            "Either add it, or configure with -DUSE_FETCHED_SFML=ON")
    endif()
endif()

# add_subdirectory(Homework_0)
# add_subdirectory(Homework_1)
add_subdirectory(Homework_2)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)
