#pragma once
#include "camera.hpp"
#include "system.hpp"

class GLFWwindow;

class ControllerSystem : public ISystem {
public:
  ControllerSystem() = default;

  void init(VoxelEngine *voxelEngine);
  void update(float deltaTime);
  void deinit();

  glm::mat4 getViewProjectionMatrix() const;
  glm::vec3 getCameraPosition() const;

private:
  GLFWwindow *window;
  Camera camera;
  VoxelEngine *engine;
};