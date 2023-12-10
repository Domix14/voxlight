#pragma once

#include <entt/entt.hpp>
#include <vector>

#include "system.hpp"

class GLFWwindow;

class VoxelEngine {
   public:
    VoxelEngine() : initialized(false), running(true) {}
    ~VoxelEngine() {
        for (auto& system : systems) {
            delete system;
        }
    }

    template <typename T>
    void registerSystem() {
        auto system = registerInternalSystem<T>();
        applicationSystems.push_back(system);
    }

    template <typename T>
    [[nodiscard]] T* getSystem() const {
        return dynamic_cast<T*>(systems.at(SystemType<T>));
    }

    [[nodiscard]] entt::registry& getRegistry() { return registry; }
    [[nodiscard]] GLFWwindow* getWindow() const { return window; }

    void init();
    void run();
    void terminate() { running = false; };

   private:
    void deinit();

    template <typename T>
    System* registerInternalSystem() {
        auto systemId = SystemType<T>;
        if (systems.size() != systemId) {
            throw std::runtime_error("System is already registered");
        }
        System* newSystem = new T(this);
        systems.push_back(newSystem);
        return systems.back();
    }

    inline static std::size_t systemCounter{0};
    template <typename Type>
    inline static const std::size_t SystemType = systemCounter++;

    std::vector<System*> systems;
    std::vector<System*> applicationSystems;
    entt::registry registry;
    bool initialized;
    bool running;

    // Internal systems
    System* worldSystem;
    System* controllerSystem;
    System* renderSystem;

    GLFWwindow* window;
};