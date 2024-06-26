#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "components.hpp"
#include "event.hpp"
#include "voxel_data.hpp"

//----------------------------------------------------------------------------//
// Engine Events
//----------------------------------------------------------------------------//

enum class EngineEventType {
  OnWindowResize,
};

struct WindowResizeEvent {
  std::uint32_t windowWidth;
  std::uint32_t windowHeight;
};

using EngineEvent = Event<EngineEventType, WindowResizeEvent>;
using EngineEventCallback = std::function<void(EngineEventType, EngineEvent)>;

//----------------------------------------------------------------------------//
// Entity Events
//----------------------------------------------------------------------------//

enum EntityEventType {
  OnTransformChange,
};

struct EntityTransformEvent {
  entt::entity entity;
  TransformComponent const& transformComponent;
  TransformComponent oldTransform;
};

using EntityEvent = Event<EntityEventType, EntityTransformEvent>;
using EntityEventCallback = std::function<void(EntityEventType, EntityEvent)>;

//----------------------------------------------------------------------------//
// Voxel Component Events
//----------------------------------------------------------------------------//

enum VoxelComponentEventType {
  OnVoxelDataCreation,
  OnVoxelDataDestruction,
  OnVoxelDataChange,
};

struct VoxelComponentCreateEvent {
  entt::entity entity;
  VoxelComponent const& voxelComponent;
};

using VoxelComponentDestroyEvent = VoxelComponentCreateEvent;

struct VoxelComponentModifyEvent {
  entt::entity entity;
  VoxelComponent const& voxelComponent;
  VoxelData const& voxelData;
  glm::ivec3 offset;
};

using VoxelComponentEvent = Event<VoxelComponentEventType, VoxelComponentCreateEvent, VoxelComponentModifyEvent>;
using VoxelComponentEventCallback = std::function<void(VoxelComponentEventType, VoxelComponentEvent)>;
