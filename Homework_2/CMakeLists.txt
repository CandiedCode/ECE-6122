
project(Homework_2 VERSION 1.0)

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
find_package(OpenMP)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Create Homework_2 executable
add_executable(Hw2 ${SOURCES})
target_link_libraries(Hw2 PRIVATE sfml-graphics sfml-window sfml-system)
