cmake_minimum_required(VERSION 2.8)
project(OpenGLSandbox)

include_directories(winsys)

add_subdirectory(winsys)
add_subdirectory(XFont)
add_subdirectory(XImage)
add_subdirectory(Tessellation)

add_dependencies(xfont winsys)
add_dependencies(ximage winsys)
add_dependencies(tessellation winsys)
