#include <api/voxlight_api.hpp>
#include <spdlog/spdlog.h>
#include <voxlight.hpp>

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

void EngineApi::addSystemInternal(System *newSystem) { voxlight.customSystems.push_back(newSystem); }

GLFWwindow *EngineApi::getGLFWwindow() { return voxlight.glfwWindow; }

entt::registry &EngineApi::getRegistry() { return voxlight.registry; }
