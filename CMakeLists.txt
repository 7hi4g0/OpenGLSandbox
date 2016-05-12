cmake_minimum_required(VERSION 2.8)
project(OpenGLSandbox)

set(CMAKE_CXX_FLAGS "-std=gnu++11")

include_directories(winsys GLCommon GLMath Model Pipeline Subdivision Scene)

add_subdirectory(winsys)
add_subdirectory(GLCommon)
add_subdirectory(GLMath)
add_subdirectory(Tessellation)
add_subdirectory(Model)
add_subdirectory(Pipeline)
add_subdirectory(Subdivision)
add_subdirectory(Scene)

add_dependencies(pipeline glcommon)
add_dependencies(tessellation pipeline winsys)
add_dependencies(model-bin winsys pipeline glmath model)
add_dependencies(subd model)
add_dependencies(subd-bin winsys pipeline glmath subd)
add_dependencies(scene model)
add_dependencies(scene-bin winsys pipeline model scene glmath)
