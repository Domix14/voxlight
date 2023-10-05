#include "Engine.hpp"

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <imgui.h>

#include <Camera.hpp>
#include <Chunk.hpp>
#include <PerlinNoise.hpp>
#include <VoxelMap.hpp>
#include <VoxelSystem.hpp>
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

Engine::Engine() : entityCount(0), voxelSystem(nullptr) { voxelSystem = new VoxelSystem(this); }

std::uint32_t Engine::createEntity() {
    auto entity = entityCount++;
    voxelComponents.emplace_back();
    return entity;
}

std::uint32_t Engine::createVoxelEntity(glm::vec3 pos, glm::vec3 rot, glm::vec3 size, float voxSize,
                                        std::vector<std::uint8_t> const& voxelData) {
    auto entity = createEntity();
    voxelComponents[entity].position = pos;
    voxelComponents[entity].rotation = rot;
    voxelComponents[entity].size = size;
    voxelComponents[entity].voxelSize = voxSize;
    voxelComponents[entity].setVoxelData(voxelData);
    voxelSystem->addEntity(entity);
    return entity;
}

void Engine::run() {
    glfwInit();
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    std::vector<GLubyte> data;
    Camera camera;

    std::vector<Chunk> chunks;

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

    voxelSystem->initialise();
    auto knightData = loadVox("knight.vox");
    auto planeData = loadVox("plane.vox");
    auto cube = loadVox("cube.vox");
    // for(std::size_t i = 0;i < chunksCount;++i) {
    //     for(std::size_t j = 0;j < chunksCount;++j) {
    //         std::vector<GLubyte> data(chunkSize*chunkSize*chunkSize);
    //         for(std::size_t x = 0;x < chunkSize;++x) {
    //             for(std::size_t z = 0; z < chunkSize;++z) {
    //                 std::size_t xvalue = i*chunkSize + x;
    //                 std::size_t zvalue = j*chunkSize + z;
    //                 std::size_t height = perlin.octave2D_01(xvalue*0.01, zvalue*0.01, 4)*chunkSize;
    //                 // height = chunkSize;
    //                 for(std::size_t y = 0; y < height;++y) {
    //                     data[x + y*chunkSize + z*chunkSize*chunkSize] = (i%2)+1;
    //                 }
    //             }
    //         }
    //         createVoxelEntity(glm::vec3(i*chunkSize, 0, j*chunkSize), glm::vec3(glm::radians(i*0.f)),
    //         glm::vec3(chunkSize), data);
    //     }
    // }
    // createVoxelEntity(glm::vec3(0, 0, 0), glm::vec3(glm::radians(0.f)), planeData.size, VOXEL_SIZE_12CM,
    //                   planeData.data);

    // int i = 1;
    // createVoxelEntity(glm::vec3(5, 0.125, 5), glm::vec3(glm::radians(i * 0.f)), knightData.size, VOXEL_SIZE_12CM,
    //                   knightData.data);
    // createVoxelEntity(glm::vec3(1, 5, 1), glm::vec3(glm::radians(i*0.f)), cube.size, VOXEL_SIZE_12CM, cube.data);
    // i++;
    // createVoxelEntity(glm::vec3(i*16, 1, i*16), glm::vec3(glm::radians(i*0.f)), knightData.size, VOXEL_SIZE_50CM,
    // knightData.data); i++; createVoxelEntity(glm::vec3(i*16, 1, i*16), glm::vec3(glm::radians(i*0.f)),
    // knightData.size, VOXEL_SIZE_25CM, knightData.data); i++; createVoxelEntity(glm::vec3(i*16, 1, i*16),
    // glm::vec3(glm::radians(i*0.f)), knightData.size, VOXEL_SIZE_12CM, knightData.data);

    auto knight = voxelSystem->createVoxelObject();
    knight->setData(knightData.data, knightData.size);
    knight->setPosition({1, 1, 1});

    voxelSystem->createWorldVoxelTexture();

    // GLint texture_units;
    // glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
    // std::cout << "Max texture units: " << texture_units << std::endl;

    camera.setPosition({-2, -2, -2});
    camera.setDirection(glm::normalize(glm::vec3(1, 1, 1) - camera.getPosition()));

    double currentFrame = 0;
    double deltaTime = 0;
    double lastFrame = 0;

    // auto lightTexture = createVoxelTexture(voxelData, glm::vec3(chunkSize));

    // createVoxelEntity(glm::vec3(0,0,0), glm::vec3(0), glm::vec3(16), voxelData);

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
        camera.update(window, deltaTime);

        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        voxelSystem->update(deltaTime, camera);
        // Draw the triangle !
        // glDrawArrays(GL_TRIANGLES, 0, 36); // 3 indices starting at 0 -> 1 triangle

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