cmake_minimum_required(VERSION 2.8)
project(Subdivision)

add_executable(subd-bin main.cpp)
add_library(subd subd.cpp)

target_link_libraries(subd model)
target_link_libraries(subd-bin winsys pipeline model subd glmath GL X11)
