#include <spdlog/spdlog.h>

#include <core/components.hpp>
#include <core/voxel_data.hpp>
#include <core/voxlight.hpp>
#include <rendering/render_system.hpp>
#include <rendering/render_utils.hpp>
#include <voxlight_api.hpp>

VoxelComponentApi::VoxelComponentApi(Voxlight &voxlight) : voxlight(voxlight) {}

void VoxelComponentApi::addComponent(entt::entity entity, VoxelData const &voxelData) {
  auto &voxelComponent = voxlight.registry.emplace<VoxelComponent>(entity);
  voxelComponent.voxelData = voxelData;
  voxelComponent.needsUpdate = true;
  auto &transformComponent = voxlight.registry.get<TransformComponent>(entity);
  voxelComponent.lastPosition = transformComponent.position;
  voxelComponent.lastRotation = transformComponent.rotation;

  VoxelComponentCreateEvent event(entity, voxelComponent);
  voxlight.voxelComponentEventManager.publish(VoxelComponentEventType::OnVoxelDataCreation, event);
}

void VoxelComponentApi::removeComponent(entt::entity entity) {
  auto voxelComponent = voxlight.registry.get<VoxelComponent>(entity);
  VoxelComponentDestroyEvent event(entity, voxelComponent);
  voxlight.voxelComponentEventManager.publish(VoxelComponentEventType::OnVoxelDataDestruction, event);
  voxlight.registry.remove<VoxelComponent>(entity);
}

bool VoxelComponentApi::hasComponent(entt::entity entity) const {
  return voxlight.registry.all_of<VoxelComponent>(entity);
}

void VoxelComponentApi::setVoxelData(entt::entity entity, VoxelData const &voxelData) {
  auto &voxelComponent = voxlight.registry.get<VoxelComponent>(entity);
  VoxelComponentModifyEvent event(entity, voxelComponent, voxelData, {0, 0, 0});
  voxlight.voxelComponentEventManager.publish(VoxelComponentEventType::OnVoxelDataChange, event);
  voxelComponent.voxelData = voxelData;
}

void VoxelComponentApi::subscribe(VoxelComponentEventType eventType, VoxelComponentEventCallback listener) {
  voxlight.voxelComponentEventManager.subscribe(eventType, listener);
}
