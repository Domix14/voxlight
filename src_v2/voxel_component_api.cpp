#include "api/voxlight_api.hpp"
#include "core/components.hpp"
#include "core/voxel_data.hpp"
#include "rendering/render_system.hpp"
#include "rendering/render_utils.hpp"
#include "voxlight.hpp"
#include <spdlog/spdlog.h>

VoxelComponentApi::VoxelComponentApi(Voxlight &voxlight) : voxlight(voxlight) {}

void VoxelComponentApi::addComponent(entt::entity entity, VoxelData const &voxelData) {
  auto &voxelComponent = voxlight.registry.emplace<VoxelComponent>(entity);
  voxelComponent.voxelData = voxelData;
  voxelComponent.needsUpdate = true;
  auto &transformComponent = voxlight.registry.get<TransformComponent>(entity);
  voxelComponent.lastPosition = transformComponent.position;
  voxelComponent.lastRotation = transformComponent.rotation;

  voxlight.voxelComponentEventManager.publish(VoxelComponentEventType::OnVoxelDataCreation,
                                              {entity, voxelComponent, voxelComponent.voxelData});
}

void VoxelComponentApi::removeComponent(entt::entity entity) {
  auto voxelComponent = voxlight.registry.get<VoxelComponent>(entity);
  voxlight.voxelComponentEventManager.publish(VoxelComponentEventType::OnVoxelDataDestruction,
                                              {entity, voxelComponent, voxelComponent.voxelData});
  voxlight.registry.remove<VoxelComponent>(entity);
}

bool VoxelComponentApi::hasComponent(entt::entity entity) const {
  return voxlight.registry.all_of<VoxelComponent>(entity);
}

void VoxelComponentApi::setVoxelData(entt::entity entity, VoxelData const &voxelData) {
  auto &voxelComponent = voxlight.registry.get<VoxelComponent>(entity);
  VoxelComponentEvent event(entity, voxelComponent, voxelData);
  voxlight.voxelComponentEventManager.publish(VoxelComponentEventType::OnVoxelDataChange, event);
  voxelComponent.voxelData = voxelData;
}

void VoxelComponentApi::subscribe(VoxelComponentEventType eventType, VoxelComponentEventCallback listener) {
  voxlight.voxelComponentEventManager.subscribe(eventType, listener);
}
