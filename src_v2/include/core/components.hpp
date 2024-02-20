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

struct VoxelComponentEvent : public Event<VoxelComponentEventType> {
  VoxelComponentEvent(entt::entity entity, VoxelComponent const& voxelComponent)
    : entity(entity), voxelComponent(voxelComponent) {}
  
  entt::entity entity;
  VoxelComponent const& voxelComponent;
};
