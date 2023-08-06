#include "utils.hpp"

#include <iostream>
#include <fstream>
#include <array>
#include <assert.h>

namespace fs = std::filesystem;

std::tuple<GLuint, ReturnCode> createShader(GLenum shaderType, fs::path shaderPath) {
    if(fs::exists(shaderPath) && shaderPath.extension() == ".glsl") {
        std::ifstream file(shaderPath, std::ios::in);
        std::string shaderCode{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

        auto shader = glCreateShader(shaderType);
        const char* c_str = shaderCode.c_str();
        glShaderSource(shader, 1, &c_str, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success) {
            GLchar infoLog[512];
            glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
            std::cout << "ERROR::SHADER::" << shaderPath << "::COMPILATION_FAILED\n" << infoLog << std::endl;
            return {0, ReturnCode::FAILURE};
        }
        return {shader, ReturnCode::SUCCESS};
    }
    
    std::cout << "Inalid shader path\n";
    return {0, ReturnCode::FAILURE};
}

std::vector<std::uint8_t> loadVox(std::filesystem::path path) {
    auto readTag = [](std::istreambuf_iterator<char>& iterator) {
        std::string tag;
        for(std::size_t i = 0; i < 4; i++) {
            tag += *iterator;
            std::advance(iterator, 1);
        }
        return tag;
    };

    auto readInt = [](std::istreambuf_iterator<char>& iterator) {
        std::array<std::uint8_t, 4> data;
        for(std::size_t i = 0; i < 4; i++) {
            data[i] = *iterator;
            std::advance(iterator, 1);
        }
        return static_cast<std::int32_t>(data[0] | data[1] << 8 | data[2] << 16 | data[3] << 24);
    };

    std::vector<std::uint8_t> data;
    std::uint8_t xsize;
    std::uint8_t ysize;
    std::uint8_t zsize;
    if(fs::exists(path) && path.extension() == ".vox") {
        std::ifstream file(path, std::ios::in | std::ios::binary);
        auto iterator = std::istreambuf_iterator<char>(file);
        
        readTag(iterator); // VOX tag
        readInt(iterator); // version

        while(iterator != std::istreambuf_iterator<char>()) {
            auto tag = readTag(iterator);
            auto contentSize = readInt(iterator);
            readInt(iterator); // Child chunks

            if(tag == "SIZE") {
                xsize = readInt(iterator);
                zsize = readInt(iterator);
                ysize = readInt(iterator);
                data.resize(xsize * ysize * zsize);
            } else if(tag == "XYZI") {
                // assert(data.size() == contentSize / 4);
                auto voxelCount = readInt(iterator);
                for(auto i = 0; i < voxelCount; i++) {
                    auto x = static_cast<std::uint32_t>(*iterator);
                    std::advance(iterator, 1);
                    auto z = static_cast<std::uint32_t>(*iterator);
                    std::advance(iterator, 1);
                    auto y = static_cast<std::uint32_t>(*iterator);
                    std::advance(iterator, 1);
                    std::uint8_t colorIndex = *iterator;
                    std::advance(iterator, 1);
                    data[x + y * xsize + z * xsize * ysize] = colorIndex;
                }
            } else {
                std::advance(iterator, contentSize);
            }
        }
    }
    return data;
}

GLuint createVoxelTexture(std::vector<GLubyte> const& data, glm::vec3 size) {
    GLuint texname;
    glGenTextures(1, &texname);
    glBindTexture(GL_TEXTURE_3D, texname);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, size.x, size.y, size.z, 0, GL_RED, 
                GL_UNSIGNED_BYTE, data.data());
    return texname;
}

std::vector<std::uint8_t> createPlane(glm::vec3 corner, float width, float length, std::uint8_t type)
{
    std::vector<std::uint8_t> data;
    data.resize(width * length);
    for (std::uint32_t x = corner.x; x < corner.x + width; x++)
    {
        for (std::uint32_t z = corner.z; z < corner.z + length; z++)
        {
            data[x + z * width] = type;
        }
    }
    return data;
}