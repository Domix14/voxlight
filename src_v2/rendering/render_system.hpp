#pragma once

#include "core/voxel_data.hpp"
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

    unsigned int createVoxelTexture(VoxelData<std::uint8_t> const& data);

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

    // opengl textures
    unsigned int colorTexture;
    unsigned int depthTexture;
    unsigned int normalTexture;
    unsigned int paletteTexture;
};