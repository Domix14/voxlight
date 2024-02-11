#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <core/voxel_data.hpp>

struct NameComponent {
    std::string name;
};

struct TransformComponent {
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation;
};

struct VoxelComponent {
    unsigned int textureId;
    bool needsUpdate;
    float distance;
    VoxelData voxelData;
};

