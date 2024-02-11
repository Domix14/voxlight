cmake_minimum_required(VERSION 3.24)


# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_CLANG_TIDY clang-tidy --checks=-*,readability-* -p ${CMAKE_CURRENT_BINARY_DIR})

set(sources
    # core
    voxlight.cpp
    # rendering
    render_system.cpp
    # controller
    # ${CMAKE_CURRENT_SOURCE_DIR}/controller/controller_system.cpp
    # world
    # ${CMAKE_CURRENT_SOURCE_DIR}/world/world_system.cpp
    # api
    engine_api.cpp
    entity_api.cpp
    voxel_component_api.cpp
    )

add_library(Voxlight ${sources})
target_include_directories(Voxlight PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)

# Perform dependency linkage
include(${CMAKE_DIR}/LinkGLFW.cmake)
LinkGLFW(Voxlight PUBLIC)

include(${CMAKE_DIR}/LinkGLAD.cmake)
LinkGLAD(Voxlight PUBLIC)

include(${CMAKE_DIR}/LinkGLM.cmake)
LinkGLM(Voxlight PUBLIC)

include(${CMAKE_DIR}/LinkSTB.cmake)
LinkSTB(Voxlight PUBLIC)

include(${CMAKE_DIR}/LinkSPGLOG.cmake)
LinkSPGLOG(Voxlight PUBLIC)

include (${CMAKE_DIR}/LinkENTT.cmake)
LinkENTT(Voxlight PUBLIC)

# include(${CMAKE_DIR}/LinkIMGUI.cmake)
# LinkIMGUI(Voxlight PRIVATE)

find_package(OpenGL REQUIRED)
if (OpenGL_FOUND)
    target_include_directories(Voxlight PRIVATE ${OPENGL_INCLDUE_DIRS})
    target_link_libraries(Voxlight PRIVATE ${OPENGL_LIBRARIES})
else()
    message(FATAL_ERROR "OpenGL was not found on the system")
endif()


target_compile_features(Voxlight PRIVATE cxx_std_20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_compile_options(Voxlight PRIVATE -Wall -Wextra)

