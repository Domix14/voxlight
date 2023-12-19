#include "voxel_engine.hpp"

#include <GLFW/glfw3.h>

#include <chrono>
#include <stdexcept>

// clang-format off
#include "world/world_system.hpp"
#include "rendering/render_system.hpp"
#include "controller/controller_system.hpp"
// clang-format on
#include "engine_config.hpp"

static GLFWwindow* initGLFW() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW\n");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, WindowTitle, nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window\n");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    return window;
}

void VoxelEngine::init() {
    if (initialized) {
        throw std::runtime_error("VoxelEngine is already initialized\n");
    }

    // Initialize GLFW window
    window = initGLFW();

    controllerSystem.init();
    renderSystem.init();
    worldSystem.init();

    initialized = true;
}

void VoxelEngine::deinit() {
    // Deinitialize GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    initialized = false;
}

void VoxelEngine::run() {
    if (!initialized) {
        throw std::runtime_error("VoxelEngine is not initialized\n");
    }

    auto lastTime = std::chrono::system_clock::now();
    float deltaTime = 0.f;
    while (running && !glfwWindowShouldClose(window)) {
        auto currentTime = std::chrono::system_clock::now();
        worldSystem.update(deltaTime);
        controllerSystem.update(deltaTime);
        renderSystem.update(deltaTime);
        deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
    }
    deinit();
}
