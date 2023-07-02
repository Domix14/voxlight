#include "utils.hpp"

#include <iostream>
#include <fstream>

namespace fs = std::filesystem;

std::tuple<GLuint, ReturnCode> createShader(GLenum shaderType, fs::path shaderPath) {
    if(fs::exists(shaderPath) && shaderPath.extension() == ".txt") {
        std::ifstream file(shaderPath, std::ios::in);
        std::string shaderCode{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

        auto shader = glCreateShader(shaderType);
        const char* c_str = shaderCode.c_str();
        glShaderSource(shader, 1, &c_str, nullptr);
        glCompileShader(shader);\

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success) {
            GLchar infoLog[512];
            glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
            std::cout << "ERROR::SHADER::" << shaderPath << "::COMPILATION_FAILED\n" << infoLog << std::endl;
            return std::make_tuple(0, ReturnCode::FAILURE);
        }
        return std::make_tuple(shader, ReturnCode::SUCCESS);
    }
    
    std::cout << "Inalid shader path\n";
    return std::make_tuple(0, ReturnCode::FAILURE);
}