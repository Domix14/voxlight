#include "rendering/render_system.hpp"

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <spdlog/spdlog.h>

#include <entt/entity/registry.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "controller/controller_system.hpp"
#include "core/components.hpp"
#include "core/voxel_data.hpp"
#include "engine_config.hpp"
#include "generated/shaders.hpp"
#include "rendering/palette.hpp"
#include "voxlight.hpp"
#include <rendering/shader.hpp>

#include "api/voxlight_api.hpp"

static GLfloat const cubeVertexData[] = {
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

static GLfloat const quadVertexData[] = {-1.f, -1.f, 0.f, 1.f, 1.f, 0.f, 1.f,  -1.f, 0.f,
                                         -1.f, 1.f,  0.f, 1.f, 1.f, 0.f, -1.f, -1.f, 0.f};

static void frameBufferCheck() {
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if(status != GL_FRAMEBUFFER_COMPLETE) {
    spdlog::error("Framebuffer is not complete");
  }
}

RenderSystem::RenderSystem(Voxlight &voxlight) : System(voxlight) {}

void RenderSystem::init() {
  // Init OpenGL
  if(!gladLoadGL(glfwGetProcAddress)) {
    throw std::runtime_error("Failed to initialize GLAD \n");
  }

  // Pointer to GLFW window
  auto glfwWindow = EngineApi(voxlight).getGLFWwindow();


  // voxelShader.create(VOXEL_VERTEX_SHADER_SRC, VOXEL_FRAGMENT_SHADER_SRC);
  voxelShader.loadAndCreate(VOXEL_VERTEX_SHADER_PATH, VOXEL_FRAGMENT_SHADER_PATH);
  sunlightShader.loadAndCreate(SUNLIGHT_VERTEX_SHADER_PATH, SUNLIGHT_FRAGMENT_SHADER_PATH);

  // Set window resize callback
  glViewport(0, 0, WindowWidth, WindowHeight);
  glfwSetWindowUserPointer(glfwWindow, this);
  auto framebufferSizeCallback = [](GLFWwindow *, int width, int height) { glViewport(0, 0, width, height); };
  glfwSetFramebufferSizeCallback(glfwWindow, framebufferSizeCallback);

  // Set clear color
  glClearColor(0.529f, 0.8f, 0.92f, 0.f);

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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (sizeof(COLOR_PALETTE) / 4), 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, COLOR_PALETTE);
  glGenerateMipmap(GL_TEXTURE_2D);

  voxelWorld.init(glm::ivec3(128));


  VoxelComponentApi(voxlight).subscribe(VoxelComponentEventType::OnVoxelDataCreation, std::bind(&RenderSystem::onVoxelDataCreation, this, std::placeholders::_1, std::placeholders::_2));
  VoxelComponentApi(voxlight).subscribe(VoxelComponentEventType::OnVoxelDataDestruction, std::bind(&RenderSystem::onVoxelDataDestruction, this, std::placeholders::_1, std::placeholders::_2));
  VoxelComponentApi(voxlight).subscribe(VoxelComponentEventType::OnVoxelDataChange, std::bind(&RenderSystem::onVoxelDataModification, this, std::placeholders::_1, std::placeholders::_2));
  EntityApi(voxlight).subscribe(EntityEventType::OnTransformChange, std::bind(&RenderSystem::onEntityTransformChange, this, std::placeholders::_1, std::placeholders::_2));
}

void RenderSystem::deinit() {}

void RenderSystem::update(float) {
  glBindFramebuffer(GL_FRAMEBUFFER, mainFramebuffer);
  GLenum attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
  glDrawBuffers(3, attachments);
  glClear(GL_COLOR_BUFFER_BIT);

  float depth = 1.0f;
  glClearTexImage(depthTexture, 0, GL_RGB, GL_FLOAT, &depth);

  entt::registry &registry = EngineApi(voxlight).getRegistry();
  auto view = registry.view<TransformComponent, VoxelComponent>();

  auto camera = CameraComponentApi(voxlight).getCurrentCamera();
  auto cameraPos = EntityApi(voxlight).getTransform(camera).position;
  for(auto [entity, transformComponent, voxelComponent] : view.each()) {
    if(voxelComponent.needsUpdate) {
      voxelWorld.rasterizeVoxelData(voxelComponent.lastPosition, voxelComponent.lastRotation, voxelComponent.voxelData, true);
      voxelComponent.needsUpdate = false;
      voxelComponent.lastPosition = transformComponent.position;
      voxelComponent.lastRotation = transformComponent.rotation;
      voxelWorld.rasterizeVoxelData(voxelComponent.lastPosition, voxelComponent.lastRotation, voxelComponent.voxelData, false);
    }

    glm::vec3 size = voxelComponent.voxelData.getDimensions();
    glm::vec3 minBox = transformComponent.position;
    glm::vec3 maxBox = minBox + size;

    auto center = (minBox + maxBox) / 2.f;
    glm::mat4 transformMatrix = glm::translate(glm::mat4(1.f), center) * glm::toMat4(transformComponent.rotation);
    // Calculate the inverse of the rotation matrix applied to the AABB box
    glm::mat4 inverseTransformation = glm::inverse(transformMatrix);
    
    glm::vec3 cameraLocalPos = glm::vec3(inverseTransformation * glm::vec4(cameraPos, 1.0f));

    // Calculate the closest point to the camera within the AABB box
    auto halfSize = size / 2.f;
    glm::vec3 closestPoint = glm::vec3(glm::clamp(cameraLocalPos.x, -halfSize.x, halfSize.x),
                                      glm::clamp(cameraLocalPos.y, -halfSize.y, halfSize.y),
                                      glm::clamp(cameraLocalPos.z, -halfSize.z, halfSize.z));

    voxelComponent.distance = glm::distance(cameraLocalPos, closestPoint);
  }
  voxelWorld.sync();

  registry.sort<VoxelComponent>([](auto const &a, auto const &b) { return a.distance < b.distance; });

  auto viewSorted = registry.view<VoxelComponent const, TransformComponent const>();
  viewSorted.use<VoxelComponent>();

  auto viewProjectionMatrix = CameraComponentApi(voxlight).getViewProjectionMatrix();
  auto invViewProjectionMatrix = glm::inverse(viewProjectionMatrix);

  voxelShader.use();
  for(auto [entity, voxelComponent, transformComponent] : viewSorted.each()) {
    glm::vec3 size = voxelComponent.voxelData.getDimensions();
    glm::vec3 minBox = transformComponent.position;
    glm::vec3 maxBox = minBox + size;

    auto translateMatrix = glm::translate(glm::mat4(1.f), minBox);
    auto scaleMatrix = glm::scale(glm::mat4(1.f), size);
    auto rotationMatrix = glm::toMat4(transformComponent.rotation);
    auto modelMatrix = translateMatrix * rotationMatrix * scaleMatrix;
    auto mvp = viewProjectionMatrix * modelMatrix;
    auto magicMatrix = glm::inverse(viewProjectionMatrix * translateMatrix * rotationMatrix);

    voxelShader.setMat4("uModelMatrix", glm::value_ptr(modelMatrix));
    voxelShader.setMat4("uViewProjectionMatrix", glm::value_ptr(viewProjectionMatrix));
    voxelShader.setVec2("uInvResolution", 1.f / 1280.f, 1.f / 720.f);
    voxelShader.setVec3("uMinBox", minBox.x, minBox.y, minBox.z);
    voxelShader.setVec3("uMaxBox", maxBox.x, maxBox.y, maxBox.z);
    voxelShader.setVec3("uChunkSize", size.x, size.y, size.z);
    voxelShader.setMat4("uMagicMatrix", glm::value_ptr(magicMatrix));

    glm::mat4 modelMatrix2 = translateMatrix * rotationMatrix;
    voxelShader.setMat4("uModelMatrix2", glm::value_ptr(modelMatrix2));

    // glm::mat4 magicMatrix2 =  modelMatrix * glm::inverse(translateMatrix * scaleMatrix);
    // voxelShader.setMat4("uMagicMatrix2", glm::value_ptr(magicMatrix2));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, voxelComponent.textureId);
    voxelShader.setInt("uChunkTexture", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, paletteTexture);
    voxelShader.setInt("uPaletteTexture", 1);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glTextureBarrier();
  }

  // Sunlight stage
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  sunlightShader.use();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, voxelWorld.getTexture());

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, colorTexture);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, depthTexture);

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, normalTexture);

  sunlightShader.setInt("uWorldTexture", 0);
  sunlightShader.setInt("uAlbedoTexture", 1);
  sunlightShader.setInt("uDepthTexture", 2);
  sunlightShader.setInt("uNormalTexture", 3);

  glm::vec3 sunPosition = {100000.f, 100000.f, 100000.f};
  auto mm = invViewProjectionMatrix;
  sunlightShader.setVec2("uInvResolution", 1.f / 1280.f, 1.f / 720.f);
  sunlightShader.setMat4("uMagicMatrix", glm::value_ptr(mm));
  sunlightShader.setVec3("uSunPos", sunPosition.x, sunPosition.y, sunPosition.z);
  glm::vec3 worldDimensions = glm::vec3(voxelWorld.getDimensions());
  sunlightShader.setVec3("uWorldDimensions", worldDimensions.x, worldDimensions.y, worldDimensions.z);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
  glVertexAttribPointer(0,  // attribute 0. No particular reason for 0, but
                            // must match the layout in the shader.
                        3,         // size
                        GL_FLOAT,  // type
                        GL_FALSE,  // normalized?
                        0,         // stride
                        (void*)0   // array buffer offset
  );
  glDrawArrays(GL_TRIANGLES, 0, 6);  // 3 indices starting at 0 -> 1 triangle

  // glBindFramebuffer(GL_READ_FRAMEBUFFER, mainFramebuffer);
  // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  // glBlitFramebuffer(0, 0, WindowWidth, WindowHeight, 0, 0, WindowWidth, WindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
  // glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

  glfwSwapBuffers(EngineApi(voxlight).getGLFWwindow());
  glfwPollEvents();

  voxelShader.refresh();
  sunlightShader.refresh();
}

void RenderSystem::onVoxelDataCreation(VoxelComponentEventType, VoxelComponentEvent const &event) {
  auto texId = CreateVoxelTexture(event.newVoxelData.getData(), event.newVoxelData.getDimensions());
  EngineApi(voxlight).getRegistry().get<VoxelComponent>(event.entity).textureId = texId;
  auto transformComponent = EntityApi(voxlight).getTransform(event.entity);
  voxelWorld.rasterizeVoxelData(transformComponent.position, transformComponent.rotation, event.newVoxelData, false);
}

void RenderSystem::onVoxelDataDestruction(VoxelComponentEventType, VoxelComponentEvent const &event) {
  DeleteVoxelTexture(event.voxelComponent.textureId);
  auto transformComponent = EntityApi(voxlight).getTransform(event.entity);
  voxelWorld.rasterizeVoxelData(transformComponent.position, transformComponent.rotation, event.voxelComponent.voxelData, true);
}

void RenderSystem::onVoxelDataModification(VoxelComponentEventType, VoxelComponentEvent const &event) {
  auto transformComponent = EntityApi(voxlight).getTransform(event.entity);
  voxelWorld.rasterizeVoxelData(transformComponent.position, transformComponent.rotation, event.voxelComponent.voxelData, true);
  voxelWorld.rasterizeVoxelData(transformComponent.position, transformComponent.rotation, event.newVoxelData, false);
  DeleteVoxelTexture(event.voxelComponent.textureId);
  auto texId = CreateVoxelTexture(event.newVoxelData.getData(), event.newVoxelData.getDimensions());
  EngineApi(voxlight).getRegistry().get<VoxelComponent>(event.entity).textureId = texId;
}

void RenderSystem::onEntityTransformChange(EntityEventType, EntityEvent const &event) {
  auto voxelComponent = EngineApi(voxlight).getRegistry().try_get<VoxelComponent>(event.entity);
  if(voxelComponent) {
    voxelWorld.rasterizeVoxelData(event.oldTransform.position, event.oldTransform.rotation, voxelComponent->voxelData, true);
    voxelWorld.rasterizeVoxelData(event.transformComponent.position, event.transformComponent.rotation, voxelComponent->voxelData, false);
  }
}
