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

# Only fetch if not using external libraries
if(NOT USE_EXTERNAL_LIBS)
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
    FetchContent_MakeAvailable(GLFW)

    # Fetch GLAD (OpenGL loader)
    FetchContent_Declare(GLAD
        GIT_REPOSITORY https://github.com/Dav1dde/glad
        GIT_TAG v0.1.34
        GIT_SHALLOW ON
    )
    FetchContent_MakeAvailable(GLAD)
else()
    message(STATUS "Homework_3: Expecting GLM, GLFW, GLAD to be available from external/")
endif()


# # Fetch STB image (single header image loading)
# FetchContent_Declare(STB
#     GIT_REPOSITORY https://github.com/nothings/stb
#     GIT_TAG master
#     GIT_SHALLOW ON
# )
# FetchContent_MakeAvailable(STB)

# # Fetch Assimp (model loading library)
# FetchContent_Declare(Assimp
#     GIT_REPOSITORY https://github.com/assimp/assimp
#     GIT_TAG v5.4.2
#     GIT_SHALLOW ON
# )
# set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
# set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
# set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
# set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(Assimp)

# Find OpenGL
find_package(OpenGL REQUIRED)
message(STATUS "OpenGL found: ${OpenGL_FOUND}")
message(STATUS "OpenGL version: ${OpenGL_VERSION}")
message(STATUS "OpenGL libraries: ${OPENGL_LIBRARIES}")
message(STATUS "OpenGL include directories: ${OPENGL_INCLUDE_DIR}")

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/test.cpp)

add_executable(Hw3 ${SOURCES})

# Add include directories
if(USE_EXTERNAL_LIBS)
    target_include_directories(Hw3 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/external/include
    )
else()
    target_include_directories(Hw3 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${glad_SOURCE_DIR}/include
        ${glfw_SOURCE_DIR}/include
    )
endif()

target_link_libraries(Hw3 PRIVATE
    OpenGL::GL
    glm
    glfw
    glad
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
# target_link_libraries(Hw3 PRIVATE OpenGL::GL glm assimp glfw glad)

# Copy shaders and assets to output directory
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/shaders" DESTINATION "${COMMON_OUTPUT_DIR}/bin/")
# file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${COMMON_OUTPUT_DIR}/bin/" OPTIONAL)
