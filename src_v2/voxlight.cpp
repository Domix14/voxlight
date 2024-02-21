#include "voxlight.hpp"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <stdexcept>

#include "core/components.hpp"
#include "engine_config.hpp"

static GLFWwindow *initGLFW() {
  if(!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW\n");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  GLFWwindow *window = glfwCreateWindow(WindowWidth, WindowHeight, WindowTitle, nullptr, nullptr);
  if(!window) {
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window\n");
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  return window;
}

Voxlight::Voxlight() : registry(), renderSystem(*this) {}

void Voxlight::init() {
  // Initialize GLFW window
  glfwWindow = initGLFW();

  // Initialize internal systems
  renderSystem.init();

  // Initialize custom systems
  for(auto &system : customSystems) {
    system->init();
  }
}

void Voxlight::run() {
  isRunning = true;
  auto lastTime = std::chrono::system_clock::now();
  float deltaTime = 0.f;

  if(entt::null == currentCamera || !registry.all_of<CameraComponent>(currentCamera)) {
    spdlog::warn("No camera found. Creating a default one.");
    auto camera = EntityApi(*this).createEntity("default_camera", TransformComponent());
    CameraComponentApi(*this).addComponent(camera);
    CameraComponentApi(*this).setCurrentCamera(camera);
  }

  while(isRunning && !glfwWindowShouldClose(glfwWindow)) {
    auto currentTime = std::chrono::system_clock::now();
    for(auto &system : customSystems) {
      system->update(deltaTime);
    }

    renderSystem.update(deltaTime);
    deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
    lastTime = currentTime;
  }
  isRunning = false;
}

void Voxlight::deinit() {
  // Deinitialize internal systems
  // worldSystem.deinit();
  // renderSystem.deinit();
  // controllerSystem.deinit();

  // Deinitialize custom systems
  for(auto &system : customSystems) {
    system->deinit();
  }

  // Deinitialize GLFW
  glfwDestroyWindow(glfwWindow);
  glfwTerminate();
}

void Voxlight::stop() {}
