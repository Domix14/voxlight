#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <utils.hpp>
#include <vector>

constexpr std::uint32_t WINDOW_WIDTH = 1280;
constexpr std::uint32_t WINDOW_HEIGHT = 720;

struct TransformComponent {
    glm::vec3 position;
    glm::vec3 scale;
};

struct VoxelComponent {
    GLuint voxelTexture;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 size;
    float voxelSize;

    std::vector<std::uint8_t> voxelData;

    VoxelComponent()
        : voxelTexture(NO_TEXTURE), position(0.f), rotation(glm::vec3(0.f)), size(0.f), voxelSize(VOXEL_SIZE_100CM) {}

    void setVoxelData(std::vector<std::uint8_t> const& data) {
        assert(data.size() == (size.x * size.y * size.z));
        voxelData = data;
        if (voxelTexture != NO_TEXTURE) {
            glDeleteTextures(1, &voxelTexture);
            voxelTexture = NO_TEXTURE;
        }
        voxelTexture = createVoxelTexture(voxelData, size);
        voxelSize = voxelSize;
    }
};

enum class ComponentType : std::uint32_t { TRANSFORM, VOXEL };

class Engine {
   public:
    Engine();
    void run();

    std::uint32_t entityCount;
    std::vector<TransformComponent> transformComponents;
    std::vector<VoxelComponent> voxelComponents;

    class VoxelSystem* voxelSystem;

    std::uint32_t createEntity();
    std::uint32_t createVoxelEntity(glm::vec3 pos, glm::vec3 rot, glm::vec3 size, float voxSize,
                                    std::vector<std::uint8_t> const& voxelData);
};

// class VoxelSystem {
//     std::vector<std::uint32_t> entities;
//     Engine* engine;

// public:
//     VoxelSystem(class Engine* engine) : engine(engine) {}
//     void update(float deltaTime, GLuint programID) {
//         for (auto entity : entities) {
//             auto minBox = engine->voxelComponents[entity].minBox;
//             auto maxBox = engine->voxelComponents[entity].maxBox;
//             auto size = engine->voxelComponents[entity].chunkSize;
//             glm::mat4 model = glm::translate(glm::mat4(1.0f), minBox);
//             model = glm::scale(model, size);

//             glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, glm::value_ptr(model));
//             glUniform3f(glGetUniformLocation(programID, "minBox"), minBox.x, minBox.y, minBox.z);
//             glUniform3f(glGetUniformLocation(programID, "maxBox"), maxBox.x, maxBox.y, maxBox.z);
//             glUniform3f(glGetUniformLocation(programID, "chunkSize"), size.x, size.y, size.z);

//             glBindTexture(GL_TEXTURE_3D, engine->voxelComponents[entity].voxelTexture);
//             glUniform1i(glGetUniformLocation(programID, "chunkTexture"), 0);
//             glDrawArrays(GL_TRIANGLES, 0, 36);
//         }
//     }

//     void addEntity(std::uint32_t entity) {
//         entities.push_back(entity);
//     }
// };

#endif  // ENGINE_HPP