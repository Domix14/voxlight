#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

#include "system.hpp"

struct VoxelData;
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

struct SunlightUniform {
  unsigned int invResolution;
  unsigned int magicMatrix;
  unsigned int sunPos;
  unsigned int worldTexture;
  unsigned int albedoTexture;
  unsigned int depthTexture;
  unsigned int normalTexture;
};

class RenderSystem : public System {
public:
  RenderSystem(Voxlight &voxlight);
  void init();
  void update(float deltaTime);
  void deinit();

  static unsigned int createVoxelTexture(std::uint8_t const *data, glm::ivec3 size);
  static void deleteVoxelTexture(unsigned int textueId);

private:
  // opengl programs
  unsigned int voxelProgram;
  unsigned int sunlightProgram;

  // uniforms
  VoxelUniform voxelUniform;
  SunlightUniform sunlightUniform;

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
