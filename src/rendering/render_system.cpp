#include <rendering/render_system.hpp>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <spdlog/spdlog.h>

#include <core/components.hpp>
#include <core/voxel_data.hpp>
#include <core/voxlight.hpp>
#include <entt/entity/registry.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <rendering/generated/shaders.hpp>
#include <rendering/palette.hpp>
#include <rendering/render_data.hpp>
#include <rendering/shader.hpp>
#include <voxlight_api.hpp>

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

  // voxelShader.create(VOXEL_VERTEX_SHADER_SRC, VOXEL_FRAGMENT_SHADER_SRC);
  voxelShader.loadAndCreate(VOXEL_VERTEX_SHADER_PATH, VOXEL_FRAGMENT_SHADER_PATH);
  sunlightShader.loadAndCreate(SUNLIGHT_VERTEX_SHADER_PATH, SUNLIGHT_FRAGMENT_SHADER_PATH);

  renderResolutionX = 1280;
  renderResolutionY = 720;

  glViewport(0, 0, renderResolutionX, renderResolutionY);

  // Set clear color
  glClearColor(0.529f, 0.8f, 0.92f, 0.f);

  // Create vertex buffers
  glGenBuffers(1, &cubeVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexData), cubeVertexData, GL_STATIC_DRAW);

  glGenBuffers(1, &quadVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertexData), quadVertexData, GL_STATIC_DRAW);

  // Create framebuffer
  createGBuffer();

  // Create palette texture
  glGenTextures(1, &paletteTexture);
  glBindTexture(GL_TEXTURE_2D, paletteTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (sizeof(COLOR_PALETTE) / 4), 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, COLOR_PALETTE);
  glGenerateMipmap(GL_TEXTURE_2D);

  voxelWorld.init(glm::ivec3(512, 128, 512));
  initImgui();

  VoxelComponentApi(voxlight).subscribe(
      VoxelComponentEventType::OnVoxelDataCreation,
      std::bind(&RenderSystem::onVoxelDataCreation, this, std::placeholders::_1, std::placeholders::_2));
  VoxelComponentApi(voxlight).subscribe(
      VoxelComponentEventType::OnVoxelDataDestruction,
      std::bind(&RenderSystem::onVoxelDataDestruction, this, std::placeholders::_1, std::placeholders::_2));
  VoxelComponentApi(voxlight).subscribe(
      VoxelComponentEventType::OnVoxelDataChange,
      std::bind(&RenderSystem::onVoxelDataModification, this, std::placeholders::_1, std::placeholders::_2));
  EntityApi(voxlight).subscribe(
      EntityEventType::OnTransformChange,
      std::bind(&RenderSystem::onEntityTransformChange, this, std::placeholders::_1, std::placeholders::_2));

  EngineApi(voxlight).subscribe(
      EngineEventType::OnWindowResize,
      std::bind(&RenderSystem::onWindowResize, this, std::placeholders::_1, std::placeholders::_2));
}

void RenderSystem::deinit() {}

void RenderSystem::update(float deltaTime) {
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
      voxelWorld.rasterizeVoxelData(voxelComponent.lastPosition, voxelComponent.lastRotation, voxelComponent.voxelData,
                                    true);
      voxelComponent.needsUpdate = false;
      voxelComponent.lastPosition = transformComponent.position;
      voxelComponent.lastRotation = transformComponent.rotation;
      voxelWorld.rasterizeVoxelData(voxelComponent.lastPosition, voxelComponent.lastRotation, voxelComponent.voxelData,
                                    false);
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
    auto invWorldMatrix = glm::inverse(viewProjectionMatrix * translateMatrix * rotationMatrix);

    voxelShader.setMat4("uModelMatrix", glm::value_ptr(modelMatrix));
    voxelShader.setMat4("uViewProjectionMatrix", glm::value_ptr(viewProjectionMatrix));
    voxelShader.setVec2("uInvResolution", 1.f / renderResolutionX, 1.f / renderResolutionY);
    voxelShader.setVec3("uMinBox", minBox.x, minBox.y, minBox.z);
    voxelShader.setVec3("uMaxBox", maxBox.x, maxBox.y, maxBox.z);
    voxelShader.setVec3("uChunkSize", size.x, size.y, size.z);
    voxelShader.setMat4("uInvWorldMatrix", glm::value_ptr(invWorldMatrix));

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

  glm::vec3 sunPosition = {100000.f, 300000.f, 100000.f};
  sunlightShader.setVec2("uInvResolution", 1.f / renderResolutionX, 1.f / renderResolutionY);
  sunlightShader.setMat4("uInvViewProjMatrix", glm::value_ptr(invViewProjectionMatrix));
  sunlightShader.setVec3("uSunPos", sunPosition.x, sunPosition.y, sunPosition.z);
  glm::vec3 worldDimensions = glm::vec3(voxelWorld.getDimensions());
  sunlightShader.setVec3("uWorldDimensions", worldDimensions.x, worldDimensions.y, worldDimensions.z);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
  glVertexAttribPointer(0,         // attribute 0. No particular reason for 0, but
                                   // must match the layout in the shader.
                        3,         // size
                        GL_FLOAT,  // type
                        GL_FALSE,  // normalized?
                        0,         // stride
                        (void *)0  // array buffer offset
  );
  glDrawArrays(GL_TRIANGLES, 0, 6);  // 3 indices starting at 0 -> 1 triangle

  // glBindFramebuffer(GL_READ_FRAMEBUFFER, mainFramebuffer);
  // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  // glBlitFramebuffer(0, 0, renderResolutionX, renderResolutionY, 0, 0, renderResolutionX, renderResolutionY,
  // GL_COLOR_BUFFER_BIT, GL_LINEAR); glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

  drawImgui(deltaTime);

  glfwSwapBuffers(EngineApi(voxlight).getGLFWwindow());
  glfwPollEvents();

  voxelShader.refresh();
  sunlightShader.refresh();
}

void RenderSystem::onVoxelDataCreation(VoxelComponentEventType, VoxelComponentEvent event) {
  auto voxelEvent = event.get<VoxelComponentChangeEvent>();
  auto texId = CreateVoxelTexture(voxelEvent.newVoxelData.getData(), voxelEvent.newVoxelData.getDimensions());
  EngineApi(voxlight).getRegistry().get<VoxelComponent>(voxelEvent.entity).textureId = texId;
  auto transformComponent = EntityApi(voxlight).getTransform(voxelEvent.entity);
  voxelWorld.rasterizeVoxelData(transformComponent.position, transformComponent.rotation, voxelEvent.newVoxelData,
                                false);
}

void RenderSystem::onVoxelDataDestruction(VoxelComponentEventType, VoxelComponentEvent event) {
  auto voxelEvent = event.get<VoxelComponentChangeEvent>();
  DeleteVoxelTexture(voxelEvent.voxelComponent.textureId);
  auto transformComponent = EntityApi(voxlight).getTransform(voxelEvent.entity);
  voxelWorld.rasterizeVoxelData(transformComponent.position, transformComponent.rotation,
                                voxelEvent.voxelComponent.voxelData, true);
}

void RenderSystem::onVoxelDataModification(VoxelComponentEventType, VoxelComponentEvent event) {
  auto voxelEvent = event.get<VoxelComponentChangeEvent>();
  auto transformComponent = EntityApi(voxlight).getTransform(voxelEvent.entity);
  voxelWorld.rasterizeVoxelData(transformComponent.position, transformComponent.rotation,
                                voxelEvent.voxelComponent.voxelData, true);
  voxelWorld.rasterizeVoxelData(transformComponent.position, transformComponent.rotation, voxelEvent.newVoxelData,
                                false);
  DeleteVoxelTexture(voxelEvent.voxelComponent.textureId);
  auto texId = CreateVoxelTexture(voxelEvent.newVoxelData.getData(), voxelEvent.newVoxelData.getDimensions());
  EngineApi(voxlight).getRegistry().get<VoxelComponent>(voxelEvent.entity).textureId = texId;
}

void RenderSystem::onEntityTransformChange(EntityEventType, EntityEvent event) {
  auto entityEvent = event.get<EntityTransformEvent>();
  auto voxelComponent = EngineApi(voxlight).getRegistry().try_get<VoxelComponent>(entityEvent.entity);
  if(voxelComponent) {
    voxelWorld.rasterizeVoxelData(entityEvent.oldTransform.position, entityEvent.oldTransform.rotation,
                                  voxelComponent->voxelData, true);
    voxelWorld.rasterizeVoxelData(entityEvent.transformComponent.position, entityEvent.transformComponent.rotation,
                                  voxelComponent->voxelData, false);
  }
}

void RenderSystem::createGBuffer() {
  glGenFramebuffers(1, &mainFramebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, mainFramebuffer);

  glGenTextures(1, &colorTexture);
  glBindTexture(GL_TEXTURE_2D, colorTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(renderResolutionX),
               static_cast<GLsizei>(renderResolutionY), 0, GL_RGBA, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glGenTextures(1, &normalTexture);
  glBindTexture(GL_TEXTURE_2D, normalTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB_SNORM, static_cast<GLsizei>(renderResolutionX),
               static_cast<GLsizei>(renderResolutionY), 0, GL_RGB, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glGenTextures(1, &depthTexture);
  glBindTexture(GL_TEXTURE_2D, depthTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, static_cast<GLsizei>(renderResolutionX),
               static_cast<GLsizei>(renderResolutionY), 0, GL_RGB, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, depthTexture, 0);
  frameBufferCheck();
}

void RenderSystem::onWindowResize(EngineEventType, EngineEvent event) {
  auto resizeEvent = event.get<WindowResizeEvent>();
  renderResolutionX = resizeEvent.windowWidth;
  renderResolutionY = resizeEvent.windowHeight;

  glViewport(0, 0, renderResolutionX, renderResolutionY);

  // delete old framebuffer
  glDeleteTextures(1, &colorTexture);
  glDeleteTextures(1, &normalTexture);
  glDeleteTextures(1, &depthTexture);

  glDeleteFramebuffers(1, &mainFramebuffer);

  createGBuffer();
}

void RenderSystem::initImgui() {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(EngineApi(voxlight).getGLFWwindow(), true);
  ImGui_ImplOpenGL3_Init("#version 460");
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
}

void RenderSystem::drawImgui(float deltaTime) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("FPS counter");
  ImGui::SetWindowSize(ImVec2(180, 60), ImGuiCond_FirstUseEver);
  ImGui::Text("FPS: %.2f", 1.f / deltaTime);
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
