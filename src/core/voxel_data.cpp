#define OGT_VOX_IMPLEMENTATION
#include <spdlog/spdlog.h>
#include <utils/ogt_vox.h>

#include <core/voxel_data.hpp>
#include <fstream>

void VoxelData::setVoxel(glm::ivec3 pos, std::uint8_t voxel) { data.at(getIndex(pos)) = voxel; }

std::uint8_t VoxelData::getVoxel(glm::ivec3 pos) const { return data.at(getIndex(pos)); }

std::uint8_t const* VoxelData::getData() const { return data.data(); }

std::vector<std::uint8_t> VoxelData::getDataAsVector() const { return data; }

glm::ivec3 VoxelData::getDimensions() const { return dimensions; }

void VoxelData::resize(glm::ivec3 newSize) {
  dimensions = newSize;
  data.resize(dimensions.x * dimensions.y * dimensions.z);
}

std::size_t VoxelData::getByteSize() const { return sizeof(std::uint8_t) * data.size(); }

void VoxelData::fill(std::uint8_t voxel) { std::fill(data.begin(), data.end(), voxel); }

std::size_t VoxelData::getIndex(glm::ivec3 pos) const {
  return pos.x + pos.y * dimensions.x + pos.z * dimensions.x * dimensions.y;
}

void VoxelData::loadFromFile(std::filesystem::path path, std::string_view name) {
  std::ifstream file(path, std::ios::binary);
  if(!file.is_open()) {
    spdlog::error("Failed to open file: {}", path.string());
    return;
  }

  std::vector<std::uint8_t> buffer((std::istream_iterator<std::uint8_t>(file)), std::istream_iterator<std::uint8_t>());
  ogt_vox_scene const* scene = ogt_vox_read_scene(buffer.data(), buffer.size());

  if(scene == nullptr) {
    spdlog::error("Failed to load vox file: {}", path.string());
    return;
  }

  for(std::size_t i = 0; i < scene->num_instances; ++i) {
    ogt_vox_instance instance = scene->instances[i];
    if(instance.name == name) {
      ogt_vox_model const* model = scene->models[instance.model_index];
      resize({model->size_x, model->size_z, model->size_y});

      for(std::size_t x = 0; x < model->size_x; ++x) {
        for(std::size_t y = 0; y < model->size_y; ++y) {
          for(std::size_t z = 0; z < model->size_z; ++z) {
            std::uint8_t voxel = model->voxel_data[x + y * model->size_x + z * model->size_x * model->size_y];
            setVoxel({x, z, y}, voxel);
          }
        }
      }
      break;
    }
  }
}
