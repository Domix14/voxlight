
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <ICamera.hpp>
#include <glm/glm.hpp>

class Camera : public ICamera {
    double cursorX;
    double cursorY;

    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 camRight;
    glm::vec3 camUp;

   public:
    Camera();

    glm::mat4 getViewProjectionMatrix() const override;

    void update(class GLFWwindow* window, double deltaTime);
    void setPosition(glm::vec3 pos) { position = pos; }
    void setDirection(glm::vec3 dir) { direction = glm::normalize(dir); }
    glm::vec3 getPosition() { return position; }
    glm::vec3 getDirection() { return direction; }
    glm::vec3 getRight() { return camRight; }
    glm::vec3 getUp() { return camUp; }
};

#endif  // CAMERA_HPP