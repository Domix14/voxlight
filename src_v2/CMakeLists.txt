cmake_minimum_required(VERSION 3.24)


# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_CLANG_TIDY clang-tidy --checks=-*,readability-* -p ${CMAKE_CURRENT_BINARY_DIR})

set(sources
    # core
    ${CMAKE_CURRENT_SOURCE_DIR}/voxel_engine.cpp
    # rendering
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/render_system.cpp
    # controller
    ${CMAKE_CURRENT_SOURCE_DIR}/controller/controller_system.cpp
    # world
    ${CMAKE_CURRENT_SOURCE_DIR}/world/world_system.cpp
    )

add_library(VoxelEngine ${sources})
target_include_directories(VoxelEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)

# Perform dependency linkage
include(${CMAKE_DIR}/LinkGLFW.cmake)
LinkGLFW(VoxelEngine PUBLIC)

include(${CMAKE_DIR}/LinkGLAD.cmake)
LinkGLAD(VoxelEngine PUBLIC)

include(${CMAKE_DIR}/LinkGLM.cmake)
LinkGLM(VoxelEngine PUBLIC)

include(${CMAKE_DIR}/LinkSTB.cmake)
LinkSTB(VoxelEngine PUBLIC)

include(${CMAKE_DIR}/LinkSPGLOG.cmake)
LinkSPGLOG(VoxelEngine PUBLIC)

include (${CMAKE_DIR}/LinkENTT.cmake)
LinkENTT(VoxelEngine PUBLIC)

# include(${CMAKE_DIR}/LinkIMGUI.cmake)
# LinkIMGUI(VoxelEngine PRIVATE)

find_package(OpenGL REQUIRED)
if (OpenGL_FOUND)
    target_include_directories(VoxelEngine PRIVATE ${OPENGL_INCLDUE_DIRS})
    target_link_libraries(VoxelEngine PRIVATE ${OPENGL_LIBRARIES})
else()
    message(FATAL_ERROR "OpenGL was not found on the system")
endif()


target_compile_features(VoxelEngine PRIVATE cxx_std_20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_compile_options(VoxelEngine PRIVATE -Wall -Wextra)

