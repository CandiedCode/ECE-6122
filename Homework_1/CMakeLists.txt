# Homework_1 CMakeLists.txt
# Inherits C++ standard and compiler options from parent

project(Homework_1 VERSION 1.0)

# Include FetchContent for downloading SFML
include(FetchContent)

# Declare SFML from GitHub
FetchContent_Declare(SFML-GIT
    GIT_REPOSITORY https://github.com/SFML/SFML
    GIT_TAG 2.6.2
    GIT_SHALLOW ON
)

# Download and make SFML available
FetchContent_MakeAvailable(SFML-GIT)

# Create library for Maze components
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Create Lab2 executable
add_executable(Hw1 ${SOURCES})
target_link_libraries(Hw1 PRIVATE sfml-graphics sfml-window sfml-system)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/fonts" DESTINATION "${COMMON_OUTPUT_DIR}/bin/")
