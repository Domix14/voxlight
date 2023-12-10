cmake_minimum_required(VERSION 3.24)
project(OpenGLTemplate C CXX)

add_executable(VoxelEngineTest main.cpp)
target_compile_features(VoxelEngineTest PRIVATE cxx_std_20)


target_link_libraries(VoxelEngineTest PRIVATE VoxelEngine)