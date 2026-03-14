project(Homework_3 VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")

# Fetch dependencies
include(FetchContent)

# Fetch GLM (header-only math library)
FetchContent_Declare(GLM
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG 1.0.1
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(GLM)

# Fetch GLAD (OpenGL loader)
FetchContent_Declare(GLAD
    GIT_REPOSITORY https://github.com/Dav1dde/glad
    GIT_TAG v0.1.34
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(GLAD)

# Fetch STB image (single header image loading)
FetchContent_Declare(STB
    GIT_REPOSITORY https://github.com/nothings/stb
    GIT_TAG master
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(STB)

# Find GLFW (window and context creation)
find_package(glfw3 REQUIRED)
message(STATUS "GLFW found: ${glfw3_FOUND}")

# Fetch Assimp (model loading library)
FetchContent_Declare(Assimp
    GIT_REPOSITORY https://github.com/assimp/assimp
    GIT_TAG v5.4.2
    GIT_SHALLOW ON
)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(Assimp)

# Find OpenGL
find_package(OpenGL REQUIRED)
message(STATUS "OpenGL found: ${OpenGL_FOUND}")
message(STATUS "OpenGL version: ${OpenGL_VERSION}")
message(STATUS "OpenGL libraries: ${OPENGL_LIBRARIES}")
message(STATUS "OpenGL include directories: ${OPENGL_INCLUDE_DIR}")

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(Hw3 ${SOURCES})

# Add include directories
target_include_directories(Hw3 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${glad_SOURCE_DIR}/include
    ${stb_SOURCE_DIR}
)

target_link_libraries(Hw3 PRIVATE OpenGL::GL glm assimp glfw glad)

# Copy shaders and assets to output directory
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/shaders" DESTINATION "${COMMON_OUTPUT_DIR}/bin/")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${COMMON_OUTPUT_DIR}/bin/" OPTIONAL)