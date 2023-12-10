#include "render_system.hpp"

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <spdlog/spdlog.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "controller/controller_system.hpp"
#include "core/components.hpp"
#include "engine_config.hpp"
#include "generated/shaders.hpp"
#include "voxel_engine.hpp"

static const GLfloat cubeVertexData[] = {
    0.0f, 0.0f, 0.0f,  // Vertex 0
    0.0f, 0.0f, 1.0f,  // Vertex 1
    0.0f, 1.0f, 0.0f,  // Vertex 2

    0.0f, 0.0f, 1.0f,  // Vertex 1
    0.0f, 1.0f, 0.0f,  // Vertex 2
    0.0f, 1.0f, 1.0f,  // Vertex 3

    1.0f, 0.0f, 0.0f,  // Vertex 4
    1.0f, 0.0f, 1.0f,  // Vertex 5
    1.0f, 1.0f, 0.0f,  // Vertex 6

    1.0f, 0.0f, 1.0f,  // Vertex 5
    1.0f, 1.0f, 0.0f,  // Vertex 6
    1.0f, 1.0f, 1.0f,  // Vertex 7

    0.0f, 0.0f, 0.0f,  // Vertex 0
    0.0f, 0.0f, 1.0f,  // Vertex 1
    1.0f, 0.0f, 0.0f,  // Vertex 4

    0.0f, 0.0f, 1.0f,  // Vertex 1
    1.0f, 0.0f, 0.0f,  // Vertex 4
    1.0f, 0.0f, 1.0f,  // Vertex 5

    0.0f, 1.0f, 0.0f,  // Vertex 2
    0.0f, 1.0f, 1.0f,  // Vertex 3
    1.0f, 1.0f, 0.0f,  // Vertex 6

    0.0f, 1.0f, 1.0f,  // Vertex 3
    1.0f, 1.0f, 0.0f,  // Vertex 6
    1.0f, 1.0f, 1.0f,  // Vertex 7

    0.0f, 0.0f, 0.0f,  // Vertex 0
    0.0f, 1.0f, 0.0f,  // Vertex 2
    1.0f, 0.0f, 0.0f,  // Vertex 4

    0.0f, 1.0f, 0.0f,  // Vertex 2
    1.0f, 0.0f, 0.0f,  // Vertex 4
    1.0f, 1.0f, 0.0f,  // Vertex 6

    0.0f, 0.0f, 1.0f,  // Vertex 1
    0.0f, 1.0f, 1.0f,  // Vertex 3
    1.0f, 0.0f, 1.0f,  // Vertex 5

    0.0f, 1.0f, 1.0f,  // Vertex 3
    1.0f, 0.0f, 1.0f,  // Vertex 5
    1.0f, 1.0f, 1.0f   // Vertex 7
};

static const GLfloat quadVertexData[] = {-1.f, -1.f, 0.f, 1.f, 1.f, 0.f, 1.f,  -1.f, 0.f,
                                         -1.f, 1.f,  0.f, 1.f, 1.f, 0.f, -1.f, -1.f, 0.f};

static void frameBufferCheck() {
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        spdlog::error("Framebuffer is not complete");
    }
}

// static GLuint createVoxelTexture(std::vector<GLubyte> const& data, glm::vec3 size) {
//     GLuint texname;
//     glGenTextures(1, &texname);
//     glBindTexture(GL_TEXTURE_3D, texname);
//     glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

//     glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

//     glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, size.x, size.y, size.z, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
//     return texname;
// }

static std::tuple<GLuint, int> loadShader(GLenum shaderType, std::string_view shaderCode) {
    auto shader = glCreateShader(shaderType);
    const char* c_str = shaderCode.data();
    glShaderSource(shader, 1, &c_str, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        spdlog::error("Failed to compile shader: {}", infoLog);
        return {0, -1};
    }
    return {shader, 0};
}

static GLuint createProgram(std::string_view vertexSrc, std::string_view fragmentSrc) {
    GLuint program = glCreateProgram();
    auto [vertexShader, vertexShaderStatus] = loadShader(GL_VERTEX_SHADER, vertexSrc);
    auto [fragmentShader, fragmentShaderStatus] = loadShader(GL_FRAGMENT_SHADER, fragmentSrc);
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    return program;
}

RenderSystem::RenderSystem(VoxelEngine* engine) : System(engine) {}

void RenderSystem::init() {
    // Init OpenGL
    if (!gladLoadGL(glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD\n");
    }

    // Save pointer to GLFW window
    window = getEngine()->getWindow();

    // Set window resize callback
    glViewport(0, 0, WindowWidth, WindowHeight);
    glfwSetWindowUserPointer(window, this);
    auto framebufferSizeCallback = [](GLFWwindow*, int width, int height) { glViewport(0, 0, width, height); };
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Set clear color
    glClearColor(0.529f, 0.8f, 0.92f, 0.f);

    // Create shader programs
    voxelProgram = createProgram(VOXEL_VERTEX_SHADER_SRC, VOXEL_FRAGMENT_SHADER_SRC);
    voxelUniform.modelMatrix = glGetUniformLocation(voxelProgram, "uModelMatrix");
    voxelUniform.viewProjectionMatrix = glGetUniformLocation(voxelProgram, "uViewProjectionMatrix");
    voxelUniform.invResolution = glGetUniformLocation(voxelProgram, "uInvResolution");
    voxelUniform.minBox = glGetUniformLocation(voxelProgram, "uMinBox");
    voxelUniform.maxBox = glGetUniformLocation(voxelProgram, "uMaxBox");
    voxelUniform.chunkSize = glGetUniformLocation(voxelProgram, "uChunkSize");
    voxelUniform.magicMatrix = glGetUniformLocation(voxelProgram, "uMagicMatrix");
    voxelUniform.chunkTexture = glGetUniformLocation(voxelProgram, "uChunkTexture");
    voxelUniform.paletteTexture = glGetUniformLocation(voxelProgram, "uPaletteTexture");

    sunglightProgram = createProgram(SUNLIGHT_VERTEX_SHADER_SRC, SUNLIGHT_FRAGMENT_SHADER_SRC);

    // Create vertex buffers
    glGenBuffers(1, &cubeVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexData), cubeVertexData, GL_STATIC_DRAW);

    glGenBuffers(1, &quadVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertexData), quadVertexData, GL_STATIC_DRAW);

    // Create framebuffers
    glGenFramebuffers(1, &mainFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mainFramebuffer);

    // Do we need this?
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WindowWidth, WindowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WindowWidth, WindowHeight, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &normalTexture);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB_SNORM, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, depthTexture, 0);
    frameBufferCheck();

    // Create palette texture
    glGenTextures(1, &paletteTexture);
    glBindTexture(GL_TEXTURE_2D, paletteTexture);
    // set the texture wrapping/filtering options (on the currently bound
    // texture object) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
    // GL_REPEAT); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("./palette.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        spdlog::error("Failed to load texture");
    }
    stbi_image_free(data);
}

void RenderSystem::deinit() {}

void RenderSystem::update(float) {
    glUseProgram(voxelProgram);
    glBindFramebuffer(GL_FRAMEBUFFER, mainFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto view = getEngine()->getRegistry().view<TransformComponent, VoxelComponent>();
    for (auto [entity, transform, voxelData] : view.each()) {
        glm::vec3 size = transform.scale;
        glm::vec3 minBox = transform.position;
        glm::vec3 maxBox = minBox + size;

        auto viewProjectionMatrix = getEngine()->getSystem<ControllerSystem>()->getViewProjectionMatrix();
        auto invViewProjectionMatrix = glm::inverse(viewProjectionMatrix);
        auto modelMatrix = glm::translate(glm::mat4(1.f), minBox);
        auto scaleMatrix = glm::scale(glm::mat4(1.f), size);
        modelMatrix = modelMatrix * scaleMatrix;
        auto mvp = viewProjectionMatrix * modelMatrix;
        glUniformMatrix4fv(voxelUniform.modelMatrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(voxelUniform.viewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(viewProjectionMatrix));
        glUniform2f(voxelUniform.invResolution, 1.f / 1280.f, 1.f / 720.f);
        glUniform3f(voxelUniform.minBox, minBox.x, minBox.y, minBox.z);
        glUniform3f(voxelUniform.maxBox, maxBox.x, maxBox.y, maxBox.z);
        glUniform3f(voxelUniform.chunkSize, size.x, size.y, size.z);
        glUniformMatrix4fv(voxelUniform.magicMatrix, 1, GL_FALSE, glm::value_ptr(invViewProjectionMatrix));

        glUniform1i(voxelUniform.chunkTexture, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, voxelData.textureId);

        // glUniform1i(voxelUniform.paletteTexture, 1);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, paletteTexture);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, mainFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, WindowWidth, WindowHeight, 0, 0, WindowWidth, WindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

unsigned int RenderSystem::createVoxelTexture(VoxelData<std::uint8_t> const& data) {
    GLuint texname;
    glGenTextures(1, &texname);
    glBindTexture(GL_TEXTURE_3D, texname);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, data.getWidth(), data.getHeight(), data.getDepth(), 0, GL_RED,
                 GL_UNSIGNED_BYTE, data.getData());
    auto err = glGetError();
    if (err != GL_NO_ERROR) {
        spdlog::error("Error: {}", err);
    }

    return texname;
}