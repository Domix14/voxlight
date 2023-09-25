#ifndef UTILS_HPP
#define UTILS_HPP

#include <glad/gl.h>

#include <cstdint>
#include <filesystem>
#include <glm/glm.hpp>
#include <tuple>
#include <vector>

enum ReturnCode : std::uint8_t { SUCCESS = 0, FAILURE = 1 };

struct VoxelContainer {
    std::vector<std::uint8_t> data;
    glm::vec3 size;
};

std::tuple<GLuint, ReturnCode> createShader(GLenum shaderType,
                                            std::string_view shaderCode);

VoxelContainer loadVox(std::filesystem::path path);

GLuint createVoxelTexture(std::vector<GLubyte> const& data, glm::vec3 size);
std::vector<std::uint8_t> createPlane(glm::vec3 corner, float width,
                                      float length, std::uint8_t type);

#endif  // UTILS_HPP
