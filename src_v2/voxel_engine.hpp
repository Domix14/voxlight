#pragma once

#include <entt/entt.hpp>
#include <vector>

#include "controller/controller_system.hpp"
#include "rendering/render_system.hpp"
#include "system.hpp"
#include "world/world_system.hpp"

class GLFWwindow;

class VoxelEngine {
   public:
    VoxelEngine() : worldSystem(this), renderSystem(this), controllerSystem(this), initialized(false), running(true) {}
    ~VoxelEngine() = default;

    [[nodiscard]] entt::registry& getRegistry() { return registry; }
    [[nodiscard]] GLFWwindow* getWindow() const { return window; }

    void init();
    void run();
    void terminate() { running = false; };

    // Internal systems
    WorldSystem worldSystem;
    RenderSystem renderSystem;
    ControllerSystem controllerSystem;

   private:
    void deinit();

    entt::registry registry;
    bool initialized;
    bool running;

    GLFWwindow* window;
};