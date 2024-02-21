#pragma once

#include <core/voxel_data.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <core/event.hpp>
#include <api/voxlight_api.hpp>


struct NameComponent {
  std::string name;
};

struct TransformComponent {
  glm::vec3 position;
  glm::vec3 scale;
  glm::quat rotation;
};

struct VoxelComponent {
  unsigned int textureId;
  bool needsUpdate;
  float distance;
  glm::vec3 lastPosition;
  glm::quat lastRotation;
  VoxelData voxelData;
};

struct CameraComponent {
  glm::mat4 projectionMatrix;
  glm::vec3 direction;
};

/// Events

struct EntityEvent : public Event<EntityEventType> {
  EntityEvent(entt::entity entity, TransformComponent const& transformComponent, TransformComponent oldTransform)
    : entity(entity), transformComponent(transformComponent), oldTransform(oldTransform) {}
  
  entt::entity entity;
  TransformComponent const& transformComponent;
  TransformComponent oldTransform;
};

struct VoxelComponentEvent : public Event<VoxelComponentEventType> {
  VoxelComponentEvent(entt::entity entity, VoxelComponent const& voxelComponent, VoxelData const& newVoxelData)
    : entity(entity), voxelComponent(voxelComponent), newVoxelData(newVoxelData) {}
  
  entt::entity entity;
  VoxelComponent const& voxelComponent;
  VoxelData const& newVoxelData;
};
