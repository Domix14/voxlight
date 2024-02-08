cmake_minimum_required(VERSION 3.24)
project(OpenGLTemplate C CXX)

add_executable(VoxlightTest main.cpp)
target_compile_features(VoxlightTest PRIVATE cxx_std_20)


target_link_libraries(VoxlightTest PRIVATE Voxlight)