#include "CameraController.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <algorithm>

void CameraController::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    float fi = camera.getFi();
    float alpha = camera.getAlpha();

    fi += glm::radians(xoffset * MOUSE_SENSITIVITY);
    alpha += glm::radians(-yoffset * MOUSE_SENSITIVITY);

    if (constrainPitch) {
        alpha = std::min(std::max(alpha, glm::radians(-89.0f)), glm::radians(89.0f));
    }

    camera.setFi(fi);
    camera.setAlpha(alpha);
    camera.updateMatrices();
}

void CameraController::processScroll(float yoffset) {
    float fov = camera.getFOV();
    if (yoffset != 0) {
        fov -= glm::radians(yoffset * 1.0f);
        fov = std::min(std::max(fov, glm::radians(1.0f)), glm::radians(90.0f));
    }

    camera.setFOV(fov);
    camera.updateMatrices();
}

void CameraController::processKeyboard(Movement direction, float velocity) {
    glm::vec3 position = camera.getPosition();

    float fi = camera.getFi();
    float alpha = camera.getAlpha();
    glm::vec3 target;
    target.x = cos(fi) * cos(alpha);
    target.y = sin(alpha);
    target.z = sin(fi) * cos(alpha);
    target = glm::normalize(target);

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(target, up));

    switch (direction) {
    case Movement::FORWARD: position += target * velocity; break;
    case Movement::BACKWARD: position -= target * velocity; break;
    case Movement::LEFT: position -= right * velocity; break;
    case Movement::RIGHT: position += right * velocity; break;
    }

    camera.setPosition(position);
    camera.updateMatrices();
}