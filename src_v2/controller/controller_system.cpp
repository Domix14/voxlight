#include "controller_system.hpp"

#include "voxel_engine.hpp"

ControllerSystem::ControllerSystem(VoxelEngine* engine) : System(engine) {}

void ControllerSystem::init() { window = getEngine()->getWindow(); };

void ControllerSystem::update(float deltaTime) { camera.update(window, deltaTime); };

void ControllerSystem::deinit() {}

glm::mat4 ControllerSystem::getViewProjectionMatrix() const { return camera.getViewProjectionMatrix(); };

glm::vec3 ControllerSystem::getCameraPosition() const { return camera.position; };