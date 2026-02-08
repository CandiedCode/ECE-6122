# Homework_1 CMakeLists.txt
# Inherits C++ standard and compiler options from parent

# Create library for Maze components
add_library(MazeLib STATIC MazeGenerator.cpp)
target_include_directories(MazeLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# SFML will be linked when creating executables that need visualization
# Uncomment below if MazeLib needs direct SFML dependencies
# target_link_libraries(MazeLib PUBLIC SFML::graphics SFML::window SFML::system)

# Uncomment below when main executable is created for Homework_1
# add_executable(MazeProgram main.cpp)
# target_link_libraries(MazeProgram MazeLib)

# Create a convenience target to build all Homework_1 components
add_custom_target(Homework_1 DEPENDS MazeLib)
