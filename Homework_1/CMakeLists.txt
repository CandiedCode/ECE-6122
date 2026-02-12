# Homework_1 CMakeLists.txt
# Inherits C++ standard and compiler options from parent

# Create library for Maze components
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Create Lab2 executable
add_executable(Lab2 ${SOURCES})
target_link_libraries(Lab2 sfml::sfml)

# Create a convenience target to build all Homework_1 components
add_custom_target(Homework_1 DEPENDS Lab2)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/fonts" DESTINATION "${CMAKE_BINARY_DIR}/Homework_1")