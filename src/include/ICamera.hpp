#pragma once

#include <glm/glm.hpp>

class ICamera {
   public:
    virtual glm::mat4 getViewProjectionMatrix() const = 0;
    virtual void update(class GLFWwindow* window, double deltaTime) = 0;
    virtual ~ICamera() {}
};
