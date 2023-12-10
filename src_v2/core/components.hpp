#pragma once

#include <glm/glm.hpp>

struct TransformComponent {
    glm::vec3 position;
    glm::vec3 scale;
};

struct VoxelComponent {
    unsigned int textureId;
};
