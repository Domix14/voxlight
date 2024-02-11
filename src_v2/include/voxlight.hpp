#pragma once

#include <entt/entt.hpp>
#include <vector>

#include "api/voxlight_api.hpp"
#include "controller/controller_system.hpp"
#include "rendering/render_system.hpp"
#include "system.hpp"
#include "world/world_system.hpp"

class GLFWwindow;

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
  RenderSystem* renderSystem;
  // ControllerSystem controllerSystem;

  // Custom systems
  std::vector<System *> customSystems;

  // Friend class declarations
  friend class EngineApi;
  friend class EntityApi;
};