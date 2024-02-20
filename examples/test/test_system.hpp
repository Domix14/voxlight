#include <spdlog/spdlog.h>

#include <api/voxlight_api.hpp>
#include <core/components.hpp>
#include <glm/gtc/noise.hpp>
#include <voxlight.hpp>
#include <glm/gtc/random.hpp>
#include <cstdint>

class TestSystem : public System {
public:
  TestSystem(Voxlight &voxlight) : System(voxlight) {}

  void generateChunk(glm::vec3 pos, glm::vec3 size) {
    auto entity = EntityApi(voxlight).createEntity("TestEntity", TransformComponent());
    EntityApi(voxlight).setPosition(entity, pos);

    // glm::vec3 rotation = glm::sphericalRand(1.f);
    // EntityApi(voxlight).setRotation(entity, glm::quat(rotation));

    VoxelData voxelData;
    voxelData.resize(size);
    static std::uint8_t p = 2;

    // generate chunk with random heioghts
    for(size_t x = 0; x < size.x; ++x) {
      for(size_t z = 0; z < size.z; ++z) {
        glm::vec2 pos2d = glm::vec2(x + pos.x, z + pos.z) / 30.f;
        float perlin = glm::clamp((glm::perlin(pos2d) + 1.f) / 2.f, 0.f, 1.f);
        // spdlog::info("perlin: {}", perlin);
        size_t height = size.y * perlin;
        for(size_t y = 0; y < height; ++y) {
          voxelData.setVoxel({x, y, z}, p);
        }
      }
    }

    p += 5;
    VoxelComponentApi(voxlight).addComponent(entity, voxelData);
  }

  void init() override {
    spdlog::info("TestSystem::init()");
    window = EngineApi(voxlight).getGLFWwindow();

    VoxelComponentApi(voxlight).subscribe(VoxelComponentEventType::AfterVoxelDataChange, [](VoxelComponentEventType eventType, VoxelComponentEvent const& event) {
      spdlog::info("VoxelComponentEventType::AfterVoxelDataChange 1");
    });

    // VoxelComponentApi(voxlight).listenForEvent(VoxelComponentEventType::AfterVoxelDataChange, [](VoxelComponentEvent const& event) {
    //   spdlog::info("VoxelComponentEventType::AfterVoxelDataChange 1");
    // });


    // auto entity =
    //     EntityApi(voxlight).createEntity("TestEntity", TransformComponent());

    // spdlog::info("1");
    // VoxelData voxelData;
    // voxelData.resize({8, 8, 8});
    // for (int i = 0; i < 64; i++) {
    //   voxelData.setVoxel({i, 0, 0}, 50);
    // }
    // spdlog::info("2");

    // VoxelComponentApi(voxlight).addComponent(entity, voxelData);

    // size_t size = 16;
    // for(size_t x = 0; x < 2; ++x) {
    //   for(size_t y = 0; y < 2; ++y) {
    //     generateChunk({x * size, 0, y * size}, {size, size, size});
    //   }
    // }

    // Create plane entity
    TransformComponent transform;
    transform.position = {0, 0, 0};
    transform.rotation = glm::quat(glm::vec3(0, 0, 0));
    auto planeEntity = EntityApi(voxlight).createEntity("Plane", transform);
    VoxelData planeVoxelData;
    planeVoxelData.resize({128, 1, 128});
    for(size_t x = 0; x < 128; ++x) {
      for(size_t z = 0; z < 128; ++z) {
        planeVoxelData.setVoxel({x, 0, z}, 2);
      }
    }
    VoxelComponentApi(voxlight).addComponent(planeEntity, planeVoxelData);

    // create cube on plane


    auto cubeEntity = EntityApi(voxlight).createEntity("Cube", TransformComponent());
    VoxelData cubeVoxelData;
    cubeVoxelData.resize({16, 16, 16});  
    for(size_t x = 0; x < 16; ++x) {
      for(size_t y = 0; y < 16; ++y) {
        for(size_t z = 0; z < 16; ++z) {
          cubeVoxelData.setVoxel({x, y, z}, 70);
        }
      }
    }
    EntityApi(voxlight).setPosition(cubeEntity, {60, 5, 60});
    VoxelComponentApi(voxlight).addComponent(cubeEntity, cubeVoxelData);

    // create sphere
    auto sphereEntity = EntityApi(voxlight).createEntity("Sphere", TransformComponent());
    VoxelData sphereVoxelData;
    sphereVoxelData.resize({16, 16, 16});
    for(size_t x = 0; x < 16; ++x) {
      for(size_t y = 0; y < 16; ++y) {
        for(size_t z = 0; z < 16; ++z) {
          glm::vec3 pos = glm::vec3(x, y, z) - glm::vec3(8, 8, 8);
          if(glm::length(pos) < 8) {
            sphereVoxelData.setVoxel({x, y, z}, 70);
          }
        }
      }
    }
    EntityApi(voxlight).setPosition(sphereEntity, {96, 8, 96});
    VoxelComponentApi(voxlight).addComponent(sphereEntity, sphereVoxelData);


    // spawn another cube
    auto cubeEntity2 = EntityApi(voxlight).createEntity("Cube2", TransformComponent());
    VoxelData cubeVoxelData2;
    cubeVoxelData2.resize({16, 16, 16});
    for(size_t x = 0; x < 16; ++x) {
      for(size_t y = 0; y < 16; ++y) {
        for(size_t z = 0; z < 16; ++z) {
          cubeVoxelData2.setVoxel({x, y, z}, 70);
        }
      }
    }
    EntityApi(voxlight).setPosition(cubeEntity2, {30, 5, 30});
    // VoxelComponentApi(voxlight).addComponent(cubeEntity2, cubeVoxelData2);
  }

  void update(float deltaTime) override {
    double xpos = 0;
    double ypos = 0;

    glfwGetCursorPos(window, &xpos, &ypos);
    double dx = xpos - cursorX;
    double dy = ypos - cursorY;
    cursorX = xpos;
    cursorY = ypos;
    camRight = glm::normalize(glm::cross(direction, camUp));
    //  camUp = glm::normalize(glm::cross(camRight, direction));

    glm::mat4 rotMat(1.0f);
    rotMat = glm::rotate(rotMat, (float)-dx / 100.f, camUp);
    rotMat = glm::rotate(rotMat, (float)-dy / 100.0f, camRight);
    direction = glm::normalize(rotMat * glm::vec4(direction, 0.0f));

    // Movement
    float horizontalSpeed = 15.f * deltaTime;
    int state = glfwGetKey(window, GLFW_KEY_W);
    glm::vec3 horizontalDir = glm::normalize(glm::vec3(direction.x, 0, direction.z));
    if(state == GLFW_PRESS) {
      position += horizontalDir * horizontalSpeed;
    }
    state = glfwGetKey(window, GLFW_KEY_S);
    if(state == GLFW_PRESS) {
      position -= horizontalDir * horizontalSpeed;
    }
    state = glfwGetKey(window, GLFW_KEY_A);
    if(state == GLFW_PRESS) {
      position -= camRight * horizontalSpeed;
    }
    state = glfwGetKey(window, GLFW_KEY_D);
    if(state == GLFW_PRESS) {
      position += camRight * horizontalSpeed;
    }

    float verticalSpeed = 15.f * deltaTime;
    state = glfwGetKey(window, GLFW_KEY_SPACE);
    if(state == GLFW_PRESS) {
      position += camUp * verticalSpeed;
    }
    state = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
    if(state == GLFW_PRESS) {
      position -= camUp * verticalSpeed;
    }

    auto camera = CameraComponentApi(voxlight).getCurrentCamera();
    CameraComponentApi(voxlight).setProjectionMatrix(camera,
                                                     glm::perspective(glm::radians(90.f), 16.0f / 9.0f, 0.1f, 500.0f));
    
    if(glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
      CameraComponentApi(voxlight).setDirection(camera, direction);
      EntityApi(voxlight).setPosition(camera, position);
    }

    auto cubeEntity = EntityApi(voxlight).getFirstWithName("Cube");
    // move cube from left to right
    glm::vec3 cubePos = EntityApi(voxlight).getTransform(cubeEntity).position;
    cubePos.x += 5.f * deltaTime;
    if(cubePos.x > 80) {
      cubePos.x = 10;
    }
    EntityApi(voxlight).setPosition(cubeEntity, cubePos);

    // rotate cube in 2 axes
    // glm::quat cubeRot = EntityApi(voxlight).getTransform(cubeEntity).rotation;
    // cubeRot = glm::rotate(cubeRot, 1.f, glm::vec3(1, 0, 0));
    // cubeRot = glm::rotate(cubeRot, .5f, glm::vec3(0, 1, 0));


    // genrate random rotation quat
    auto rot = EntityApi(voxlight).getTransform(cubeEntity).rotation;
    static float rotation = 0;

    // rotate in y axis

    EntityApi(voxlight).setRotation(cubeEntity, glm::angleAxis(rotation, glm::vec3(0.f, 1.f, 0.f)));
    rotation += 0.1f * deltaTime;


    
  }

  void deinit() override { spdlog::info("TestSystem::deinit()"); }

private:
  GLFWwindow *window;
  double cursorX = 0;
  double cursorY = 0;
  glm::vec3 position = glm::vec3(0, 0, 0);
  glm::vec3 direction = glm::vec3(0, 0, -1);
  glm::vec3 camRight = glm::vec3(1, 0, 0);
  glm::vec3 camUp = glm::vec3(0, 1, 0);
};
