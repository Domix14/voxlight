#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

struct TransformComponent {
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation;
};

struct VoxelComponent {
    unsigned int textureId;
};
