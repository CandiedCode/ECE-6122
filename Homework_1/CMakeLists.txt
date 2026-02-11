# Homework_1 CMakeLists.txt
# Inherits C++ standard and compiler options from parent

# Create library for Maze components
# add_library(MazeLib STATIC MazeGenerator.cpp)
# target_include_directories(MazeLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Create Lab2 executable
add_executable(Lab2 ${SOURCES})
target_link_libraries(Lab2 sfml::sfml)

# Create a convenience target to build all Homework_1 components
add_custom_target(Homework_1 DEPENDS Lab2)
