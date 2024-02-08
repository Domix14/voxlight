#include "voxlight.hpp"

#include <GLFW/glfw3.h>

#include <chrono>
#include <stdexcept>

// clang-format off
#include "world/world_system.hpp"
#include "rendering/render_system.hpp"
#include "controller/controller_system.hpp"
// clang-format on
#include "engine_config.hpp"

static GLFWwindow *initGLFW() {
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW\n");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  GLFWwindow *window = glfwCreateWindow(WindowWidth, WindowHeight, WindowTitle,
                                        nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window\n");
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  return window;
}

void Voxlight::init() {
  // Initialize GLFW window
  glfwWindow = initGLFW();

  // Initialize internal systems
  // controllerSystem.init(this);
  // renderSystem.init(this);
  // worldSystem.init(this);

  // Initialize custom systems
  for (auto system : customSystems) {
    system->init(this);
  }
}

void Voxlight::run() {
  isRunning = true;
  auto lastTime = std::chrono::system_clock::now();
  float deltaTime = 0.f;

  while (isRunning && !glfwWindowShouldClose(glfwWindow)) {
    auto currentTime = std::chrono::system_clock::now();
    // worldSystem.update(deltaTime);
    // controllerSystem.update(deltaTime);
    // renderSystem.update(deltaTime);
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
  for (auto system : customSystems) {
    system->deinit();
    delete system;
  }

  // Deinitialize GLFW
  glfwDestroyWindow(glfwWindow);
  glfwTerminate();
}

void Voxlight::stop() {}