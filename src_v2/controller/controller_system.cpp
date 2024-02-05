#include "controller_system.hpp"
#include "voxel_engine.hpp"

void ControllerSystem::init(VoxelEngine *voxelEngine) {
  engine = voxelEngine;
  window = engine->getWindow();
};

void ControllerSystem::update(float deltaTime) {
  camera.update(window, deltaTime);
};

void ControllerSystem::deinit() {}

glm::mat4 ControllerSystem::getViewProjectionMatrix() const {
  return camera.getViewProjectionMatrix();
};

glm::vec3 ControllerSystem::getCameraPosition() const {
  return camera.position;
};