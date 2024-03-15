#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

class VoxelData {
 public:
  void setVoxel(glm::ivec3 pos, std::uint8_t voxel);
  std::uint8_t getVoxel(glm::ivec3 pos) const;
  std::uint8_t const *getData() const;
  std::vector<std::uint8_t> getDataAsVector() const;
  glm::ivec3 getDimensions() const;
  void resize(glm::ivec3 newSize);
  std::size_t getByteSize() const;
  void fill(std::uint8_t voxel);
  void loadFromFile(std::filesystem::path path, std::string_view name);

 private:
  std::size_t getIndex(glm::ivec3 pos) const;

  std::vector<std::uint8_t> data;
  glm::ivec3 dimensions;
};
