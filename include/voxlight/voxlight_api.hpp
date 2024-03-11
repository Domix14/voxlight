#pragma once

#include <cinttypes>
#include <entt/fwd.hpp>
#include <glm/fwd.hpp>
#include <string_view>

#include "core/system.hpp"

/// Forward declarations
class Voxlight;
struct GLFWwindow;
struct TransformComponent;
struct CustomComponent;
struct CameraComponent;
class VoxelData;
struct VoxelComponent;
struct VoxelComponentEvent;
struct EntityEvent;
struct EngineEvent;

//----------------------------------------------------------------------------//
// Engine API
//----------------------------------------------------------------------------//

enum class EngineEventType {
  OnWindowResize,
};

using EngineEventCallback = std::function<void(EngineEventType, EngineEvent const &)>;
class EngineApi {
 public:
  /**
   *  \brief Start the engine
   *   Start the engine looop and run all systems. Engine will run until stop() is called.
   */
  void start();

  /**
   * \brief Stop the engine
   *  Stop the engine loop and all systems.
   */
  void stop();

  /**
   * \brief Returns the GLFW window pointer
   * \return GLFW window pointer
   */
  GLFWwindow *getGLFWwindow();

  /**
   * \brief Returns the entt registry
   * \return entt registry
   */
  entt::registry &getRegistry();

  /**
   * \brief Sets the window resolution
   * \param width The width of the window
   * \param height The height of the window
   */
  void setWindowResolution(std::uint32_t width, std::uint32_t height);

  /**
   * \brief Adds a system to the engine
   * \tparam T The system to add
   */
  template <std::derived_from<System> T>
  void addSystem() {
    addSystemInternal(std::make_unique<T>(voxlight));
  }

  /**
   * \brief Subscribes to an engine event
   * \param eventType The type of event to subscribe to
   * \param listener The callback to call when the event is triggered
   */
  void subscribe(EngineEventType eventType, EngineEventCallback listener);

  EngineApi(Voxlight &voxlight);

 private:
  void addSystemInternal(std::unique_ptr<System> newSystem);
  Voxlight &voxlight;
};

//----------------------------------------------------------------------------//
// Entity API
//----------------------------------------------------------------------------//

enum EntityEventType {
  OnTransformChange,
};

using EntityEventCallback = std::function<void(EntityEventType, EntityEvent const &)>;

class EntityApi {
 public:
  /**
   * \brief Creates an entity
   * Creates an entity with a name and a transform component
   * \param name The name of the entity
   * \param transformComponent The transform component of the entity
   * \return The created entity
   */
  entt::entity createEntity(std::string const &name, TransformComponent const &transformComponent);

  /**
   * \brief Returns first entity with name
   * \param name The name of the entity
   * \return The entity with provided name
   */
  entt::entity getFirstWithName(std::string_view name) const;

  /**
   * \brief Returns the transform component of an entity
   * \param entity The entity to get the transform component of
   * \return The transform component of the entity
   */
  TransformComponent const &getTransform(entt::entity entity) const;

  /**
   * \brief Returns the name of an entity
   * \param entity The entity to get the name of
   * \return The name of the entity
   */
  std::string const &getName(entt::entity entity) const;

  /**
   * \brief Returns the name of an entity
   * \param entity The entity to get the name of
   * \param name The name to set
   */
  void setName(entt::entity entity, std::string_view name);

  /**
   * \brief Sets the position of an entity
   * \param entity The entity to set the position of
   * \return The position of the entity
   */
  void setPosition(entt::entity entity, glm::vec3 position);

  /**
   * \brief Sets the scale of an entity
   * \param entity The entity to get the scale of
   * \return The scale of the entity
   */
  void setScale(entt::entity entity, glm::vec3 scale);

  /**
   * \brief Sets the rotation of an entity
   * \param entity The entity to get the rotation of
   * \return The rotation of the entity
   */
  void setRotation(entt::entity entity, glm::quat rotation);

  /**
   * \brief Sets the transform of an entity
   * \param entity The entity to set the transform of
   * \return The transform of the entity
   */
  void setTransform(entt::entity entity, TransformComponent const &transform);

  /**
   * \brief Subscribes to an entity event
   * \param eventType The type of event to subscribe to
   * \param listener The callback to call when the event is triggered
   */
  void subscribe(EntityEventType eventType, EntityEventCallback listener);

  EntityApi(Voxlight &voxlight);

 private:
  Voxlight &voxlight;
};

//----------------------------------------------------------------------------//
// Voxel Component API
//----------------------------------------------------------------------------//

enum VoxelComponentEventType {
  OnVoxelDataCreation,
  OnVoxelDataDestruction,
  OnVoxelDataChange,
};

using VoxelComponentEventCallback = std::function<void(VoxelComponentEventType, VoxelComponentEvent const &)>;

class VoxelComponentApi {
 public:
  /**
   * \brief Adds a voxel component to an entity
   * \param entity The entity to add the voxel component to
   * \param voxelData The voxel data to add
   */
  void addComponent(entt::entity entity, VoxelData const &voxelData);

  /**
   * \brief Removes a voxel component from an entity
   * \param entity The entity to remove the voxel component from
   */
  void removeComponent(entt::entity entity);

  /**
   * \brief Checks if an entity has a voxel component
   * \param entity The entity to check
   * \return True if the entity has a voxel component, false otherwise
   */
  bool hasComponent(entt::entity entity) const;

  /**
   * \brief Sets voxel data of voxel component
   * \param entity The entity to set the voxel data of
   * \param voxelData The voxel data to set
   */
  void setVoxelData(entt::entity entity, VoxelData const &voxelData);

  /**
   * \brief Subscribes to a voxel component event
   * \param eventType The type of event to subscribe to
   * \param listener The callback to call when the event is triggered
   */
  void subscribe(VoxelComponentEventType eventType, VoxelComponentEventCallback listener);

  VoxelComponentApi(Voxlight &voxlight);

 private:
  Voxlight &voxlight;
};

//----------------------------------------------------------------------------//
// Camera Component API
//----------------------------------------------------------------------------//
class CameraComponentApi {
 public:
  /**
   * \brief Adds a camera component to an entity
   * \param entity The entity to add the camera component to
   */
  void addComponent(entt::entity entity);

  /**
   * \brief Removes a camera component from an entity
   * \param entity The entity to remove the camera component from
   */
  void removeComponent(entt::entity entity);

  /**
   * \brief Checks if an entity has a camera component
   * \param entity The entity to check
   * \return True if the entity has a camera component, false otherwise
   */
  bool hasComponent(entt::entity entity) const;

  /**
   * \brief Returns the camera component of an entity
   * \param entity The entity to get the camera component of
   * \return The camera component of the entity
   */
  CameraComponent const &getComponent(entt::entity entity) const;

  /**
   * \brief Sets the projection matrix of a camera component
   * \param entity The entity to set the projection matrix of
   * \param projectionMatrix The projection matrix to set
   */
  void setProjectionMatrix(entt::entity entity, glm::mat4 const &projectionMatrix);

  /**
   * \brief Sets the direction of a camera component
   * \param entity The entity to set the direction of
   * \param direction The direction to set
   */
  void setDirection(entt::entity entity, glm::vec3 const &direction);

  /**
   * \brief Returns current camera
   * Returns entity with current selected camera component
   * \return Current camera entity
   */
  entt::entity getCurrentCamera() const;

  /**
   * \brief Sets current camera
   * Sets current selected camera component
   * \param camera The entity to set as current camera
   */
  void setCurrentCamera(entt::entity camera);

  /**
   * \brief Returns view matrix
   * Returns view matrix of current camera
   * \return View matrix
   */
  glm::mat4 getViewProjectionMatrix() const;

  CameraComponentApi(Voxlight &voxlight);

 private:
  Voxlight &voxlight;
};
