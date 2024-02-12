#include <spdlog/spdlog.h>

#include <api/voxlight_api.hpp>
#include <voxlight.hpp>

class TestSystem : public System {
   public:
    TestSystem(Voxlight& voxlight) : System(voxlight) {}

    void init() override {
        spdlog::info("TestSystem::init()");
        window = EngineApi(voxlight).getGLFWwindow();

        auto entity = EntityApi(voxlight).createEntity("TestEntity", TransformComponent());

        spdlog::info("1");
        VoxelData voxelData;
        voxelData.resize({8, 8, 8});
        for (int i = 0; i < 64; i++) {
            voxelData.setVoxel({i, 0, 0}, 50);
        }
        spdlog::info("2");

        VoxelComponentApi(voxlight).addComponent(entity, voxelData);
        // EntityApi(engine).addComponent<MyCustomComponent>();
        spdlog::info("3");
    }

    void update(float deltaTime) override {
        spdlog::info("TestSystem::update()");
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
        float horizontalSpeed = 5.f * deltaTime;
        int state = glfwGetKey(window, GLFW_KEY_W);
        glm::vec3 horizontalDir = glm::normalize(glm::vec3(direction.x, 0, direction.z));
        if (state == GLFW_PRESS) {
            position += horizontalDir * horizontalSpeed;
        }
        state = glfwGetKey(window, GLFW_KEY_S);
        if (state == GLFW_PRESS) {
            position -= horizontalDir * horizontalSpeed;
        }
        state = glfwGetKey(window, GLFW_KEY_A);
        if (state == GLFW_PRESS) {
            position -= camRight * horizontalSpeed;
        }
        state = glfwGetKey(window, GLFW_KEY_D);
        if (state == GLFW_PRESS) {
            position += camRight * horizontalSpeed;
        }

        float verticalSpeed = 5.f * deltaTime;
        state = glfwGetKey(window, GLFW_KEY_SPACE);
        if (state == GLFW_PRESS) {
            position += camUp * verticalSpeed;
        }
        state = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
        if (state == GLFW_PRESS) {
            position -= camUp * verticalSpeed;
        }

        auto camera = CameraComponentApi(voxlight).getCurrentCamera();
        CameraComponentApi(voxlight).setProjectionMatrix(
            camera, glm::perspective(glm::radians(90.f), 16.0f / 9.0f, 0.1f, 500.0f));
        CameraComponentApi(voxlight).setDirection(camera, direction);
        EntityApi(voxlight).setPosition(camera, position);
    }

    void deinit() override { spdlog::info("TestSystem::deinit()"); }

   private:
    GLFWwindow* window;
    double cursorX = 0;
    double cursorY = 0;
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::vec3 direction = glm::vec3(0, 0, -1);
    glm::vec3 camRight = glm::vec3(1, 0, 0);
    glm::vec3 camUp = glm::vec3(0, 1, 0);
};