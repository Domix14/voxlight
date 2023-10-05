#ifndef UTILS_HPP
#define UTILS_HPP

#include <glad/gl.h>

#include <cstdint>
#include <filesystem>
#include <glm/glm.hpp>
#include <tuple>
#include <vector>

constexpr float VOXEL_SIZE_10CM = 0.1f;
constexpr float VOXEL_SIZE_12CM = 0.125f;
constexpr float VOXEL_SIZE_25CM = 0.25f;
constexpr float VOXEL_SIZE_50CM = 0.5f;
constexpr float VOXEL_SIZE_100CM = 1.f;

typedef GLuint Program;
typedef GLuint Texture;
typedef float VoxelSize;
typedef std::uint8_t Voxel;

constexpr std::uint32_t NO_TEXTURE = std::numeric_limits<GLuint>::max();

enum ReturnCode : std::uint8_t { SUCCESS = 0, FAILURE = 1 };

struct VoxelContainer {
    std::vector<std::uint8_t> data;
    glm::vec3 size;
};

std::tuple<GLuint, ReturnCode> createShader(GLenum shaderType, std::string_view shaderCode);

VoxelContainer loadVox(std::filesystem::path path);

GLuint createVoxelTexture(std::vector<GLubyte> const& data, glm::vec3 size);
std::vector<std::uint8_t> createPlane(glm::vec3 corner, float width, float length, std::uint8_t type);

#endif  // UTILS_HPP
