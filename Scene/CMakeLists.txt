cmake_minimum_required(VERSION 2.8)
project(Scene)

add_executable(scene-bin main.cpp)
add_library(scene scene.cpp keyframe.cpp)

target_link_libraries(scene model glcommon)
target_link_libraries(scene-bin winsys pipeline model scene glmath GL X11)
