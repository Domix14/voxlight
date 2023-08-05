
#include <Camera.hpp>
#include <GLFW/glfw3.h>
#include "glm/gtc/matrix_transform.hpp"

void Camera::update(GLFWwindow* window, double deltaTime) {
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
    float horizontalSpeed = 30.f * deltaTime;
    int state = glfwGetKey(window, GLFW_KEY_W);
    glm::vec3 horizontalDir = glm::normalize(glm::vec3(direction.x, 0, direction.z));
    if (state == GLFW_PRESS)
    {
        position += horizontalDir * horizontalSpeed;
    }
    state = glfwGetKey(window, GLFW_KEY_S);
    if (state == GLFW_PRESS)
    {
        position -= horizontalDir * horizontalSpeed;
    }
    state = glfwGetKey(window, GLFW_KEY_A);
    if (state == GLFW_PRESS)
    {
        position -= camRight * horizontalSpeed;
    }
    state = glfwGetKey(window, GLFW_KEY_D);
    if (state == GLFW_PRESS)
    {
        position += camRight * horizontalSpeed;
    }

    float verticalSpeed = 40.f * deltaTime;
    state = glfwGetKey(window, GLFW_KEY_SPACE);
    if (state == GLFW_PRESS)
    {
        position += camUp * verticalSpeed;
    }
    state = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
    if (state == GLFW_PRESS)
    {
        position -= camUp * verticalSpeed;
    }

}