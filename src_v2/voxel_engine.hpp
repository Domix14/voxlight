#pragma once

#include <entt/entt.hpp>
#include <vector>

#include "controller/controller_system.hpp"
#include "rendering/render_system.hpp"
#include "system.hpp"
#include "world/world_system.hpp"

class GLFWwindow;

class VoxelEngine final {
public:
  VoxelEngine() = default;
  ~VoxelEngine() = default;

  [[nodiscard]] entt::registry &getRegistry() { return registry; }
  [[nodiscard]] GLFWwindow *getWindow() const { return window; }

  void init();
  void run();
  void terminate() { running = false; };

  template <std::derived_from<ISystem> T> void createSystem() {
    T *system = new T();
    system->init(this);
    customSystems.push_back(system);
  }

  WorldSystem &getWorldSystem() { return worldSystem; }
  RenderSystem &getRenderSystem() { return renderSystem; }
  ControllerSystem &getControllerSystem() { return controllerSystem; }

private:
  void deinit();

  entt::registry registry;
  bool initialized;
  bool running;

  GLFWwindow *window;

  // Internal systems
  WorldSystem worldSystem;
  RenderSystem renderSystem;
  ControllerSystem controllerSystem;

  // Custom systems
  std::vector<ISystem *> customSystems;
};