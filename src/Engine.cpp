#include "Engine.hpp"

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <imgui.h>

#include <Chunk.hpp>
#include <ICamera.hpp>
#include <PerlinNoise.hpp>
#include <VoxelWorld.hpp>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "utils.hpp"

// cube vertices with size 1 and origin in 0, 0, 0

void ProcessInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

Engine::Engine() : voxelWorld(nullptr), camera(nullptr), windowWidth(1280), windowHeight(720) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    window = glfwCreateWindow(windowWidth, windowHeight, "VoxelEngine", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create the GLFW window\n";
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return;
    }

    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetWindowUserPointer(window, this);
    auto framebufferSizeCallback = [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        static_cast<Engine*>(glfwGetWindowUserPointer(window))->setWindowSize(width, height);
    };
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glClearColor(0.529f, 0.8f, 0.92f, 0.f);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void Engine::run() {
    voxelWorld = new VoxelWorld(this);
    voxelWorld->init();

    auto knightData = loadVox("knight.vox");
    auto planeData = loadVox("plane.vox");
    auto cube = loadVox("cube.vox");

    auto knight = voxelWorld->spawnVoxelObject();
    knight->setData(knightData.data, knightData.size);
    knight->setPosition({1, 1, 1});

    auto knight2 = voxelWorld->spawnVoxelObject();
    knight2->setData(knightData.data, knightData.size);
    knight2->setPosition({2, 2, 2});

    auto knight3 = voxelWorld->spawnVoxelObject();
    knight3->setData(knightData.data, knightData.size);
    knight3->setPosition({3, 3, 3});

    double currentFrame = 0;
    double deltaTime = 0;
    double lastFrame = 0;

    while (!glfwWindowShouldClose(window)) {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ProcessInput(window);

        camera->update(window, deltaTime);
        voxelWorld->update(deltaTime);

        glfwPollEvents();

        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        voxelWorld->render(camera->getViewProjectionMatrix());

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(200, 50));
            ImGui::Begin("Camera position");  // Create a window called "Hello, world!" and append into it.
            ImGuiIO& io = ImGui::GetIO();
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glfwTerminate();
}

void Engine::setCamera(ICamera* newCamera) { camera = newCamera; }

void Engine::setWindowSize(std::uint32_t width, std::uint32_t height) {
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

std::pair<std::uint32_t, std::uint32_t> Engine::getWindowSize() const { return {windowWidth, windowHeight}; }