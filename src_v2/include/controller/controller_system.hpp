#pragma once
#include "camera.hpp"
#include "system.hpp"

struct GLFWwindow;

class ControllerSystem : public System {
 public:
  ControllerSystem() = default;

  void init(Voxlight *voxlight);
  void update(float deltaTime);
  void deinit();

  glm::mat4 getViewProjectionMatrix() const;
  glm::vec3 getCameraPosition() const;

 private:
  GLFWwindow *window;
  Camera camera;
  Voxlight *engine;
};
