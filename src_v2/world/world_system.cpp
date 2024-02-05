#include "world_system.hpp"

#include <entt/entt.hpp>

#include "controller/controller_system.hpp"
#include "core/components.hpp"
#include "core/voxel_data.hpp"
#include "engine_config.hpp"
#include "rendering/render_system.hpp"
#include "voxel_engine.hpp"

static inline std::uint32_t idx(glm::ivec3 pos) {
  auto pos0 = pos >> 1;
  return pos0.x + pos0.y * HalfWorldSize +
         pos0.z * HalfWorldSize * HalfWorldSize;
}

static inline std::uint8_t bitMask(glm::ivec3 pos) {
  auto bitPos = pos & 0x01;
  return static_cast<std::uint8_t>(1
                                   << (bitPos.x + bitPos.z * 2 + bitPos.y * 4));
}

void WorldSystem::init(VoxelEngine *voxelEngine) {
  engine = voxelEngine;
  voxelMap.resize(HalfWorldSize * HalfWorldSize * HalfWorldSize);
  //   engine->getRenderSystem().createWorldTexture(voxelMap,
  //                                                glm::ivec3(HalfWorldSize));
}

void WorldSystem::update(float) {
  auto &registry = engine->getRegistry();
  auto view = registry.view<VoxelComponent, TransformComponent>();

  auto cameraPos = engine->getControllerSystem().getCameraPosition();
  for (auto [entity, voxelData, transform] : view.each()) {
    glm::vec3 size = transform.scale;
    glm::vec3 minBox = transform.position;
    glm::vec3 maxBox = minBox + size;

    auto rotatedCameraPos = cameraPos; // TODO: rotate camera
    auto closestPoint =
        glm::vec3(glm::clamp(rotatedCameraPos.x, minBox.x, maxBox.x),
                  glm::clamp(rotatedCameraPos.y, minBox.y, maxBox.y),
                  glm::clamp(rotatedCameraPos.z, minBox.z, maxBox.z));
    voxelData.distance = glm::distance(rotatedCameraPos, closestPoint);
  }
  engine->getRegistry().sort<VoxelComponent>(
      [](auto const &a, auto const &b) { return a.distance > b.distance; });
}

void WorldSystem::deinit() {}

void WorldSystem::setVoxel(glm::ivec3 position) {
  voxelMap[idx(position)] |= bitMask(position);
}

void WorldSystem::clearVoxel(glm::ivec3 position) {
  voxelMap[idx(position)] &= ~bitMask(position);
}

std::uint32_t
WorldSystem::createVoxelEntity(VoxelData<std::uint8_t> const &data,
                               glm::vec3 position, glm::quat rotation) {
  glm::ivec3 size = {data.getWidth(), data.getHeight(), data.getDepth()};
  auto texId = engine->getRenderSystem().createVoxelTexture(
      data.getDataAsVector(), size);
  auto voxelEntity = engine->getRegistry().create();
  engine->getRegistry().emplace<VoxelComponent>(voxelEntity, texId, true, 0.f);
  engine->getRegistry().emplace<TransformComponent>(
      voxelEntity, position,
      glm::vec3(data.getWidth(), data.getHeight(), data.getDepth()), rotation);

  // engine->worldSystem()->updateWorldTexture(data, size,
  //                                                            glm::ivec3(HalfWorldSize));
  //                                                            // todo:
  //                                                            temporary
  //                                                            solution
  return 0;
}