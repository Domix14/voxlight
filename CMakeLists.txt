cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(VoxelEngine C CXX)



add_subdirectory(src_v2)
add_subdirectory(examples/test)

add_custom_command(
     COMMAND "py" "${CMAKE_SOURCE_DIR}/tools/generate_shader.py" "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "${CMAKE_CURRENT_SOURCE_DIR}/src_v2/generated"
     OUTPUT  "{CMAKE_CURRENT_SOURCE_DIR}/src_v2/generated/shaders/shaders.hpp"
     COMMENT "Generating shader header files."
     DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders
)
add_library(generatedLib "{CMAKE_CURRENT_SOURCE_DIR}/src_v2/generated/shaders/shaders.hpp")
set_target_properties(generatedLib PROPERTIES LINKER_LANGUAGE CXX)
add_dependencies(VoxelEngine generatedLib)

# set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# # Set directory paths
# set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set(CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)


# # set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# # set(CMAKE_CXX_CLANG_TIDY clang-tidy --checks=-*,readability-* -p ${CMAKE_CURRENT_BINARY_DIR})

# set(sources
#  ${SOURCE_DIR}/utils.cpp
#  ${SOURCE_DIR}/VoxelMap.cpp
#  ${SOURCE_DIR}/Camera.cpp
#  ${SOURCE_DIR}/Chunk.cpp
#  ${SOURCE_DIR}/Engine.cpp
#  ${SOURCE_DIR}/VoxelWorld.cpp
#  ${SOURCE_DIR}/VoxelObject.cpp)

# add_library(VoxelEngine ${sources} ${includes})
# target_include_directories(VoxelEngine PUBLIC ${SOURCE_DIR}/include)


# # Perform dependency linkage
# include(${CMAKE_DIR}/LinkGLFW.cmake)
# LinkGLFW(VoxelEngine PUBLIC)

# include(${CMAKE_DIR}/LinkGLAD.cmake)
# LinkGLAD(VoxelEngine PUBLIC)

# include(${CMAKE_DIR}/LinkGLM.cmake)
# LinkGLM(VoxelEngine PUBLIC)


# include(${CMAKE_DIR}/LinkSTB.cmake)
# LinkSTB(VoxelEngine PUBLIC)

# # include(${CMAKE_DIR}/LinkIMGUI.cmake)
# # LinkIMGUI(VoxelEngine PRIVATE)

# find_package(OpenGL REQUIRED)
# if (OpenGL_FOUND)
#     target_include_directories(VoxelEngine PRIVATE ${OPENGL_INCLDUE_DIRS})
#     target_link_libraries(VoxelEngine PRIVATE ${OPENGL_LIBRARIES})
# else()
#     message(FATAL_ERROR "OpenGL was not found on the system")
# endif()

# # Enable C++20
# set_target_properties(VoxelEngine PROPERTIES
#     CXX_STANDARD 20
#     CXX_STANDARD_REQUIRED YES
#     CXX_EXTENSIONS NO)

# # Set project folders
# set_target_properties(VoxelEngine PROPERTIES FOLDER ${PROJECT_NAME})
# target_compile_options(VoxelEngine PRIVATE -Wall -Wextra)



# add_subdirectory(examples/test)

