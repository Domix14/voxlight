#pragma once

#include <entt/entity/registry.hpp>
#include <vector>

#include "../rendering/render_system.hpp"
#include "../voxlight_api.hpp"
#include "components.hpp"
#include "event_manager.hpp"
#include "system.hpp"

struct GLFWwindow;
class Voxlight final {
 public:
  Voxlight(std::uint32_t windowWidth, std::uint32_t windowHeight, std::string windowTitle);

 private:
  void init();
  void run();
  void stop();

  entt::registry registry;
  void deinit();

  void initGLFW();

  // config
  bool isRunning = false;
  std::uint32_t windowWidth;
  std::uint32_t windowHeight;
  std::string windowTitle;

  GLFWwindow *glfwWindow = nullptr;

  // Internal systems
  RenderSystem renderSystem;

  // Custom systems
  std::vector<std::unique_ptr<System>> customSystems;

  // Camera
  entt::entity currentCamera = entt::null;

  // World
  glm::ivec3 worldSize = {512, 256, 512};

  // Event system
  EventManager<EngineEvent> engineEventManager;
  EventManager<VoxelComponentEvent> voxelComponentEventManager;
  EventManager<EntityEvent> entityEventManager;

  // Friend class declarations
  friend class EngineApi;
  friend class EntityApi;
  friend class VoxelComponentApi;
  friend class CameraComponentApi;
  friend class WorldApi;
};
