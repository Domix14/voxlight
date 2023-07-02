cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(OpenGLTemplate C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set directory paths
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(sources
 ${SOURCE_DIR}/main.cpp
 ${SOURCE_DIR}/utils.cpp
 ${SOURCE_DIR}/VoxelMap.cpp
 ${SOURCE_DIR}/CPURenderer.cpp
 ${SOURCE_DIR}/Camera.cpp)

add_executable(opengl-template ${sources} ${includes})
target_include_directories(opengl-template PRIVATE ${SOURCE_DIR}/include)

# Perform dependency linkage
include(${CMAKE_DIR}/LinkGLFW.cmake)
LinkGLFW(opengl-template PRIVATE)

include(${CMAKE_DIR}/LinkGLAD.cmake)
LinkGLAD(opengl-template PRIVATE)

include(${CMAKE_DIR}/LinkGLM.cmake)
LinkGLM(opengl-template PRIVATE)

find_package(OpenGL REQUIRED)
if (OpenGL_FOUND)
    target_include_directories(opengl-template PRIVATE ${OPENGL_INCLDUE_DIRS})
    target_link_libraries(opengl-template PRIVATE ${OPENGL_LIBRARIES})
else()
    message(FATAL_ERROR "OpenGL was not found on the system")
endif()

# Enable C++17
set_target_properties(opengl-template PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

# Set project folders
set_target_properties(opengl-template PROPERTIES FOLDER ${PROJECT_NAME})