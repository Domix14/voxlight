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
  voxlight.deinit();
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

GLFWwindow *EngineApi::getGLFWwindow() const { return voxlight.glfwWindow; }

entt::registry &EngineApi::getRegistry() const { return voxlight.registry; }

void EngineApi::subscribe(EngineEventType eventType, EngineEventCallback listener) {
  voxlight.engineEventManager.subscribe(eventType, listener);
}

void EngineApi::setWindowResolution(std::uint32_t width, std::uint32_t height) {
  voxlight.windowWidth = static_cast<std::uint32_t>(width);
  voxlight.windowHeight = static_cast<std::uint32_t>(height);

  WindowResizeEvent event = {voxlight.windowWidth, voxlight.windowHeight};
  voxlight.engineEventManager.publish(EngineEventType::OnWindowResize, event);
}
