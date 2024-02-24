#pragma once

#include <core/system.hpp>
#include <cstdint>
#include <rendering/shader.hpp>
#include <rendering/voxel_world.hpp>
#include <vector>
#include <voxlight_api.hpp>

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
  void onWindowResize(EngineEventType eventType, EngineEvent const &event);

  void createGBuffer();
  void initImgui();
  void drawImgui(float deltaTime);

  int renderResolutionX;
  int renderResolutionY;

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
