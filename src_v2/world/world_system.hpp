#pragma once
#include "core/voxel_data.hpp"
#include "glm/glm.hpp"
#include "system.hpp"

class WorldSystem : public System {
   public:
    WorldSystem(VoxelEngine* engine);

    void init();
    void update(float deltaTime);
    void deinit();

    int createVoxelObject(VoxelData<std::uint8_t> const& data, glm::vec3 position);
};