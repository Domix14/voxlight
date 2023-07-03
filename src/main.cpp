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

static std::vector<GLubyte> pixels(800*800*3, 100);
static VoxelMap voxelMap;
static CPURenderer renderer(800, 800);

void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


int main()
{
    std::string str1 = "Testing";
    std::string str2 = std::move(str1);

    // Well, we use str1 after a std::move(). Not that smart :)
    // clang-tidy will find this!
    std::cout << str1 << str2;

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
    GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL Template", nullptr, nullptr);
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
    glViewport(0, 0, 800, 800);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    });

    auto vetexShader = createShader(GL_VERTEX_SHADER, "../shaders/vertexShader.txt");
    auto fragmentShader = createShader(GL_FRAGMENT_SHADER, "../shaders/fragmentShader.txt");

    if(std::get<1>(vetexShader) == ReturnCode::FAILURE || std::get<1>(fragmentShader) == ReturnCode::FAILURE) {
        std::cout << "Failed to create shaders\n";
        return -1;
    }

    // auto shaderProgram = glCreateProgram();
    // glAttachShader(shaderProgram, std::get<0>(vetexShader));
    // glAttachShader(shaderProgram, std::get<0>(fragmentShader));
    // glLinkProgram(shaderProgram);
    
    // GLuint vertexArrayID;
	// glGenVertexArrays(1, &vertexArrayID);
	// glBindVertexArray(vertexArrayID);

    // GLuint vertexBuffer;
    // glGenBuffers(1, &vertexBuffer);
    // glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // GLuint pixelBuffer;
    // glGenBuffers(1, &pixelBuffer);
    // glBindBuffer(GL_PIXEL_PACK_BUFFER, pixelBuffer);
    // glBufferData(GL_PIXEL_PACK_BUFFER, 800*800, 0, GL_STREAM_READ);

    // for(std::size_t x = 0; x < 800; ++x) {
    //     for(std::size_t y = 0; y < 800; ++y) {
    //         float pixel[4] = {0.5, 0.5, 0.5, 0.5};
    //         glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &pixel);
    //     }
    // }

    // GLuint colorBuffer;
    // glGenBuffers(1, &colorBuffer);
    // glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    // glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.1f, 100.0f);
    // glm::mat4 view = glm::lookAt(
    //     glm::vec3(4,3,-3), // Camera is at (4,3,3), in World Space
    //     glm::vec3(0,0,0), // and looks at the origin
    //     glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
    // );
    // glm::mat4 model = glm::mat4(1.0f);
    // glm::mat4 mvp = projection * view * model;

    // GLuint matrixID = glGetUniformLocation(shaderProgram, "MVP");

    // Enable depth test
    // glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    // glDepthFunc(GL_LESS);
    // This is the render loop
    glClearColor(0.0f, 0.0f, 0.1f, 0.0f);

    // void* pixels = nullptr;

    float time = 0;
    float radius = 20.0f;
    camera.setPosition({20.f,10.f,20.f});
    camera.setDirection(glm::normalize(glm::vec3(10.f,0,10.f) - camera.getPosition()));
    while (!glfwWindowShouldClose(window))
    {
        time += 0.01f;
        ProcessInput(window);
        camera.update(window);
        
        // camera.setPosition({glm::sin(time)*radius, 10.f, 1.0f*glm::cos(time)*radius});
        // glm::vec3 camPos = {glm::sin(time)*radius, 10.f, 1.0f*glm::cos(time)*radius};
        // camera.setDirection(glm::normalize(glm::vec3(0,0,0) - camera.getPosition()));
        // glm::vec3 camDir = glm::normalize(glm::vec3(0,0,0) - camPos);

        // glm::vec3 camRight = {1,0,0};
        // glm::vec3 camUp = {0,0,1};
        
        // Druids are the best
        // glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
        // glClear(GL_COLOR_BUFFER_BIT);

        // std::thread thread(refreshImage, camPos, camRight, camUp, planeLocation);
        // thread.join();

        glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
        // refreshImage(camPos, camDir);
        // glDrawPixels(800, 800, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
        renderer.process(&camera);
        renderer.draw();

        // glUseProgram(shaderProgram);

        // glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);

        // // 1st attribute buffer : vertices
        // glEnableVertexAttribArray(0);

        // glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        // glVertexAttribPointer(
        // 0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        // 3,                  // size
        // GL_FLOAT,           // type
        // GL_FALSE,           // normalized?
        // 0,                  // stride
        // (void*)0            // array buffer offset
        // );

        // glEnableVertexAttribArray(1);
        // glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        // glVertexAttribPointer(
        //     1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        //     3,                                // size
        //     GL_FLOAT,                         // type
        //     GL_FALSE,                         // normalized?
        //     0,                                // stride
        //     (void*)0                          // array buffer offset
        // );

        // // Draw the triangle !
        // glDrawArrays(GL_TRIANGLES, 0, 12*3); // Starting from vertex 0; 3 vertices total -> 1 triangle
        // glDisableVertexAttribArray(0);
        // glDisableVertexAttribArray(1);
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup VBO
	// glDeleteBuffers(1, &vertexBuffer);
	// glDeleteVertexArrays(1, &vertexArrayID);
	// glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}