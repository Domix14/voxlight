#include <iostream>

#include <glad/gl.h>
#include <thread>

#include <GLFW/glfw3.h>
#include "utils.hpp"
#include <VoxelMap.hpp>
#include <PerlinNoise.hpp>
#include <Chunk.hpp>
#include <Camera.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <vector>
#include <cstdint>
#include <cmath>

// force laptop to use dedicated gpu
#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) uint32_t  NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#ifdef __cplusplus
}
#endif

constexpr std::uint32_t WINDOW_WIDTH = 1280;
constexpr std::uint32_t WINDOW_HEIGHT = 720;

static VoxelMap voxelMap(16);

// cube vertices with size 1 and origin in 0, 0, 0

static const GLfloat cubeBufferData[] = {
    0.0f, 0.0f, 0.0f, // Vertex 0
    0.0f, 0.0f, 1.0f, // Vertex 1
    0.0f, 1.0f, 0.0f, // Vertex 2

    0.0f, 0.0f, 1.0f, // Vertex 1
    0.0f, 1.0f, 0.0f, // Vertex 2
    0.0f, 1.0f, 1.0f, // Vertex 3

    1.0f, 0.0f, 0.0f, // Vertex 4
    1.0f, 0.0f, 1.0f, // Vertex 5
    1.0f, 1.0f, 0.0f, // Vertex 6

    1.0f, 0.0f, 1.0f, // Vertex 5
    1.0f, 1.0f, 0.0f, // Vertex 6
    1.0f, 1.0f, 1.0f, // Vertex 7

    0.0f, 0.0f, 0.0f, // Vertex 0
    0.0f, 0.0f, 1.0f, // Vertex 1
    1.0f, 0.0f, 0.0f, // Vertex 4

    0.0f, 0.0f, 1.0f, // Vertex 1
    1.0f, 0.0f, 0.0f, // Vertex 4
    1.0f, 0.0f, 1.0f, // Vertex 5

    0.0f, 1.0f, 0.0f, // Vertex 2
    0.0f, 1.0f, 1.0f, // Vertex 3
    1.0f, 1.0f, 0.0f, // Vertex 6

    0.0f, 1.0f, 1.0f, // Vertex 3
    1.0f, 1.0f, 0.0f, // Vertex 6
    1.0f, 1.0f, 1.0f, // Vertex 7

    0.0f, 0.0f, 0.0f, // Vertex 0
    0.0f, 1.0f, 0.0f, // Vertex 2
    1.0f, 0.0f, 0.0f, // Vertex 4

    0.0f, 1.0f, 0.0f, // Vertex 2
    1.0f, 0.0f, 0.0f, // Vertex 4
    1.0f, 1.0f, 0.0f, // Vertex 6

    0.0f, 0.0f, 1.0f, // Vertex 1
    0.0f, 1.0f, 1.0f, // Vertex 3
    1.0f, 0.0f, 1.0f, // Vertex 5

    0.0f, 1.0f, 1.0f, // Vertex 3
    1.0f, 0.0f, 1.0f, // Vertex 5
    1.0f, 1.0f, 1.0f  // Vertex 7      
};

static GLuint createTexture() {
    // GLuint texname;
    // glGenBuffers(1, &texname);
    // glBindBuffer(GL_TEXTURE_BUFFER, texname);
    // auto size = voxelMap.getWorldSize();
    // glBufferData(GL_TEXTURE_BUFFER, size*size*size, voxelMap.getVoxelData(), GL_STATIC_DRAW);
    // return texname;

    GLuint texname;
    glGenTextures(1, &texname);
    glBindTexture(GL_TEXTURE_3D, texname);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    auto size = voxelMap.getWorldSize();
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, size, size, size, 0, GL_RED, 
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

    std::vector<GLubyte> data;
    std::cout << data.max_size() << std::endl;
    Camera camera;

    voxelMap.generateRandom();
    std::vector<Chunk> chunks;

    // Construct the window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Doom3D", nullptr, nullptr);
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
    
    glClearColor(0.529f, 0.8f,  0.92f, 0.f);

    GLuint programID = glCreateProgram();
    auto [vertexShader, vertexShaderStatus] = createShader(GL_VERTEX_SHADER, "../shaders/vertexShader.txt");
    auto [fragmentShader, fragmentShaderStatus] = createShader(GL_FRAGMENT_SHADER, "../shaders/fragmentShader.txt");
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeBufferData), cubeBufferData, GL_STATIC_DRAW);

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

    std::size_t chunkSize = 64;
    std::size_t chunksCount = 8;
    siv::PerlinNoise perlin{ 4321 };
    for(std::size_t i = 0;i < chunksCount;++i) {
        for(std::size_t j = 0;j < chunksCount;++j) {
            std::vector<GLubyte> data(chunkSize*chunkSize*chunkSize);
            for(std::size_t x = 0;x < chunkSize;++x) {
                for(std::size_t z = 0; z < chunkSize;++z) {
                    std::size_t xvalue = i*chunkSize + x;
                    std::size_t zvalue = j*chunkSize + z;
                    std::size_t height = perlin.octave2D_01(xvalue*0.01, zvalue*0.01, 4)*chunkSize;
                    // std::cout << perlin.octave2D_01(xvalue/divider, zvalue/divider, 1) << std::endl;
                    // std::cout << xvalue/divider << "  " << zvalue/divider << "\n";
                    for(std::size_t y = 0; y < height;++y) {
                        data[x + y*chunkSize + z*chunkSize*chunkSize] = (i%2)+1;
                    }
                }
            }
            chunks.emplace_back(glm::vec3(i*chunkSize, 0, j*chunkSize), glm::vec3(chunkSize), data);
        }
    }
    std::cout << "Chunks count: " << chunks.size() << std::endl;

    glUseProgram(programID);
    GLuint camPosID = glGetUniformLocation(programID, "inCamPos");
    GLuint camDirID = glGetUniformLocation(programID, "inCamDir");
    GLuint textureID  = glGetUniformLocation(programID, "chunkTexture");
    GLuint sunPosID  = glGetUniformLocation(programID, "sunPos");

    GLuint materialsID = glGetUniformLocation(programID, "materials");

    GLint texture_units;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
    std::cout << "Max texture units: " << texture_units << std::endl;

    camera.setPosition({10, 10, 10});
    camera.setDirection(glm::normalize(glm::vec3(0,0,5) - camera.getPosition()));

    float materials[] = {
            0, 1, 0,
            0.5, 0.5, 0.5,
        };
    double currentFrame = 0;
    double deltaTime = 0;
    double lastFrame = 0;
    double sunRotation = 0;

    std::vector<std::size_t> drawOrder;
    for(std::size_t i = 0;i < chunks.size();++i) {
        drawOrder.push_back(i);
    }
    while (!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        sunRotation += deltaTime * 0.1f;

        auto worldSize = voxelMap.getWorldSize();
        auto sunPosition = glm::vec3(glm::sin(sunRotation)*worldSize, worldSize, glm::cos(sunRotation)*worldSize);
        ProcessInput(window);
        camera.update(window, deltaTime);

       

        //glClear(GL_COLOR_BUFFER_BIT);
       
        // renderer.process(&camera);

        glfwPollEvents();

        
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programID);

        auto pos = camera.getPosition();
        auto dir = camera.getDirection();

         glm::mat4 projection = glm::perspective(
            glm::radians(90.f), // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
            4.0f / 3.0f,       // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
            0.5f,              // Near clipping plane. Keep as big as possible, or you'll get precision issues.
            1000.0f             // Far clipping plane. Keep as little as possible.
        );

        glm::mat4 view = glm::lookAt(
            pos, // the position of your camera, in world space
            pos+(dir*1000.f),   // where you want to look at, in world space
            glm::vec3(0,1,0)        // probably glm::vec3(0,1,0), but (0,-1,0) would make you looking upside-down, which can be great too
        );

        

        glUniformMatrix4fv(glGetUniformLocation(programID, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(programID, "view"), 1, GL_FALSE, &view[0][0]);

        glUniform3f(camPosID, pos.x, pos.y, pos.z);
        glUniform3f(camDirID, dir.x, dir.y, dir.z);
        glUniform3f(sunPosID, sunPosition.x, sunPosition.y, sunPosition.z);
        
        glUniform3fv(materialsID, 6, materials);

        // Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
        
		// glBindTexture(GL_TEXTURE_BUFFER, texture);
        // glTexBuffer(GL_TEXTURE_BUFFER, GL_R8, texture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		

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

        std::sort(drawOrder.begin(), drawOrder.end(), [&chunks, &camera](std::size_t a, std::size_t b) {
            auto aPos = chunks[a].getMinBox() + glm::vec3(8);
            auto bPos = chunks[b].getMinBox() + glm::vec3(8);
            auto aDist = glm::distance(aPos, camera.getPosition());
            auto bDist = glm::distance(bPos, camera.getPosition());
            return aDist > bDist;
        });

        for(std::size_t i = 0;i < drawOrder.size();++i) {
            auto chunk = chunks[drawOrder[i]];
            auto size = chunk.getSize();
            glm::mat4 model = glm::translate(glm::mat4(1.0f), chunk.getMinBox());
            model = glm::scale(model, size);
            glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &model[0][0]);
            auto minBox = chunk.getMinBox();
            auto maxBox = chunk.getMaxBox();
            glUniform3f(glGetUniformLocation(programID, "minBox"), minBox.x, minBox.y, minBox.z);
            glUniform3f(glGetUniformLocation(programID, "maxBox"), maxBox.x, maxBox.y, maxBox.z);
            glUniform3f(glGetUniformLocation(programID, "chunkSize"), size.x, size.y, size.z);

            glBindTexture(GL_TEXTURE_3D, chunk.getTexture());
            glUniform1i(textureID, 0);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 3 indices starting at 0 -> 1 triangle
        }
		// Draw the triangle !
		// glDrawArrays(GL_TRIANGLES, 0, 36); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(0);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            ImGui::SetNextWindowPos(ImVec2(0,0));
            ImGui::SetNextWindowSize(ImVec2(200, 50));
            ImGui::Begin("Camera position");                          // Create a window called "Hello, world!" and append into it.
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}