#pragma once

#include <cstdint>
#include <vector>

#include "../core/system.hpp"
#include "../voxlight_api.hpp"
#include "shader.hpp"
#include "voxel_world.hpp"

class RenderSystem : public System {
 public:
  RenderSystem(Voxlight &voxlight);
  void init();
  void update(float deltaTime);
  void deinit();

 private:
  void onVoxelDataCreation(VoxelComponentEventType eventType, VoxelComponentEvent event);
  void onVoxelDataDestruction(VoxelComponentEventType eventType, VoxelComponentEvent event);
  void onVoxelDataModification(VoxelComponentEventType eventType, VoxelComponentEvent event);
  void onEntityTransformChange(EntityEventType eventType, EntityEvent event);
  void onWindowResize(EngineEventType eventType, EngineEvent event);

  void createGBuffer();
  void initImgui();
  void drawImgui(float deltaTime);

  std::uint32_t renderResolutionX;
  std::uint32_t renderResolutionY;

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
