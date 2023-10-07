#include "Engine.hpp"

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <imgui.h>

#include <Chunk.hpp>
#include <ICamera.hpp>
#include <PerlinNoise.hpp>
#include <VoxelMap.hpp>
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

Engine::Engine() : voxelWorld(nullptr) {}

void Engine::run() {
    glfwInit();
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    // Construct the window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "VoxelEngine", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create the GLFW window\n";
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return;
    }

    // Handle view port dimensions
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window,
                                   [](GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); });

    glClearColor(0.529f, 0.8f, 0.92f, 0.f);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w,
                 clear_color.w);
    while (!glfwWindowShouldClose(window)) {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ProcessInput(window);

        camera->update(window, deltaTime);
        return;
        voxelWorld->update(deltaTime);

        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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