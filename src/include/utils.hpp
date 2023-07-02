#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <tuple>
#include <filesystem>
#include <glad/gl.h>

enum ReturnCode : std::uint8_t {
    SUCCESS = 0,
    FAILURE = 1
};

std::tuple<GLuint, ReturnCode> createShader(GLenum shaderType, std::filesystem::path path);

#endif // UTILS_HPP
