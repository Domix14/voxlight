cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(OpenGLTemplate C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set directory paths
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)


# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_CLANG_TIDY clang-tidy --checks=-*,readability-* -p ${CMAKE_CURRENT_BINARY_DIR})

set(sources
 ${SOURCE_DIR}/main.cpp
 ${SOURCE_DIR}/utils.cpp
 ${SOURCE_DIR}/VoxelMap.cpp
 ${SOURCE_DIR}/Camera.cpp
 ${SOURCE_DIR}/Chunk.cpp
 ${SOURCE_DIR}/Engine.cpp
 ${SOURCE_DIR}/VoxelSystem.cpp)

add_executable(doom3d ${sources} ${includes})
target_include_directories(doom3d PRIVATE ${SOURCE_DIR}/include)


# Perform dependency linkage
include(${CMAKE_DIR}/LinkGLFW.cmake)
LinkGLFW(doom3d PRIVATE)

include(${CMAKE_DIR}/LinkGLAD.cmake)
LinkGLAD(doom3d PRIVATE)

include(${CMAKE_DIR}/LinkGLM.cmake)
LinkGLM(doom3d PRIVATE)


include(${CMAKE_DIR}/LinkSTB.cmake)
LinkSTB(doom3d PRIVATE)

# include(${CMAKE_DIR}/LinkIMGUI.cmake)
# LinkIMGUI(doom3d PRIVATE)

find_package(OpenGL REQUIRED)
if (OpenGL_FOUND)
    target_include_directories(doom3d PRIVATE ${OPENGL_INCLDUE_DIRS})
    target_link_libraries(doom3d PRIVATE ${OPENGL_LIBRARIES})
else()
    message(FATAL_ERROR "OpenGL was not found on the system")
endif()

# Enable C++17
set_target_properties(doom3d PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

# Set project folders
set_target_properties(doom3d PROPERTIES FOLDER ${PROJECT_NAME})
target_compile_options(doom3d PUBLIC -Wall)


