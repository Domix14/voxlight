#include <iostream>

#include <glad/gl.h>
#include <thread>

#include <GLFW/glfw3.h>
#include "utils.hpp"
#include <VoxelMap.hpp>
#include <Camera.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <vector>
#include <cstdint>
#include <cmath>

constexpr std::uint32_t WINDOW_WIDTH = 1920;
constexpr std::uint32_t WINDOW_HEIGHT = 1080;

static VoxelMap voxelMap(128);

static GLuint createTexture() {
    GLuint texname;
    glGenTextures(1, &texname);
    glBindTexture(GL_TEXTURE_3D, texname);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    auto size = voxelMap.getWorldSize();
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB8, size, size, size, 0, GL_RGB, 
                GL_UNSIGNED_BYTE, voxelMap.getVoxelData());
    return texname;
}

void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


int main()
{
    glfwInit();
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    Camera camera;

    voxelMap.addSpehere(glm::vec3(0,10.f,0), 15.f, VoxelType::Grass);
    voxelMap.addSpehere(glm::vec3(40.f,40.f,40.f), 20.f, VoxelType::Water);
    voxelMap.addPlane(glm::vec3(0.f, 0.f, 0.f), 60.f, 60.f, VoxelType::Dirt);
    voxelMap.addPlane(glm::vec3(25.f, 1.f, 25.f), 50.f, 50.f, VoxelType::Water);
    voxelMap.setVoxel({0,0,0}, VoxelType::Grass);

    // Construct the window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Template", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create the GLFW window\n";
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
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

    GLuint programID = glCreateProgram();
    // auto [vertexShader, vertexShaderStatus] = createShader(GL_VERTEX_SHADER, "../shaders/vertexShader.txt");
    auto [fragmentShader, fragmentShaderStatus] = createShader(GL_FRAGMENT_SHADER, "../shaders/fragmentShader.txt");
    // glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	static const GLfloat g_vertex_buffer_data[] = { 
		-1.0f, -1.0f, 0.0f,
		 -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,

         1.0f, 1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 -1.0f,  -1.0f, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    GLuint texture = createTexture();

    glUseProgram(programID);
    GLuint camPosID = glGetUniformLocation(programID, "inCamPos");
    GLuint camDirID = glGetUniformLocation(programID, "inCamDir");
    GLuint TextureID  = glGetUniformLocation(programID, "worldTexture");

    camera.setPosition({10.f,10.f,10.f});
    camera.setDirection(glm::normalize(glm::vec3(0.f,0.f,0.f) - camera.getPosition()));

    double currentFrame = 0;;
    double deltaTime = 0;
    double lastFrame = 0;
    while (!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ProcessInput(window);
        camera.update(window, deltaTime);

        glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
       
        // renderer.process(&camera);

        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programID);

        auto pos = camera.getPosition();
        auto dir = camera.getDirection();

        glUniform3f(camPosID, pos.x, pos.y, pos.z);
        glUniform3f(camDirID, dir.x, dir.y, dir.z);

        // Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, texture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureID, 0);

        glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 6); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(0);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}