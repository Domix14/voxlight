#include <glad/gl.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <rendering/shader.hpp>

std::uint32_t Shader::compileShader(std::uint32_t shaderType, std::string_view shaderCode) const {
  auto shader = glCreateShader(shaderType);
  char const *c_str = shaderCode.data();
  glShaderSource(shader, 1, &c_str, nullptr);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if(!success) {
    std::string infoLog(512, '\0');
    glGetShaderInfoLog(shader, infoLog.size(), nullptr, infoLog.data());
    spdlog::error("Failed to compile shader: {}", infoLog);
    return 0;
  }
  return shader;
}

void Shader::create(std::string_view vertexSource, std::string_view fragmentSource) {
  programId = glCreateProgram();
  auto vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
  auto fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
  glAttachShader(programId, vertexShader);
  glAttachShader(programId, fragmentShader);
  glLinkProgram(programId);

  // gather uniform locations
  GLint numUniforms;
  glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &numUniforms);
  for(GLint i = 0; i < numUniforms; i++) {
    GLsizei length;
    GLint size;
    GLenum type;
    char name[256];
    glGetActiveUniform(programId, i, sizeof(name), &length, &size, &type, name);
    uniformLocations[name] = glGetUniformLocation(programId, name);
  }
  lastCompileTime = std::chrono::system_clock::now().time_since_epoch().count();
}

void Shader::use() const { glUseProgram(programId); }

void Shader::setBool(std::string_view name, bool value) const {
  glUniform1i(getUniformLocation(name), static_cast<int>(value));
}

void Shader::setInt(std::string_view name, int value) const { glUniform1i(getUniformLocation(name), value); }

void Shader::setFloat(std::string_view name, float value) const { glUniform1f(getUniformLocation(name), value); }

void Shader::setVec2(std::string_view name, float x, float y) const { glUniform2f(getUniformLocation(name), x, y); }

void Shader::setVec3(std::string_view name, float x, float y, float z) const {
  glUniform3f(getUniformLocation(name), x, y, z);
}

void Shader::setVec4(std::string_view name, float x, float y, float z, float w) const {
  glUniform4f(getUniformLocation(name), x, y, z, w);
}

void Shader::setMat2(std::string_view name, float const *value) const {
  glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, value);
}

void Shader::setMat3(std::string_view name, float const *value) const {
  glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, value);
}

void Shader::setMat4(std::string_view name, float const *value) const {
  glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, value);
}

void Shader::loadAndCreate(std::string_view vertexPath, std::string_view fragmentPath) {
  std::ifstream vertexFile(vertexPath.data());
  std::ifstream fragmentFile(fragmentPath.data());
  std::string vertexSource((std::istreambuf_iterator<char>(vertexFile)), std::istreambuf_iterator<char>());
  std::string fragmentSource((std::istreambuf_iterator<char>(fragmentFile)), std::istreambuf_iterator<char>());
  create(vertexSource, fragmentSource);
  vertexShaderPath = vertexPath;
  fragmentShaderPath = fragmentPath;
  lastCompileTime = std::chrono::system_clock::now().time_since_epoch().count();
}

void Shader::refresh() {
  std::uint64_t lastVertexWrite =
      std::chrono::clock_cast<std::chrono::system_clock>(std::filesystem::last_write_time(vertexShaderPath))
          .time_since_epoch()
          .count();
  std::uint64_t lastFragmentWrite =
      std::chrono::clock_cast<std::chrono::system_clock>(std::filesystem::last_write_time(fragmentShaderPath))
          .time_since_epoch()
          .count();
  if(lastVertexWrite > lastCompileTime || lastFragmentWrite > lastCompileTime) {
    spdlog::info("Reloading shaders");
    loadAndCreate(vertexShaderPath, fragmentShaderPath);
  }
}

std::uint32_t Shader::getUniformLocation(std::string_view name) const {
  if(uniformLocations.find(name.data()) == uniformLocations.end()) {
    spdlog::error("Uniform {} not found", name);
    return 0;
  }

  return uniformLocations.at(name.data());
}
