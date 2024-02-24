cmake_minimum_required(VERSION 3.26)

add_library(
    Voxlight
    # core
    api/camera_component_api.cpp
    api/engine_api.cpp
    api/entity_api.cpp
    api/voxel_component_api.cpp
    core/voxlight.cpp
    # rendering
    rendering/render_system.cpp
    rendering/render_utils.cpp
    rendering/shader.cpp
    # api
    rendering/voxel_world.cpp
)
target_include_directories(Voxlight PUBLIC ${PROJECT_SOURCE_DIR}/include)

set(CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)

# Perform dependency linkage
include(${CMAKE_DIR}/LinkGLFW.cmake)
linkglfw(Voxlight PUBLIC)

include(${CMAKE_DIR}/LinkGLAD.cmake)
linkglad(Voxlight PUBLIC)

include(${CMAKE_DIR}/LinkGLM.cmake)
linkglm(Voxlight PUBLIC)

include(${CMAKE_DIR}/LinkSTB.cmake)
linkstb(Voxlight PUBLIC)

include(${CMAKE_DIR}/LinkSPGLOG.cmake)
linkspglog(Voxlight PUBLIC)

include(${CMAKE_DIR}/LinkENTT.cmake)
linkentt(Voxlight PUBLIC)

# include(${CMAKE_DIR}/LinkIMGUI.cmake) LinkIMGUI(Voxlight PRIVATE)
find_package(OpenGL REQUIRED)

if (OpenGL_FOUND)
    target_include_directories(Voxlight PRIVATE ${OPENGL_INCLDUE_DIRS})
    target_link_libraries(Voxlight PRIVATE ${OPENGL_LIBRARIES})
else ()
    message(FATAL_ERROR "OpenGL was not found on the system")
endif ()

target_compile_features(Voxlight PRIVATE cxx_std_20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_compile_options(Voxlight PRIVATE -Wall -Wextra)
