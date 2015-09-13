cmake_minimum_required(VERSION 2.8)
project(OpenGLSandbox)

include_directories(winsys GLMath)

add_subdirectory(winsys)
add_subdirectory(XFont)
add_subdirectory(XImage)
add_subdirectory(Tessellation)
add_subdirectory(Model)
add_subdirectory(GLMath)

add_dependencies(xfont winsys)
add_dependencies(ximage winsys)
add_dependencies(tessellation winsys)
add_dependencies(model-bin winsys glmath)
