#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <rendering/render_system.hpp>

class VoxelWorld {
public:
  VoxelWorld() = default;
  ~VoxelWorld() = default;

  void init(glm::ivec3 dim) {
    dimensions = dim;
    halfdimensions = dim / 2;
    data.resize(halfdimensions.x * halfdimensions.y * halfdimensions.z);
  }

  void setVoxel(glm::ivec3 pos) {
    data.at(idx(pos)) |= bitMask(pos);
  }

  void clearVoxel(glm::ivec3 pos) {
    data.at(idx(pos)) &= ~bitMask(pos);
  }

  std::uint8_t const *getData() const { return data.data(); }
  unsigned int getTexture() const { return worldTexture; }

  void rasterizeVoxelData(glm::ivec3 const& pos, glm::quat const& rot, VoxelData const& voxelData) {
    for (int x = 0; x < voxelData.getDimensions().x; ++x) {
      for (int y = 0; y < voxelData.getDimensions().y; ++y) {
        for (int z = 0; z < voxelData.getDimensions().z; ++z) {
          auto voxel = voxelData.getVoxel({x, y, z});
          auto voxelPos = glm::ivec3(x, y, z);
          auto worldPos = glm::ivec3(glm::vec3(pos) + rot * glm::vec3(voxelPos));
          if (voxel != 0) {
            setVoxel(worldPos);
          } else {
            clearVoxel(worldPos);
          }
        }
      }
    }
  }

  void sync() {
    worldTexture = RenderSystem::createVoxelTexture(data.data(), halfdimensions);
  }

private:
  constexpr std::uint32_t idx(glm::ivec3 pos) {
    auto pos0 = pos >> 1;
    return pos0.x + pos0.y * halfdimensions.x + pos0.z * halfdimensions.x * halfdimensions.y;
  }

  constexpr std::uint8_t bitMask(glm::ivec3 pos) {
    auto bitPos = pos & 0x01;
    return static_cast<std::uint8_t>(1 << (bitPos.x + bitPos.z * 2 + bitPos.y * 4));
  }

  std::vector<std::uint8_t> data;
  glm::ivec3 dimensions;
  glm::ivec3 halfdimensions;
  unsigned int worldTexture;
};
