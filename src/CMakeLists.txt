cmake_minimum_required(VERSION 3.26)

add_library(
    voxlight
    # core
    api/camera_component_api.cpp
    api/engine_api.cpp
    api/entity_api.cpp
    api/voxel_component_api.cpp
    api/world_api.cpp
    core/voxel_data.cpp
    # rendering
    core/voxlight.cpp
    rendering/render_system.cpp
    rendering/render_utils.cpp
    rendering/shader.cpp
    # api
    rendering/voxel_world.cpp
)

target_include_directories(
    voxlight PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/voxlight>
)

target_compile_features(voxlight PRIVATE cxx_std_20)

set(CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)

# Perform dependency linkage
include(${CMAKE_DIR}/LinkGLFW.cmake)
linkglfw(voxlight PUBLIC)

include(${CMAKE_DIR}/LinkGLAD.cmake)
linkglad(voxlight PUBLIC)

include(${CMAKE_DIR}/LinkGLM.cmake)
linkglm(voxlight PUBLIC)

include(${CMAKE_DIR}/LinkSPGLOG.cmake)
linkspglog(voxlight PUBLIC)

include(${CMAKE_DIR}/LinkENTT.cmake)
linkentt(voxlight PUBLIC)

include(${CMAKE_DIR}/LinkPUGIXML.cmake)
linkpugixml(voxlight PRIVATE)

# include(${CMAKE_DIR}/LinkIMGUI.cmake) LinkIMGUI(Voxlight PRIVATE)
find_package(OpenGL REQUIRED)

if (OpenGL_FOUND)
    target_include_directories(voxlight PRIVATE ${OPENGL_INCLDUE_DIRS})
    target_link_libraries(voxlight PRIVATE ${OPENGL_LIBRARIES})
else ()
    message(FATAL_ERROR "OpenGL was not found on the system")
endif ()
