#pragma once

#include <entt/entity/registry.hpp>
#include <vector>

#include "api/voxlight_api.hpp"
#include "core/components.hpp"
#include "core/event_manager.hpp"
#include "rendering/render_system.hpp"
#include "system.hpp"

struct GLFWwindow;
class Voxlight final {
 public:
  Voxlight(int windowWidth, int windowHeight, std::string windowTitle);
  ~Voxlight() = default;

 private:
  void init();
  void run();
  void stop();

  entt::registry registry;
  void deinit();

  void initGLFW();

  // config
  bool isRunning = false;
  int windowWidth;
  int windowHeight;
  std::string windowTitle;

  GLFWwindow *glfwWindow = nullptr;

  // Internal systems
  RenderSystem renderSystem;

  // Custom systems
  std::vector<std::unique_ptr<System>> customSystems;

  // Camera
  entt::entity currentCamera = entt::null;

  // Event system
  EventManager<EngineEvent> engineEventManager;
  EventManager<VoxelComponentEvent> voxelComponentEventManager;
  EventManager<EntityEvent> entityEventManager;

  // Friend class declarations
  friend class EngineApi;
  friend class EntityApi;
  friend class VoxelComponentApi;
  friend class CameraComponentApi;
};
