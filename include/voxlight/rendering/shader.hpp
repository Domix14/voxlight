#pragma once

#include <unordered_map>
#include <string_view>
#include <string>
#include <cstdint>


class Shader {
    public:
    Shader() = default;
    ~Shader() = default;

    void create(std::string_view vertexSource, std::string_view fragmentSource);

    void use() const;

    void setBool(std::string_view name, bool value) const;
    void setInt(std::string_view name, int value) const;
    void setFloat(std::string_view name, float value) const;
    void setVec2(std::string_view name, float x, float y) const;
    void setVec3(std::string_view name, float x, float y, float z) const;
    void setVec4(std::string_view name, float x, float y, float z, float w) const;
    void setMat2(std::string_view name, float const *value) const;
    void setMat3(std::string_view name, float const *value) const;
    void setMat4(std::string_view name, float const *value) const;

    // Hot reloading
    void loadAndCreate(std::string_view vertexPath, std::string_view fragmentPath);
    void refresh();
private:
    std::uint32_t compileShader(std::uint32_t shaderType, std::string_view source) const;
    std::uint32_t getUniformLocation(std::string_view name) const;

    std::uint32_t programId;
    std::unordered_map<std::string, std::uint32_t> uniformLocations;

    // Variables for hot reloading
    std::uint64_t lastCompileTime = 0.0f;
    std::string vertexShaderPath;
    std::string fragmentShaderPath;
};