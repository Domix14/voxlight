#pragma once

#include <entt/entt.hpp>
#include <vector>

#include "controller/controller_system.hpp"
#include "rendering/render_system.hpp"
#include "system.hpp"
#include "world/world_system.hpp"

class GLFWwindow;

typedef entt::entity Entity;

class VoxelEngine final {
public:
  VoxelEngine() = default;
  ~VoxelEngine() = default;

  void init();
  void run();
  void terminate() { running = false; };

  template <std::derived_from<ISystem> T> void createSystem() {
    T *system = new T();
    customSystems.push_back(system);
  }

  Entity createEntity() { return registry.create(); }

  template <typename... Components>
  void addComponent(Entity entity, Components &&...components) {
    registry.emplace<Components...>(entity,
                                    std::forward<Components>(components)...);
  }

  entt::registry &getRegistry() { return registry; }
  GLFWwindow *getWindow() const { return window; }
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