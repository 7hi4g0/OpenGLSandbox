cmake_minimum_required(VERSION 2.8)
project(Tessellation)

add_executable(tessellation main.cpp)

target_link_libraries(tessellation winsys pipeline GL X11)
