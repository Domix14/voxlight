#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

#include "system.hpp"
#include "voxel_world.hpp"
#include "shader.hpp"
#include <api/voxlight_api.hpp>

class RenderSystem : public System {
public:
  RenderSystem(Voxlight &voxlight);
  void init();
  void update(float deltaTime);
  void deinit();

private:
  void onVoxelDataCreation(VoxelComponentEventType eventType, VoxelComponentEvent const &event);
  void onVoxelDataDestruction(VoxelComponentEventType eventType, VoxelComponentEvent const &event);
  void onVoxelDataModification(VoxelComponentEventType eventType, VoxelComponentEvent const &event);
  void onEntityTransformChange(EntityEventType eventType, EntityEvent const &event);

  // shaders
  Shader voxelShader;
  Shader sunlightShader;

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

  // Voxel world
  VoxelWorld voxelWorld;
};
