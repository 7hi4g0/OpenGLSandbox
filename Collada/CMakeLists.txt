cmake_minimum_required(VERSION 2.8)
project(Collada)

add_executable(collada collada.cpp)

target_link_libraries(collada tinyxml2)
