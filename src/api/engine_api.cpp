#include <spdlog/spdlog.h>

#include <core/components.hpp>
#include <core/voxlight.hpp>
#include <voxlight_api.hpp>

EngineApi::EngineApi(Voxlight &voxlight) : voxlight(voxlight) {}

void EngineApi::start() {
  if(voxlight.isRunning) {
    spdlog::error("Failed to execure run(). Engine is already running.");
    return;
  }

  voxlight.init();
  voxlight.run();
}

void EngineApi::stop() {
  if(!voxlight.isRunning) {
    spdlog::error("Failed to execure stop(). Engine is not running.");
    return;
  }

  voxlight.stop();
}

void EngineApi::addSystemInternal(std::unique_ptr<System> newSystem) {
  voxlight.customSystems.push_back(std::move(newSystem));
}

GLFWwindow *EngineApi::getGLFWwindow() { return voxlight.glfwWindow; }

entt::registry &EngineApi::getRegistry() { return voxlight.registry; }

void EngineApi::subscribe(EngineEventType eventType, EngineEventCallback listener) {
  voxlight.engineEventManager.subscribe(eventType, listener);
}

void EngineApi::setWindowResolution(int width, int height) {
  spdlog::info("Window resized to {}x{}", width, height);

  voxlight.windowWidth = width;
  voxlight.windowHeight = height;
  voxlight.engineEventManager.publish(EngineEventType::OnWindowResize,
                                      EngineEvent(voxlight.windowWidth, voxlight.windowHeight));
}