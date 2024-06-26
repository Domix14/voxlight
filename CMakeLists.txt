cmake_minimum_required(VERSION 3.26)

project(Voxlight VERSION 0.1.0)

set(VOXLIGHT_BUILD_EXAMPLES ON CACHE BOOL "Build examples for Voxlight.")
set(VOXLIGHT_BUILD_TESTS ON CACHE BOOL "Build tests for Voxlight.")

add_compile_definitions(
    VOXEL_FRAGMENT_SHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/shaders/voxel/fragment_shader.glsl"
)
add_compile_definitions(
    VOXEL_VERTEX_SHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/shaders/voxel/vertex_shader.glsl"
)
add_compile_definitions(
    SUNLIGHT_FRAGMENT_SHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/shaders/sunlight/fragment_shader.glsl"
)
add_compile_definitions(
    SUNLIGHT_VERTEX_SHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/shaders/sunlight/vertex_shader.glsl"
)

add_subdirectory(src)

if (VOXLIGHT_BUILD_EXAMPLES)
    add_subdirectory(examples/test)
endif ()

if (VOXLIGHT_BUILD_TESTS)
    add_subdirectory(tests)
endif ()

# Generate shader header files
set(OUT_SHADER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/voxlight/rendering/generated")
add_custom_command(
    COMMAND "py" "${CMAKE_SOURCE_DIR}/tools/generate_shader.py"
            "${CMAKE_CURRENT_SOURCE_DIR}/shaders" ${OUT_SHADER_PATH}
    OUTPUT "${OUT_SHADER_PATH}/shaders.hpp" COMMENT "Generating shader header files."
)
add_library(generatedLib "${OUT_SHADER_PATH}/shaders.hpp")
set_target_properties(generatedLib PROPERTIES LINKER_LANGUAGE CXX)
add_dependencies(voxlight generatedLib)
