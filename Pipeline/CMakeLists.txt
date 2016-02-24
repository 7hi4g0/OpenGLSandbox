cmake_minimum_required(VERSION 2.8)
project(Pipeline)

add_library(pipeline pipeline.cpp)

target_link_libraries(pipeline glcommon)
