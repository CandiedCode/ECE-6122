# Homework_1 CMakeLists.txt
# Inherits C++ standard and compiler options from parent

project(Homework_1 VERSION 1.0)

# Include FetchContent for downloading SFML
include(FetchContent)

# Declare SFML from GitHub
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML
    GIT_TAG 2.6.2
    GIT_SHALLOW ON
)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "")  # We only need graphics, window, and system modules
# Download and make SFML available
FetchContent_MakeAvailable(SFML)

# # Uncomment this out for PACE Ice to include Local SFML directory (Make sure FetchContent block above is commented out
# include_directories(${PROJECT_SOURCE_DIR}/../SFML/include)
# link_directories(${PROJECT_SOURCE_DIR}/../SFML/lib)

# Create library for Maze components
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Create Homework_1 executable
add_executable(Hw1 ${SOURCES})
target_link_libraries(Hw1 PRIVATE sfml-graphics sfml-window sfml-system)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/fonts" DESTINATION "${COMMON_OUTPUT_DIR}/bin/")
