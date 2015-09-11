cmake_minimum_required(VERSION 2.8)
project(Model)

add_executable(model model.cpp)
#add_library(winsys winsys.cpp)

target_link_libraries(model)
