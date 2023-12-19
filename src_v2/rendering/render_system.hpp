#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

#include "system.hpp"

class GLFWwindow;

class VoxelEngine;

struct VoxelUniform {
    unsigned int modelMatrix;
    unsigned int viewProjectionMatrix;
    unsigned int invResolution;
    unsigned int minBox;
    unsigned int maxBox;
    unsigned int chunkSize;
    unsigned int magicMatrix;
    unsigned int chunkTexture;
    unsigned int paletteTexture;
};

class RenderSystem : public System {
   public:
    RenderSystem(VoxelEngine* engine);

    void init();
    void update(float deltaTime);
    void deinit();

    unsigned int createVoxelTexture(std::vector<std::uint8_t> const& data, glm::ivec3 size);

    void createWorldTexture(std::vector<std::uint8_t> const& data, glm::ivec3 size);
    void updateWorldTexture(std::vector<std::uint8_t> const& data, glm::ivec3 size, glm::ivec3 offset);

   private:
    GLFWwindow* window;

    // opengl programs
    unsigned int voxelProgram;
    unsigned int sunglightProgram;

    // uniforms
    VoxelUniform voxelUniform;

    // opengl buffers
    unsigned int cubeVertexBuffer;
    unsigned int quadVertexBuffer;

    // framebuffer
    unsigned int mainFramebuffer;

    // voxel world
    unsigned int worldVoxelTexture;
    glm::ivec3 worldVoxelTextureSize;

    // opengl textures
    unsigned int colorTexture;
    unsigned int depthTexture;
    unsigned int normalTexture;
    unsigned int paletteTexture;
};