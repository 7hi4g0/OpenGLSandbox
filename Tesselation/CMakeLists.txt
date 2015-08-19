cmake_minimum_required(VERSION 2.8)
project(Tesselation)

add_executable(tesselation main.cpp)

target_link_libraries(tesselation winsys GL X11)
