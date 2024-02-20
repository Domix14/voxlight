#pragma once

#include <entt/entt.hpp>
#include <vector>

#include "api/voxlight_api.hpp"
#include "controller/controller_system.hpp"
#include "rendering/render_system.hpp"
#include "system.hpp"
#include "world/world_system.hpp"
#include "core/event_manager.hpp"
#include "core/event_manager.hpp"
#include "core/components.hpp"

struct GLFWwindow;

typedef entt::entity Entity;

class Voxlight final {
public:
  Voxlight();
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
  RenderSystem renderSystem;

  // Custom systems
  std::vector<std::unique_ptr<System>> customSystems;

  // Camera
  entt::entity currentCamera = entt::null;

  // Event system
  EventManager<VoxelComponentEvent> voxelComponentEventManager;
  

  // Friend class declarations
  friend class EngineApi;
  friend class EntityApi;
  friend class VoxelComponentApi;
  friend class CameraComponentApi;
};
