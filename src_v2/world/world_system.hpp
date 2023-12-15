#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "core/voxel_data.hpp"
#include "glm/glm.hpp"
#include "system.hpp"

class WorldSystem : public System {
   public:
    WorldSystem(VoxelEngine* engine);

    void init();
    void update(float deltaTime);
    void deinit();

    int createVoxelEntity(VoxelData<std::uint8_t> const& data, glm::vec3 position,
                          glm::quat rotation = {1.f, 0.f, 0.f, 0.f});
};