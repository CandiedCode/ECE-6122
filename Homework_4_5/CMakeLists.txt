# =============================================================================
# CMakeLists.txt  --  Distributed CUDA Ray Tracer  (PROVIDED -- do not modify)
# ECE 4122/6122  Homework 4 & 5
#
# =============================================================================
cmake_minimum_required(VERSION 3.18)

project(CUDArayTracer LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD         17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_ARCHITECTURES 80)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-O2>)
add_compile_options($<$<COMPILE_LANGUAGE:CUDA>:-O2>)

include_directories(${PROJECT_SOURCE_DIR}/../SFML/include)

link_directories(${PROJECT_SOURCE_DIR}/../SFML/lib)

# coordinator uses all four SFML components (RenderWindow + UdpSocket)
add_executable(coordinator coordinator.cu)
target_include_directories(coordinator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(coordinator PRIVATE
  sfml-network sfml-graphics sfml-window sfml-system)
set_target_properties(coordinator PROPERTIES
  CUDA_SEPARABLE_COMPILATION OFF
  RUNTIME_OUTPUT_DIRECTORY   ${CMAKE_BINARY_DIR})

# worker uses only sfml-network (UdpSocket) -- no display
add_executable(worker worker.cu)
target_include_directories(worker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(worker PRIVATE
  sfml-network sfml-system)
set_target_properties(worker PROPERTIES
  CUDA_SEPARABLE_COMPILATION OFF
  RUNTIME_OUTPUT_DIRECTORY   ${CMAKE_BINARY_DIR})
