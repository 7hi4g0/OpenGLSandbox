cmake_minimum_required(VERSION 2.8)
project(XFont)

add_executable(xfont main.cpp)

target_link_libraries(xfont winsys GL X11)
