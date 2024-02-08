#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "core/voxel_data.hpp"
#include "glm/glm.hpp"
#include "system.hpp"

class WorldSystem : public ISystem {
public:
  WorldSystem() = default;

  void init(Voxlight *voxlight);
  void update(float deltaTime);
  void deinit();

  std::uint32_t createVoxelEntity(VoxelData const &data, glm::vec3 position,
                                  glm::quat rotation = {1.f, 0.f, 0.f, 0.f});
  void setVoxel(glm::ivec3 position);
  void clearVoxel(glm::ivec3 position);

private:
  std::vector<std::uint8_t> voxelMap;

  Voxlight *engine;
};