
project(Homework_2 VERSION 1.0)

# Set C++ standard and compile flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

# Include FetchContent for downloading dependencies
include(FetchContent)

# Declare SFML from GitHub
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML
    GIT_TAG 2.6.2
    GIT_SHALLOW ON
)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "")  # We only need graphics, window, and system modules

# Download and make dependencies available
FetchContent_MakeAvailable(SFML)

# Find threading library
find_package(Threads REQUIRED)

# Find OpenMP with proper error handling
find_package(OpenMP REQUIRED COMPONENTS CXX)
message(STATUS "OpenMP found: ${OPENMP_CXX_FOUND}")
message(STATUS "OpenMP libraries: ${OpenMP_CXX_LIBRARIES}")
message(STATUS "OpenMP include dirs: ${OpenMP_CXX_INCLUDE_DIRS}")

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Create Homework_2 executable
add_executable(Hw2 ${SOURCES})
target_link_libraries(Hw2 PRIVATE sfml-graphics sfml-window sfml-system Threads::Threads OpenMP::OpenMP_CXX)
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/fonts" DESTINATION "${COMMON_OUTPUT_DIR}/bin/")
