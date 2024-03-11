#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "../core/voxel_data.hpp"
#include "render_utils.hpp"

class VoxelWorld {
 public:
  void init(glm::ivec3 dim);
  void setVoxel(glm::ivec3 pos);
  void clearVoxel(glm::ivec3 pos);

  std::uint8_t const* getData() const;
  unsigned int getTexture() const;
  glm::ivec3 getDimensions() const;

  void rasterizeVoxelData(glm::ivec3 const& pos, glm::quat const& rot, VoxelData const& voxelData, bool clear);

  void sync();

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
  unsigned int worldTexture = 0;
};
