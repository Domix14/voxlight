#include <api/voxlight_api.hpp>
#include <core/components.hpp>
#include <spdlog/spdlog.h>
#include <voxlight.hpp>

EntityApi::EntityApi(Voxlight &voxlight) : voxlight(voxlight) {}

entt::entity EntityApi::createEntity(std::string const &name, TransformComponent const &transformComponent) {
  auto newEntity = voxlight.registry.create();
  voxlight.registry.emplace<NameComponent>(newEntity, name);
  voxlight.registry.emplace<TransformComponent>(newEntity, transformComponent);
  return newEntity;
}

entt::entity EntityApi::getFirstWithName(std::string_view name) const {
  auto view = voxlight.registry.view<NameComponent>();
  for(auto [entity, nameComponent] : view.each()) {
    if(name == nameComponent.name) {
      return entity;
    }
  }
  return entt::null;
}

TransformComponent const &EntityApi::getTransform(entt::entity entity) const {
  return voxlight.registry.get<TransformComponent>(entity);
}

std::string const &EntityApi::getName(entt::entity entity) const {
  return voxlight.registry.get<NameComponent>(entity).name;
}

void EntityApi::setName(entt::entity entity, std::string_view name) {
  voxlight.registry.get<NameComponent>(entity).name = name;
}

void EntityApi::setPosition(entt::entity entity, glm::vec3 const &position) {
  auto& transformComponent = voxlight.registry.get<TransformComponent>(entity);
  if(voxlight.registry.all_of<VoxelComponent>(entity)) {
    auto& voxelComponent = voxlight.registry.get<VoxelComponent>(entity);
    voxelComponent.lastPosition = transformComponent.position;
    voxelComponent.needsUpdate = true;
  }
  transformComponent.position = position;
}

void EntityApi::setScale(entt::entity entity, glm::vec3 const &scale) {
  voxlight.registry.get<TransformComponent>(entity).scale = scale;
  if(voxlight.registry.all_of<VoxelComponent>(entity)) {
    voxlight.registry.get<VoxelComponent>(entity).needsUpdate = true;
  }
}

void EntityApi::setRotation(entt::entity entity, glm::quat const &rotation) {
  auto& transformComponent = voxlight.registry.get<TransformComponent>(entity);
  if(voxlight.registry.all_of<VoxelComponent>(entity)) {
    auto& voxelComponent = voxlight.registry.get<VoxelComponent>(entity);
    voxelComponent.lastRotation = transformComponent.rotation;
    voxelComponent.needsUpdate = true;
  }
  transformComponent.rotation = rotation;
}

void EntityApi::setTransform(entt::entity entity, TransformComponent const &transform) {
  voxlight.registry.get<TransformComponent>(entity) = transform;
  if(voxlight.registry.all_of<VoxelComponent>(entity)) {
    voxlight.registry.get<VoxelComponent>(entity).needsUpdate = true;
  }
}
