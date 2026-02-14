# Homework_1 CMakeLists.txt
# Inherits C++ standard and compiler options from parent

# Include FetchContent for downloading SFML
include(FetchContent)

# Declare SFML from GitHub
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML
    GIT_TAG 2.6.2
    GIT_SHALLOW ON
)

# Download and make SFML available
FetchContent_MakeAvailable(SFML)

# Create library for Maze components
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Create Lab2 executable
add_executable(Lab2 ${SOURCES})
target_link_libraries(Lab2 PRIVATE sfml-graphics sfml-window sfml-system)

# Create a convenience target to build all Homework_1 components
add_custom_target(Homework_1 DEPENDS Lab2)

file(COPY "${PROJECT_SOURCE_DIR}/fonts" DESTINATION "${COMMON_OUTPUT_DIR}/bin/")