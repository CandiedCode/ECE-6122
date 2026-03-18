project(Homework_3 VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")

# Check if we're using external/ (built by root CMakeLists.txt)
if(TARGET glfw)
    message(STATUS "Homework_3: Using pre-built targets from external/ (root CMakeLists.txt enabled add_subdirectory(external))")
    # External targets (glfw, glad, assimp) are already built
    # GLM is header-only; create an interface target pointing at external/glm-0.9.7.1
    add_library(glm INTERFACE)
    target_include_directories(glm INTERFACE ${CMAKE_SOURCE_DIR}/external/glm-0.9.7.1)
else()
    # Fetch dependencies via FetchContent (for macOS or when external/ not built)
    message(STATUS "Homework_3: Using FetchContent for libraries")
    include(FetchContent)

    # Fetch GLM (header-only math library)
    FetchContent_Declare(GLM
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 0.9.7.1
        GIT_SHALLOW ON
    )
    FetchContent_MakeAvailable(GLM)

    # Fetch GLFW (window and context creation)
    FetchContent_Declare(GLFW
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 3.1.2
        GIT_SHALLOW ON
    )
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(GLFW)

    add_library(glad ${GLFW_SOURCE_DIR}/deps/glad.c)
    target_include_directories(glad PUBLIC ${GLFW_SOURCE_DIR}/deps)
    message(STATUS "Using GLAD from GLFW FetchContent (${GLFW_SOURCE_DIR}/deps)")

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
endif()

# Find OpenGL
find_package(OpenGL REQUIRED)
message(STATUS "OpenGL found: ${OpenGL_FOUND}")
message(STATUS "OpenGL version: ${OpenGL_VERSION}")
message(STATUS "OpenGL libraries: ${OPENGL_LIBRARIES}")
message(STATUS "OpenGL include directories: ${OPENGL_INCLUDE_DIR}")

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Hw3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Texture.cpp
)

add_executable(Hw3 ${SOURCES})

# Add include directories
if(TARGET glfw)
    # Using external/ libraries
    target_include_directories(Hw3 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/external/glfw-3.1.2/include
        ${CMAKE_SOURCE_DIR}/external/glfw-3.1.2/deps
        ${CMAKE_SOURCE_DIR}/external/assimp-3.0.1270/include
        ${CMAKE_SOURCE_DIR}/external/stb
    )
else()
    # Using FetchContent libraries
    target_include_directories(Hw3 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${GLFW_SOURCE_DIR}/include
        ${assimp_SOURCE_DIR}/contrib/stb
    )
endif()

target_link_libraries(Hw3 PRIVATE
    OpenGL::GL
    glm
    glfw
    glad
    assimp
)

# Add macOS frameworks only when using FetchContent GLFW
if(NOT USE_EXTERNAL_LIBS AND APPLE)
    target_link_libraries(Hw3 PRIVATE
        "-framework CoreFoundation"
        "-framework CoreGraphics"
        "-framework CoreVideo"
        "-framework IOKit"
        "-framework AppKit"
    )
endif()

# On Linux, GLFW 3.1.2 does not propagate its X11 transitive dependencies
# through CMake targets, so we link them explicitly for both FetchContent and external.
if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    target_link_libraries(Hw3 PRIVATE
        ${X11_LIBRARIES}
        ${X11_Xrandr_LIB}
        ${X11_Xinerama_LIB}
        ${X11_Xi_LIB}
        ${X11_Xcursor_LIB}
        dl
        pthread
    )
endif()
# target_link_libraries(Hw3 PRIVATE OpenGL::GL glm assimp glfw glad)

# Copy shaders and assets to output directory
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${COMMON_OUTPUT_DIR}/bin/")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/shaders" DESTINATION "${COMMON_OUTPUT_DIR}/bin/")
