#pragma once
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

class Camera {
 public:
  double cursorX;
  double cursorY;

  glm::vec3 position;
  glm::vec3 direction;
  glm::vec3 camRight;
  glm::vec3 camUp;

  Camera() {
    cursorX = 0;
    cursorY = 0;
    position = glm::vec3(0, 0, 0);
    direction = glm::vec3(0, 0, -1);
    camRight = glm::vec3(1, 0, 0);
    camUp = glm::vec3(0, 1, 0);
  }

  glm::mat4 getViewProjectionMatrix() const {
    glm::mat4 projection = glm::perspective(glm::radians(90.f), 16.0f / 9.0f, 0.1f, 500.0f);
    glm::mat4 view = glm::lookAt(position, position + (direction * 1000.f), glm::vec3(0, 1, 0));
    return projection * view;
  }

  void update(struct GLFWwindow *window, float deltaTime) {
    double xpos = 0;
    double ypos = 0;

    glfwGetCursorPos(window, &xpos, &ypos);
    double dx = xpos - cursorX;
    double dy = ypos - cursorY;
    cursorX = xpos;
    cursorY = ypos;
    camRight = glm::normalize(glm::cross(direction, camUp));
    //  camUp = glm::normalize(glm::cross(camRight, direction));

    glm::mat4 rotMat(1.0f);
    rotMat = glm::rotate(rotMat, (float)-dx / 100.f, camUp);
    rotMat = glm::rotate(rotMat, (float)-dy / 100.0f, camRight);
    direction = glm::normalize(rotMat * glm::vec4(direction, 0.0f));

    // Movement
    float horizontalSpeed = 5.f * deltaTime;
    int state = glfwGetKey(window, GLFW_KEY_W);
    glm::vec3 horizontalDir = glm::normalize(glm::vec3(direction.x, 0, direction.z));
    if(state == GLFW_PRESS) {
      position += horizontalDir * horizontalSpeed;
    }
    state = glfwGetKey(window, GLFW_KEY_S);
    if(state == GLFW_PRESS) {
      position -= horizontalDir * horizontalSpeed;
    }
    state = glfwGetKey(window, GLFW_KEY_A);
    if(state == GLFW_PRESS) {
      position -= camRight * horizontalSpeed;
    }
    state = glfwGetKey(window, GLFW_KEY_D);
    if(state == GLFW_PRESS) {
      position += camRight * horizontalSpeed;
    }

    float verticalSpeed = 5.f * deltaTime;
    state = glfwGetKey(window, GLFW_KEY_SPACE);
    if(state == GLFW_PRESS) {
      position += camUp * verticalSpeed;
    }
    state = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
    if(state == GLFW_PRESS) {
      position -= camUp * verticalSpeed;
    }
  }
  void setPosition(glm::vec3 pos) { position = pos; }
  void setDirection(glm::vec3 dir) { direction = glm::normalize(dir); }
  glm::vec3 getPosition() { return position; }
  glm::vec3 getDirection() { return direction; }
  glm::vec3 getRight() { return camRight; }
  glm::vec3 getUp() { return camUp; }
};
