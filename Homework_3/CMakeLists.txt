project(Homework_3 VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")

# Determine whether to use external libraries or FetchContent
# Priority: command-line flag > environment variable > auto-detect
if(NOT DEFINED USE_EXTERNAL_LIBS)
    if(DEFINED ENV{USE_EXTERNAL_LIBS})
        set(USE_EXTERNAL_LIBS $ENV{USE_EXTERNAL_LIBS} CACHE BOOL "Use libraries from external/ folder instead of FetchContent")
        message(STATUS "Homework_3: USE_EXTERNAL_LIBS from environment variable = ${USE_EXTERNAL_LIBS}")
    elseif(EXISTS "${CMAKE_SOURCE_DIR}/external" AND NOT APPLE)
        set(USE_EXTERNAL_LIBS ON CACHE BOOL "Use libraries from external/ folder instead of FetchContent" FORCE)
        message(STATUS "Homework_3: Using libraries from external/ folder (auto-detected)")
    else()
        set(USE_EXTERNAL_LIBS OFF CACHE BOOL "Use libraries from external/ folder instead of FetchContent" FORCE)
        message(STATUS "Homework_3: Using FetchContent for libraries (auto-detected)")
    endif()
else()
    set(USE_EXTERNAL_LIBS "${USE_EXTERNAL_LIBS}" CACHE BOOL "Use libraries from external/ folder instead of FetchContent")
    message(STATUS "Homework_3: USE_EXTERNAL_LIBS from command-line = ${USE_EXTERNAL_LIBS}")
endif()

# Fetch dependencies
include(FetchContent)

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

if(NOT USE_EXTERNAL_LIBS)
    # Fetch GLM (header-only math library)
    FetchContent_Declare(GLM
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 0.9.7.1
        GIT_SHALLOW ON
    )
    FetchContent_MakeAvailable(GLM)

    # Fetch GLFW (window and context creation)
    # Note: GLFW is already fetched above for GLAD extraction, so we just need to build it
    FetchContent_Declare(GLFW
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 3.1.2
        GIT_SHALLOW ON
    )
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(GLFW)  # This will use the already-fetched GLFW_SOURCE

    if(NOT TARGET glad)
        add_library(glad ${GLFW_SOURCE_DIR}/deps/glad.c)
        target_include_directories(glad PUBLIC ${GLFW_SOURCE_DIR}/deps)
        message(STATUS "Using GLAD from GLFW FetchContent (${GLFW_SOURCE_DIR}/deps)")
    endif()
else()
    message(STATUS "Homework_3: Using GLM and GLFW from external/")
    # GLM is header-only; create an interface target pointing at external/glm-0.9.7.1
    if(NOT TARGET glm)
        add_library(glm INTERFACE)
        target_include_directories(glm INTERFACE ${CMAKE_SOURCE_DIR}/external/glm-0.9.7.1)
    endif()

    if(NOT TARGET glad)
        add_library(glad ${CMAKE_SOURCE_DIR}/external/glfw-3.1.2/deps/glad.c)
        target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/external/glfw-3.1.2/deps)
        message(STATUS "Using GLAD from external/glfw-3.1.2/deps (bundled with GLFW)")
    endif()

    # For assimp: if the target doesn't exist (external/CMakeLists.txt not included),
    # create an interface target. Otherwise, just add the include directories to Hw3.
    if(NOT TARGET assimp)
        add_library(assimp INTERFACE)
        target_include_directories(assimp INTERFACE
            ${CMAKE_SOURCE_DIR}/external/assimp-3.0.1270/include
            ${CMAKE_SOURCE_DIR}/external/stb
        )
        message(STATUS "Using Assimp from external/assimp-3.0.1270/include (interface library)")
    else()
        message(STATUS "Using Assimp from external/assimp-3.0.1270 (already built)")
    endif()
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
if(USE_EXTERNAL_LIBS)
    target_include_directories(Hw3 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/external/glfw-3.1.2/include
        ${CMAKE_SOURCE_DIR}/external/assimp-3.0.1270/include
        ${CMAKE_SOURCE_DIR}/external/stb
    )
else()
    target_include_directories(Hw3 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${GLFW_SOURCE_DIR}/include
        ${assimp_SOURCE_DIR}/include
        ${assimp_BINARY_DIR}/include
        ${CMAKE_SOURCE_DIR}/external/stb
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
# through CMake targets, so we link them explicitly.
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
