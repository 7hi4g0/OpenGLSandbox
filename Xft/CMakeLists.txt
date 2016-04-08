cmake_minimum_required(VERSION 2.8)
project(Xft)

add_executable(xft-bin main.cpp)

target_link_libraries(xft-bin winsys pipeline glmath model X11 GL m)
