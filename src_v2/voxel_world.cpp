#include <rendering/voxel_world.hpp>

#include <glm/gtx/quaternion.hpp>
#include <rendering/render_utils.hpp>

#include <glad/gl.h>

void VoxelWorld::init(glm::ivec3 dim) {
  dimensions = dim;
  halfdimensions = dim / 2;
  data.resize(halfdimensions.x * halfdimensions.y * halfdimensions.z);
  worldTexture = CreateVoxelTexture(data.data(), halfdimensions);
}

void VoxelWorld::setVoxel(glm::ivec3 pos) { data.at(idx(pos)) |= bitMask(pos); }

void VoxelWorld::clearVoxel(glm::ivec3 pos) { data.at(idx(pos)) &= ~bitMask(pos); }

std::uint8_t const *VoxelWorld::getData() const { return data.data(); }

unsigned int VoxelWorld::getTexture() const { return worldTexture; }

glm::ivec3 VoxelWorld::getDimensions() const { return dimensions; }

void VoxelWorld::rasterizeVoxelData(glm::ivec3 const &pos, glm::quat const &rot, VoxelData const &voxelData,
                                    bool clear) {
  for(int x = 0; x < voxelData.getDimensions().x; ++x) {
    for(int y = 0; y < voxelData.getDimensions().y; ++y) {
      for(int z = 0; z < voxelData.getDimensions().z; ++z) {
        auto voxel = voxelData.getVoxel({x, y, z});
        auto voxelPos = glm::ivec3(x, y, z);
        auto worldPos = glm::ivec3(glm::vec3(pos) + rot * glm::vec3(voxelPos));
        if(voxel != 0) {
          if(clear) {
            clearVoxel(worldPos);
          } else {
            setVoxel(worldPos);
          }
        }
      }
    }
  }
}

void VoxelWorld::sync() {
  // if(worldTexture != 0) {
  //   DeleteVoxelTexture(worldTexture);
  // }
  // worldTexture = CreateVoxelTexture(data.data(), halfdimensions);
  glBindTexture(GL_TEXTURE_3D, worldTexture);
  glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, halfdimensions.x, halfdimensions.y, halfdimensions.z, GL_RED, GL_UNSIGNED_BYTE,
                  data.data());
  glBindTexture(GL_TEXTURE_3D, 0);
}
