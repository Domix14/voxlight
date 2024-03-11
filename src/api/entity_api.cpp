#include <spdlog/spdlog.h>

#include <core/components.hpp>
#include <core/voxlight.hpp>
#include <voxlight_api.hpp>

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

void EntityApi::setPosition(entt::entity entity, glm::vec3 position) {
  auto &transformComponent = voxlight.registry.get<TransformComponent>(entity);
  TransformComponent oldTransform = transformComponent;
  transformComponent.position = position;

  EntityEvent event(entity, transformComponent, oldTransform);
  voxlight.entityEventManager.publish(EntityEventType::OnTransformChange, event);
}

void EntityApi::setScale(entt::entity entity, glm::vec3 scale) {
  voxlight.registry.get<TransformComponent>(entity).scale = scale;
}

void EntityApi::setRotation(entt::entity entity, glm::quat rotation) {
  auto &transformComponent = voxlight.registry.get<TransformComponent>(entity);
  TransformComponent oldTransform = transformComponent;
  transformComponent.rotation = rotation;

  EntityEvent event(entity, transformComponent, oldTransform);
  voxlight.entityEventManager.publish(EntityEventType::OnTransformChange, event);
}

void EntityApi::setTransform(entt::entity entity, TransformComponent const &transform) {
  auto &transformComponent = voxlight.registry.get<TransformComponent>(entity);
  TransformComponent oldTransform = transformComponent;
  transformComponent = transform;

  EntityEvent event(entity, transformComponent, oldTransform);
  voxlight.entityEventManager.publish(EntityEventType::OnTransformChange, event);
}

void EntityApi::subscribe(EntityEventType eventType, EntityEventCallback listener) {
  voxlight.entityEventManager.subscribe(eventType, listener);
}
