cmake_minimum_required(VERSION 2.8)
project(GLMath)

add_library(glmath matrix4.cpp matrix.cpp point.cpp)

target_link_libraries(glmath)
