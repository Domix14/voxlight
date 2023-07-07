#include <iostream>

#include <glad/gl.h>
#include <thread>

#include <GLFW/glfw3.h>
#include "utils.hpp"
#include <VoxelMap.hpp>
#include <CPURenderer.hpp>
#include <Camera.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <cstdint>
#include <cmath>

constexpr std::uint32_t WINDOW_WIDTH = 800;
constexpr std::uint32_t WINDOW_HEIGHT = 800;

static std::vector<GLubyte> pixels(WINDOW_WIDTH*WINDOW_HEIGHT*3, 100);
static VoxelMap voxelMap;
static CPURenderer renderer(WINDOW_WIDTH, WINDOW_HEIGHT);

void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    Camera camera;

    voxelMap.addSpehere(glm::vec3(0,10.f,0), 15.f, VoxelType::Grass);
    voxelMap.addSpehere(glm::vec3(40.f,40.f,40.f), 20.f, VoxelType::Stone);
    voxelMap.addPlane(glm::vec3(0.f, 0.f, 0.f), 60.f, 60.f, VoxelType::Dirt);
    voxelMap.addPlane(glm::vec3(25.f, 1.f, 25.f), 50.f, 50.f, VoxelType::Water);
    renderer.setVoxelMap(&voxelMap);

    // Construct the window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Template", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create the GLFW window\n";
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    // Handle view port dimensions
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    });
    
    glClearColor(0.0f, 0.0f, 0.1f, 0.0f);

    camera.setPosition({5.f,5.f,5.f});
    camera.setDirection(glm::normalize(glm::vec3(0.f,0.f,0.f) - camera.getPosition()));
    while (!glfwWindowShouldClose(window))
    {
        ProcessInput(window);
        camera.update(window);

        glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
       
        renderer.process(&camera);
        renderer.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}