cmake_minimum_required(VERSION 3.24)
project(OpenGLTemplate C CXX)

add_executable(VoxelEngineTest main.cpp)

target_link_libraries(VoxelEngineTest PRIVATE VoxelEngine)