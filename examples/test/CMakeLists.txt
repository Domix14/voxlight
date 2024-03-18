cmake_minimum_required(VERSION 3.24)
project(OpenGLTemplate C CXX)

add_executable(VoxlightExample main.cpp)

target_include_directories(VoxlightExample PRIVATE .)

target_compile_features(VoxlightExample PRIVATE cxx_std_20)

target_link_libraries(VoxlightExample PRIVATE voxlight)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/assets/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)
