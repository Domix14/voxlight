
#include <Camera.hpp>
#include <GLFW/glfw3.h>
#include "glm/gtc/matrix_transform.hpp"

void Camera::update(GLFWwindow* window) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    double dx = xpos - curosorX;
    double dy = ypos - curosorY;
    curosorX = xpos;
    curosorY = ypos;
    camRight = glm::normalize(glm::cross(direction, {0,1,0}));
    camUp = glm::normalize(glm::cross(camRight, direction));

    glm::mat4 rotMat(1.0f);
    rotMat = glm::rotate(rotMat, (float)-dx/100.f, camUp);
    rotMat = glm::rotate(rotMat, (float)-dy/100.0f, camRight);
    direction = glm::normalize(rotMat * glm::vec4(direction, 0.0f));

    // Movement
    int state = glfwGetKey(window, GLFW_KEY_W);
    if (state == GLFW_PRESS)
    {
        position += direction * 0.6f;
    }
    state = glfwGetKey(window, GLFW_KEY_S);
    if (state == GLFW_PRESS)
    {
        position -= direction * 0.6f;
    }
    state = glfwGetKey(window, GLFW_KEY_A);
    if (state == GLFW_PRESS)
    {
        position -= camRight * 0.6f;
    }
    state = glfwGetKey(window, GLFW_KEY_D);
    if (state == GLFW_PRESS)
    {
        position += camRight * 0.6f;
    }
}