#include "api/voxlight_api.hpp"
#include "core/components.hpp"
#include "voxlight.hpp"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

CameraComponentApi::CameraComponentApi(Voxlight &voxlight) : voxlight(voxlight) {}

void CameraComponentApi::addComponent(entt::entity entity) {
  voxlight.registry.emplace<CameraComponent>(entity, CameraComponent());
}

void CameraComponentApi::removeComponent(entt::entity entity) {
  if(voxlight.currentCamera == entity) {
    spdlog::error("Failed to remove CameraComponent. Camera is currently in use.");
    return;
  }

  voxlight.registry.remove<CameraComponent>(entity);
}

bool CameraComponentApi::hasComponent(entt::entity entity) const {
  return voxlight.registry.all_of<CameraComponent>(entity);
}

CameraComponent const &CameraComponentApi::getComponent(entt::entity entity) const {
  return voxlight.registry.get<CameraComponent>(entity);
}

void CameraComponentApi::setProjectionMatrix(entt::entity entity, glm::mat4 const &projectionMatrix) {
  voxlight.registry.get<CameraComponent>(entity).projectionMatrix = projectionMatrix;
}

void CameraComponentApi::setDirection(entt::entity entity, glm::vec3 const &direction) {
  voxlight.registry.get<CameraComponent>(entity).direction = direction;
}

entt::entity CameraComponentApi::getCurrentCamera() const { return voxlight.currentCamera; }

void CameraComponentApi::setCurrentCamera(entt::entity camera) { voxlight.currentCamera = camera; }

glm::mat4 CameraComponentApi::getViewProjectionMatrix() const {
  auto [cameraComponent, transformComponent] =
      voxlight.registry.get<CameraComponent, TransformComponent>(voxlight.currentCamera);
  auto cameraView = glm::lookAt(transformComponent.position, transformComponent.position + cameraComponent.direction,
                                {0.f, 1.f, 0.f});
  return cameraComponent.projectionMatrix * cameraView;
}
