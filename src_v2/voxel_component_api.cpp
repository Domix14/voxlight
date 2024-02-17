#include "api/voxlight_api.hpp"
#include "core/components.hpp"
#include "core/voxel_data.hpp"
#include "rendering/render_system.hpp"
#include "voxlight.hpp"
#include "core/voxel_world.hpp"

VoxelComponentApi::VoxelComponentApi(Voxlight &voxlight) : voxlight(voxlight) {}

void VoxelComponentApi::addComponent(entt::entity entity, VoxelData const &voxelData) {
  auto &voxelComponent = voxlight.registry.emplace<VoxelComponent>(entity);
  voxelComponent.voxelData = voxelData;
  voxelComponent.textureId = RenderSystem::createVoxelTexture(voxelComponent.voxelData.getData(), voxelComponent.voxelData.getDimensions());
  auto& transform = voxlight.registry.get<TransformComponent>(entity);
  voxlight.voxelWorld.rasterizeVoxelData(transform.position, transform.rotation, voxelData);
}

void VoxelComponentApi::removeComponent(entt::entity entity) {
  auto voxelComponent = voxlight.registry.get<VoxelComponent>(entity);
  RenderSystem::deleteVoxelTexture(voxelComponent.textureId);
  voxlight.registry.remove<VoxelComponent>(entity);
}

bool VoxelComponentApi::hasComponent(entt::entity entity) const {
  return voxlight.registry.all_of<VoxelComponent>(entity);
}

void VoxelComponentApi::setVoxelData(entt::entity entity, VoxelData const &voxelData) {
  auto &voxelComponent = voxlight.registry.get<VoxelComponent>(entity);
  voxelComponent.voxelData = voxelData;
  RenderSystem::deleteVoxelTexture(voxelComponent.textureId);
  voxelComponent.textureId = RenderSystem::createVoxelTexture(voxelComponent.voxelData.getData(), voxelComponent.voxelData.getDimensions());
}
