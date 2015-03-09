cmake_minimum_required(VERSION 2.8)
project(OpenGLSandbox)

include_directories(winsys)

add_subdirectory(winsys)
add_subdirectory(XFont)

add_dependencies(xfont winsys)
