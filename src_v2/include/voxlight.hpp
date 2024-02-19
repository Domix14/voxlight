#pragma once

#include <entt/entt.hpp>
#include <vector>

#include "api/voxlight_api.hpp"
#include "controller/controller_system.hpp"
#include "rendering/render_system.hpp"
#include "system.hpp"
#include "world/world_system.hpp"

struct GLFWwindow;

typedef entt::entity Entity;

class Voxlight final {
public:
  Voxlight() = default;
  ~Voxlight() = default;

private:
  void init();
  void run();
  void stop();

  entt::registry registry;
  void deinit();

  bool isRunning = false;

  GLFWwindow *glfwWindow;

  // Internal systems
  // WorldSystem worldSystem;
  RenderSystem *renderSystem;
  // ControllerSystem controllerSystem;

  // Custom systems
  std::vector<System *> customSystems;

  // Camera
  entt::entity currentCamera = entt::null;

  // Friend class declarations
  friend class EngineApi;
  friend class EntityApi;
  friend class VoxelComponentApi;
  friend class CameraComponentApi;
};
