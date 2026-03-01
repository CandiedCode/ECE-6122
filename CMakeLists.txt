cmake_minimum_required(VERSION 3.22)
project(ECE6122 LANGUAGES CXX)

# Set C++ standard globally
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# # Add strict compiler warnings and checks globally
# if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
#     add_compile_options(
#         -Wall                                    # Enable all standard warnings
#         -Wextra                                  # Enable extra warnings
#         -pedantic                                # Strict ISO C++ compliance
#         -Wshadow                                 # Warn about variable shadowing
#         # -Wconversion                             # Warn about implicit conversions
#         # -Wsign-conversion                        # Warn about sign conversions
#         -Wold-style-cast                         # Warn about C-style casts
#         -Woverloaded-virtual                     # Warn about overloaded virtuals
#         -Wnon-virtual-dtor                       # Warn about non-virtual destructors
#         -Wformat=2                               # Strict format checking
#         -Werror=format-security                  # Error on format security issues
#         -Werror=undef                            # Error on undefined macros
#     )
# endif()

# # Additional strict flags for GCC
# if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
#     add_compile_options(
#         -Wduplicated-branches                    # Warn about duplicate branches
#         -Wduplicated-cond                        # Warn about duplicate conditions
#         -Wlogical-op                             # Warn about suspicious logical operations
#     )
# endif()

# # Additional strict flags for Clang
# if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
#     add_compile_options(
#         -Winconsistent-missing-override         # Warn about missing override
#     )
# endif()

# add_subdirectory(SFML)
add_subdirectory(Homework_0)
add_subdirectory(Homework_1)
add_subdirectory(Homework_2)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)
