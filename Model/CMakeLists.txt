cmake_minimum_required(VERSION 2.8)
project(Model)

add_executable(model-bin model.cpp main.cpp)
add_library(model model.cpp)

target_link_libraries(model-bin)
