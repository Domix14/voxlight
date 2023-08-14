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

VoxelContainer loadVox(std::filesystem::path path) {
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

    VoxelContainer container;
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
                container.size.x = readInt(iterator);
                container.size.z = readInt(iterator);
                container.size.y = readInt(iterator);
                
                assert(xsize % 4 == 0);
                assert(ysize % 4 == 0);
                assert(zsize % 4 == 0);

                container.data.resize(container.size.x * container.size.y * container.size.z);
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
                    container.data[x + y * container.size.x + z * container.size.x * container.size.y] = colorIndex;
                }
            } else {
                std::advance(iterator, contentSize);
            }
        }
    }
    return container;
}

GLuint createVoxelTexture(std::vector<GLubyte> const& data, glm::vec3 size) {
    GLuint texname;
    glGenTextures(1, &texname);
    glBindTexture(GL_TEXTURE_3D, texname);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    //glSamplerParameterf(texname,GL_TEXTURE_MAX_LOD,0);

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