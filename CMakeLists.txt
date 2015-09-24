cmake_minimum_required(VERSION 2.8)
project(OpenGLSandbox)

set(CMAKE_CXX_FLAGS "-std=gnu++11")

include_directories(winsys Model Pipeline Subdivision GLMath)

add_subdirectory(winsys)
add_subdirectory(XFont)
add_subdirectory(XImage)
add_subdirectory(Tessellation)
add_subdirectory(Model)
add_subdirectory(GLMath)
add_subdirectory(Pipeline)
add_subdirectory(Subdivision)

add_dependencies(xfont winsys)
add_dependencies(ximage winsys)
add_dependencies(tessellation pipeline winsys)
add_dependencies(model-bin winsys pipeline glmath)
