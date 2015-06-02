cmake_minimum_required(VERSION 2.8)
project(XIamge)

add_executable(ximage main.cpp)

target_link_libraries(ximage winsys GL X11)
