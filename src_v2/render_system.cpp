#include "rendering/render_system.hpp"

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <spdlog/spdlog.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "controller/controller_system.hpp"
#include "core/components.hpp"
#include "core/voxel_data.hpp"
#include "engine_config.hpp"
#include "generated/shaders.hpp"
#include "rendering/palette.hpp"
#include "voxlight.hpp"

#include "api/voxlight_api.hpp"

static const GLfloat cubeVertexData[] = {
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

static const GLfloat quadVertexData[] = {-1.f, -1.f, 0.f, 1.f,  1.f,  0.f,
                                         1.f,  -1.f, 0.f, -1.f, 1.f,  0.f,
                                         1.f,  1.f,  0.f, -1.f, -1.f, 0.f};

static void frameBufferCheck() {
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    spdlog::error("Framebuffer is not complete");
  }
}

static std::tuple<GLuint, int> loadShader(GLenum shaderType,
                                          std::string_view shaderCode) {
  auto shader = glCreateShader(shaderType);
  const char *c_str = shaderCode.data();
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

static GLuint createProgram(std::string_view vertexSrc,
                            std::string_view fragmentSrc) {
  GLuint program = glCreateProgram();
  auto [vertexShader, vertexShaderStatus] =
      loadShader(GL_VERTEX_SHADER, vertexSrc);
  auto [fragmentShader, fragmentShaderStatus] =
      loadShader(GL_FRAGMENT_SHADER, fragmentSrc);
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);
  return program;
}

RenderSystem::RenderSystem(Voxlight &voxlight) : System(voxlight) {}

void RenderSystem::init() {
  // Init OpenGL
  if (!gladLoadGL(glfwGetProcAddress)) {
    throw std::runtime_error("Failed to initialize GLAD\n");
  }

  // Pointer to GLFW window
  auto glfwWindow = EngineApi(voxlight).getGLFWwindow();

  // Set window resize callback
  glViewport(0, 0, WindowWidth, WindowHeight);
  glfwSetWindowUserPointer(glfwWindow, this);
  auto framebufferSizeCallback = [](GLFWwindow *, int width, int height) {
    glViewport(0, 0, width, height);
  };
  glfwSetFramebufferSizeCallback(glfwWindow, framebufferSizeCallback);

  // Set clear color
  glClearColor(0.529f, 0.8f, 0.92f, 0.f);

  // Create shader programs
  voxelProgram =
      createProgram(VOXEL_VERTEX_SHADER_SRC, VOXEL_FRAGMENT_SHADER_SRC);
  voxelUniform.modelMatrix = glGetUniformLocation(voxelProgram, "uModelMatrix");
  voxelUniform.viewProjectionMatrix =
      glGetUniformLocation(voxelProgram, "uViewProjectionMatrix");
  voxelUniform.invResolution =
      glGetUniformLocation(voxelProgram, "uInvResolution");
  voxelUniform.minBox = glGetUniformLocation(voxelProgram, "uMinBox");
  voxelUniform.maxBox = glGetUniformLocation(voxelProgram, "uMaxBox");
  voxelUniform.chunkSize = glGetUniformLocation(voxelProgram, "uChunkSize");
  voxelUniform.magicMatrix = glGetUniformLocation(voxelProgram, "uMagicMatrix");
  voxelUniform.chunkTexture =
      glGetUniformLocation(voxelProgram, "uChunkTexture");
  voxelUniform.paletteTexture =
      glGetUniformLocation(voxelProgram, "uPaletteTexture");

  sunlightProgram =
      createProgram(SUNLIGHT_VERTEX_SHADER_SRC, SUNLIGHT_FRAGMENT_SHADER_SRC);
  sunlightUniform.invResolution =
      glGetUniformLocation(voxelProgram, "uInvResolution");
  sunlightUniform.magicMatrix =
      glGetUniformLocation(voxelProgram, "uMagicMatrix");
  sunlightUniform.sunPos = glGetUniformLocation(voxelProgram, "uSunPos");
  sunlightUniform.worldTexture =
      glGetUniformLocation(voxelProgram, "uWorldTexture");
  sunlightUniform.albedoTexture =
      glGetUniformLocation(voxelProgram, "uAlbedoTexture");
  sunlightUniform.depthTexture =
      glGetUniformLocation(voxelProgram, "uDepthTexture");
  sunlightUniform.normalTexture =
      glGetUniformLocation(voxelProgram, "uNormalTexture");

  // Create vertex buffers
  glGenBuffers(1, &cubeVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexData), cubeVertexData,
               GL_STATIC_DRAW);

  glGenBuffers(1, &quadVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertexData), quadVertexData,
               GL_STATIC_DRAW);

  // Create framebuffers
  glGenFramebuffers(1, &mainFramebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, mainFramebuffer);

  // Do we need this?
  unsigned int rboDepth;
  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WindowWidth,
                        WindowHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rboDepth);

  glGenTextures(1, &colorTexture);
  glBindTexture(GL_TEXTURE_2D, colorTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WindowWidth, WindowHeight, 0, GL_RGBA,
               GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glGenTextures(1, &normalTexture);
  glBindTexture(GL_TEXTURE_2D, normalTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB_SNORM, WindowWidth, WindowHeight, 0,
               GL_RGB, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glGenTextures(1, &depthTexture);
  glBindTexture(GL_TEXTURE_2D, depthTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, WindowWidth, WindowHeight, 0, GL_RGB,
               GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         colorTexture, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         normalTexture, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                         depthTexture, 0);
  frameBufferCheck();

  // Create palette texture
  glGenTextures(1, &paletteTexture);
  glBindTexture(GL_TEXTURE_2D, paletteTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // // load and generate the texture
  // int width, height, nrChannels;
  // unsigned char *data =
  //     stbi_load("./palette.png", &width, &height, &nrChannels, 0);
  // if (data) {
  //   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
  //                GL_UNSIGNED_BYTE, data);
  //   glGenerateMipmap(GL_TEXTURE_2D);
  // } else {
  //   spdlog::error("Failed to load texture");
  // }
  // stbi_image_free(data);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (sizeof(COLOR_PALETTE) / 4), 1, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, COLOR_PALETTE);
  glGenerateMipmap(GL_TEXTURE_2D);
}

void RenderSystem::deinit() {}

void RenderSystem::update(float) {
  glUseProgram(voxelProgram);
  glBindFramebuffer(GL_FRAMEBUFFER, mainFramebuffer);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  entt::registry &registry = EngineApi(voxlight).getRegistry();
  auto view = registry.view<TransformComponent, VoxelComponent>();

  auto viewSorted =
      registry.view<const VoxelComponent, const TransformComponent>();
  viewSorted.use<VoxelComponent>();
  // auto viewProjectionMatrix =
  //     engine->getControllerSystem().getViewProjectionMatrix();

  // temporary
  // glm::vec3 cameraPos = {0,0,-10};
  // glm::vec3 cameraDir = {0,0,1};
  // glm::mat4 viewMat = glm::lookAt(cameraPos, cameraDir, {0,1,0});
  // glm::mat4 projMat = glm::perspective(glm::radians(90.f), 16.0f / 9.0f,
  // 0.1f, 500.0f);

  // auto viewProjectionMatrix = projMat * viewMat;
  auto viewProjectionMatrix =
      CameraComponentApi(voxlight).getViewProjectionMatrix();
  auto invViewProjectionMatrix = glm::inverse(viewProjectionMatrix);
  int p = 0;
  for (auto [entity, voxelComponent, transformComponent] : viewSorted.each()) {
    spdlog::info("Drawing voxel entity {}", p);
    p++;
    spdlog::info("Voxel texture {}", voxelComponent.textureId);
    glm::vec3 size = voxelComponent.voxelData.getDimensions();
    glm::vec3 minBox = transformComponent.position;
    glm::vec3 maxBox = minBox + size;

    auto translateMatrix = glm::translate(glm::mat4(1.f), minBox);
    auto scaleMatrix = glm::scale(glm::mat4(1.f), size);
    auto rotationMatrix = glm::toMat4(transformComponent.rotation);
    auto modelMatrix = translateMatrix * rotationMatrix * scaleMatrix;
    auto mvp = viewProjectionMatrix * modelMatrix;
    auto magicMatrix = translateMatrix * scaleMatrix * glm::inverse(mvp);
    glUniformMatrix4fv(voxelUniform.modelMatrix, 1, GL_FALSE,
                       glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(voxelUniform.viewProjectionMatrix, 1, GL_FALSE,
                       glm::value_ptr(viewProjectionMatrix));
    glUniform2f(voxelUniform.invResolution, 1.f / 1280.f, 1.f / 720.f);
    glUniform3f(voxelUniform.minBox, minBox.x, minBox.y, minBox.z);
    glUniform3f(voxelUniform.maxBox, maxBox.x, maxBox.y, maxBox.z);
    glUniform3f(voxelUniform.chunkSize, size.x, size.y, size.z);
    glUniformMatrix4fv(voxelUniform.magicMatrix, 1, GL_FALSE,
                       glm::value_ptr(magicMatrix));

    glUniform1i(voxelUniform.chunkTexture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, voxelComponent.textureId);

    glUniform1i(voxelUniform.paletteTexture, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, paletteTexture);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }

  // Sunlight stage
  // glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // glUseProgram(sunlightProgram);

  // glActiveTexture(GL_TEXTURE0);
  // glBindTexture(GL_TEXTURE_3D, worldVoxelTexture);

  // glActiveTexture(GL_TEXTURE1);
  // glBindTexture(GL_TEXTURE_2D, colorTexture);

  // glActiveTexture(GL_TEXTURE2);
  // glBindTexture(GL_TEXTURE_2D, depthTexture);

  // glActiveTexture(GL_TEXTURE3);
  // glBindTexture(GL_TEXTURE_2D, normalTexture);

  // glUniform1i(glGetUniformLocation(sunlightProgram, "uWorldTexture"), 0);
  // glUniform1i(glGetUniformLocation(sunlightProgram, "uAlbedoTexture"), 1);
  // glUniform1i(glGetUniformLocation(sunlightProgram, "uDepthTexture"), 2);
  // glUniform1i(glGetUniformLocation(sunlightProgram, "uNormalTexture"), 3);

  // auto mm = glm::scale(glm::mat4(1.f), glm::vec3(8)) * viewProjectionInv;
  // glUniform2f(glGetUniformLocation(sunlightProgram, "invResolution"), 1.f /
  // WINDOW_WIDTH, 1.f / WINDOW_HEIGHT);
  // glUniformMatrix4fv(glGetUniformLocation(sunlightProgram, "uMagicMatrix"),
  // 1, GL_FALSE, &mm[0][0]); glUniform3f(glGetUniformLocation(sunlightProgram,
  // "uSunPos"), sunPosition.x, sunPosition.y, sunPosition.z);

  // glEnableVertexAttribArray(0);
  // glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
  // glVertexAttribPointer(0,  // attribute 0. No particular reason for 0, but
  //                           // must match the layout in the shader.
  //                       3,         // size
  //                       GL_FLOAT,  // type
  //                       GL_FALSE,  // normalized?
  //                       0,         // stride
  //                       (void*)0   // array buffer offset
  // );
  // glDrawArrays(GL_TRIANGLES, 0, 6);  // 3 indices starting at 0 -> 1 triangle

  glBindFramebuffer(GL_READ_FRAMEBUFFER, mainFramebuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, WindowWidth, WindowHeight, 0, 0, WindowWidth,
                    WindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

  glfwSwapBuffers(EngineApi(voxlight).getGLFWwindow());
  glfwPollEvents();
}

unsigned int RenderSystem::createVoxelTexture(VoxelData const &voxelData) {
  GLuint texname;
  glGenTextures(1, &texname);
  glBindTexture(GL_TEXTURE_3D, texname);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  auto size = voxelData.getDimensions();
  glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, size.x, size.y, size.z, 0, GL_RED,
               GL_UNSIGNED_BYTE, voxelData.getData());
  glBindTexture(GL_TEXTURE_3D, 0);
  return texname;
}

void RenderSystem::deleteVoxelTexture(unsigned int textureId) {
  glDeleteTextures(1, &textureId);
}

// void RenderSystem::createWorldTexture(std::vector<std::uint8_t> const &data,
//                                       glm::ivec3 size) {
//   if (0 != worldVoxelTexture) {
//     glDeleteTextures(1, &worldVoxelTexture);
//   }
//   worldVoxelTextureSize = size;
//   worldVoxelTexture = createVoxelTexture(data, size);
// }

// void RenderSystem::updateWorldTexture(std::vector<std::uint8_t> const &,
//                                       glm::ivec3, glm::ivec3) {
//   // todo
// }
