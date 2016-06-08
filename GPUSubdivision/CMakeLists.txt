cmake_minimum_required(VERSION 2.8)
project(GPUSubdivision)

add_executable(gpusubd-bin main.cpp)
add_library(gpusubd gpusubd.cpp adptlevel.cpp)

target_link_libraries(gpusubd model subd)
target_link_libraries(gpusubd-bin winsys pipeline model subd gpusubd scene glmath GL X11)
