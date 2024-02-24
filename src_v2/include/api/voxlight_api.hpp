#pragma once

#include <cinttypes>
#include <entt/fwd.hpp>
#include <glm/fwd.hpp>
#include <string_view>
#include <system.hpp>

// Forward declarations
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

enum class EngineEventType : std::uint8_t {
  OnWindowResize,
};

using EngineEventCallback = std::function<void(EngineEventType, EngineEvent const &)>;
class EngineApi {
 public:
  void start();
  void stop();

  GLFWwindow *getGLFWwindow();
  entt::registry &getRegistry();

  void setWindowResolution(int width, int height);

  template <std::derived_from<System> T>
  void addSystem() {
    addSystemInternal(std::make_unique<T>(voxlight));
  }

  void subscribe(EngineEventType eventType, EngineEventCallback listener);

  EngineApi(Voxlight &voxlight);

 private:
  void addSystemInternal(std::unique_ptr<System> newSystem);
  Voxlight &voxlight;
};

enum EntityEventType : std::uint8_t {
  OnTransformChange,
};

using EntityEventCallback = std::function<void(EntityEventType, EntityEvent const &)>;

class EntityApi {
 public:
  entt::entity createEntity(std::string const &name, TransformComponent const &transformComponent);

  entt::entity getFirstWithName(std::string_view name) const;
  TransformComponent const &getTransform(entt::entity entity) const;
  std::string const &getName(entt::entity entity) const;

  void setName(entt::entity entity, std::string_view name);
  void setPosition(entt::entity entity, glm::vec3 const &position);
  void setScale(entt::entity entity, glm::vec3 const &scale);
  void setRotation(entt::entity entity, glm::quat const &rotation);
  void setTransform(entt::entity entity, TransformComponent const &transform);

  void subscribe(EntityEventType eventType, EntityEventCallback listener);

  EntityApi(Voxlight &voxlight);

 private:
  Voxlight &voxlight;
};

enum VoxelComponentEventType : std::uint8_t {
  OnVoxelDataCreation,
  OnVoxelDataDestruction,
  OnVoxelDataChange,
};

using VoxelComponentEventCallback = std::function<void(VoxelComponentEventType, VoxelComponentEvent const &)>;

class VoxelComponentApi {
 public:
  void addComponent(entt::entity entity, VoxelData const &voxelData);
  void removeComponent(entt::entity entity);
  bool hasComponent(entt::entity entity) const;

  void setVoxelData(entt::entity entity, VoxelData const &voxelData);

  void subscribe(VoxelComponentEventType eventType, VoxelComponentEventCallback listener);

  VoxelComponentApi(Voxlight &voxlight);

 private:
  Voxlight &voxlight;
};
class CameraComponentApi {
 public:
  void addComponent(entt::entity entity);
  void removeComponent(entt::entity entity);
  bool hasComponent(entt::entity entity) const;
  CameraComponent const &getComponent(entt::entity entity) const;

  void setProjectionMatrix(entt::entity entity, glm::mat4 const &projectionMatrix);
  void setDirection(entt::entity entity, glm::vec3 const &direction);

  entt::entity getCurrentCamera() const;
  void setCurrentCamera(entt::entity camera);

  glm::mat4 getViewProjectionMatrix() const;

  CameraComponentApi(Voxlight &voxlight);

 private:
  Voxlight &voxlight;
};
