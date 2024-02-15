#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

class VoxelData {
public:
  VoxelData() = default;
  ~VoxelData() = default;

  void setVoxel(glm::uvec3 pos, std::uint8_t voxel) {
    data.at(getIndex(pos)) = voxel;
  };
  std::uint8_t getVoxel(glm::uvec3 pos) const {
    return data.at(getIndex(pos));
  };

  std::uint8_t const *getData() const { return data.data(); };
  std::vector<std::uint8_t> getDataAsVector() const { return data; };
  glm::uvec3 getDimensions() const { return dimensions; };
  void resize(glm::uvec3 newSize) {
    dimensions = newSize;
    data.resize(dimensions.x * dimensions.y * dimensions.z);
  };
  size_t getByteSize() const { return sizeof(std::uint8_t) * data.size(); };

  void fill(std::uint8_t voxel) {
    for (size_t i = 0; i < data.size(); ++i) {
      data[i] = voxel;
    }
  }

  //   void shrinkToFit() {
  //     bool voxelFlag = false;
  //     size_t minDepth = depth;
  //     size_t maxDepth = 0;
  //     for (size_t z = 0; z < depth; ++z) {
  //       voxelFlag = false;
  //       for (size_t x = 0; x < width && !voxelFlag; ++x) {
  //         for (size_t y = 0; y < height && !voxelFlag; ++y) {
  //           voxelFlag = getVoxel({x, y, z}) != 0;
  //           if (voxelFlag && z < minDepth) {
  //             minDepth = z;
  //           }
  //           if (voxelFlag && z > maxDepth) {
  //             maxDepth = z;
  //           }
  //         }
  //       }
  //     }

  //     size_t minWidth = width;
  //     size_t maxWidth = 0;
  //     for (size_t x = 0; x < width; ++x) {
  //       voxelFlag = false;
  //       for (size_t z = minDepth; z <= maxDepth && !voxelFlag; ++z) {
  //         for (size_t y = 0; y < height && !voxelFlag; ++y) {
  //           voxelFlag = getVoxel(x, y, z) != T(0);
  //           if (voxelFlag && x < minWidth) {
  //             minWidth = x;
  //           }
  //           if (voxelFlag != T(0) && x > maxWidth) {
  //             maxWidth = x;
  //           }
  //         }
  //       }
  //     }

  //     size_t minHeight = height;
  //     size_t maxHeight = 0;
  //     for (size_t y = 0; y < height; ++y) {
  //       voxelFlag = false;
  //       for (size_t x = minWidth; x <= maxWidth && !voxelFlag; ++x) {
  //         for (size_t z = minDepth; z <= maxDepth && !voxelFlag; ++z) {
  //           voxelFlag = getVoxel(x, y, z) != T(0);
  //           if (voxelFlag && y < minHeight) {
  //             minHeight = y;
  //           }
  //           if (voxelFlag && y > maxHeight) {
  //             maxHeight = y;
  //           }
  //         }
  //       }
  //     }

  //     std::vector<std::uint8_t> newData((maxWidth - minWidth + 1) *
  //                                       (maxHeight - minHeight + 1) *
  //                                       (maxDepth - minDepth + 1));
  //     for (size_t z = minDepth; z <= maxDepth; ++z) {
  //       for (size_t x = minWidth; x <= maxWidth; ++x) {
  //         for (size_t y = minHeight; y <= maxHeight; ++y) {
  //           newData[(x - minWidth) + (y - minHeight) * (maxWidth - minWidth +
  //           1) +
  //                   (z - minDepth) * (maxWidth - minWidth + 1) *
  //                       (maxHeight - minHeight + 1)] = getVoxel(x, y, z);
  //         }
  //       }
  //     }
  //     dimensions.x = maxWidth - minWidth + 1;
  //     dimensions.y = maxHeight - minHeight + 1;
  //     dimensions. = maxDepth - minDepth + 1;
  //     data = std::move(newData);
  //   };

  //   void loadFromVoxFile(std::filesystem::path path) {
  //     auto readTag = [](std::istreambuf_iterator<char> &iterator) {
  //       std::string tag;
  //       for (std::size_t i = 0; i < 4; i++) {
  //         tag += *iterator;
  //         std::advance(iterator, 1);
  //       }
  //       return tag;
  //     };

  //     auto readInt = [](std::istreambuf_iterator<char> &iterator) {
  //       std::array<std::uint8_t, 4> data;
  //       for (std::size_t i = 0; i < 4; i++) {
  //         data[i] = *iterator;
  //         std::advance(iterator, 1);
  //       }
  //       return static_cast<std::int32_t>(data[0] | data[1] << 8 | data[2] <<
  //       16 |
  //                                        data[3] << 24);
  //     };

  //     if (std::filesystem::exists(path) && path.extension() == ".vox") {
  //       std::ifstream file(path, std::ios::in | std::ios::binary);
  //       auto iterator = std::istreambuf_iterator<char>(file);

  //       readTag(iterator); // VOX tag
  //       readInt(iterator); // version

  //       while (iterator != std::istreambuf_iterator<char>()) {
  //         auto tag = readTag(iterator);
  //         auto contentSize = readInt(iterator);
  //         readInt(iterator); // Child chunks

  //         if (tag == "SIZE") {
  //           auto w = readInt(iterator);
  //           auto d = readInt(iterator);
  //           auto h = readInt(iterator);
  //           resize(w, h, d);
  //         } else if (tag == "XYZI") {
  //           // assert(data.size() == contentSize / 4);
  //           auto voxelCount = readInt(iterator);
  //           for (auto i = 0; i < voxelCount; i++) {
  //             auto x = static_cast<std::uint32_t>(*iterator);
  //             std::advance(iterator, 1);
  //             auto z = static_cast<std::uint32_t>(*iterator);
  //             std::advance(iterator, 1);
  //             auto y = static_cast<std::uint32_t>(*iterator);
  //             std::advance(iterator, 1);
  //             T colorIndex = static_cast<std::uint8_t>(*iterator);
  //             std::advance(iterator, 1);
  //             setVoxel(x, y, z, colorIndex);
  //           }
  //         } else {
  //           std::advance(iterator, contentSize);
  //         }
  //       }
  //     }
  //   }

private:
  size_t getIndex(glm::uvec3 pos) const {
    return pos.x + pos.y * dimensions.x + pos.z * dimensions.x * dimensions.y;
  };

  std::vector<std::uint8_t> data;
  glm::uvec3 dimensions;
};
