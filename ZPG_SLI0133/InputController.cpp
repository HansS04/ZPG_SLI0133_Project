#include "InputController.h"
#include "Scene.h"  
#include "Camera.h" 
#include <glm/gtc/constants.hpp>
#include <iostream>

InputController::InputController(Application& app, GLFWwindow* window, float initialX, float initialY)
    : m_App(app), m_Window(window), m_LastX(initialX), m_LastY(initialY) {
}


void InputController::processPollingInput(float deltaTime) {
    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_Window, true);

    Scene* scene = m_App.getActiveScene();
    if (!scene) return;

    const float cameraSpeed = 5.0f * deltaTime;
    if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
        processCameraKeyboard(CameraMovement::FORWARD, cameraSpeed);
    if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
        processCameraKeyboard(CameraMovement::BACKWARD, cameraSpeed);
    if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
        processCameraKeyboard(CameraMovement::LEFT, cameraSpeed);
    if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
        processCameraKeyboard(CameraMovement::RIGHT, cameraSpeed);

    if (glfwGetKey(m_Window, GLFW_KEY_1) == GLFW_PRESS) m_App.loadScene(1);
    if (glfwGetKey(m_Window, GLFW_KEY_2) == GLFW_PRESS) m_App.loadScene(2);
    if (glfwGetKey(m_Window, GLFW_KEY_3) == GLFW_PRESS) m_App.loadScene(3);
    if (glfwGetKey(m_Window, GLFW_KEY_0) == GLFW_PRESS) m_App.loadScene(0);
    if (glfwGetKey(m_Window, GLFW_KEY_4) == GLFW_PRESS) m_App.loadScene(4); // <-- PØIDEJTE TENTO ØÁDEK
}

void InputController::onKey(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        Scene* scene = m_App.getActiveScene();
        if (scene) {
            scene->toggleFlashlight();
        }
    }
}

void InputController::onMouseButton(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            m_RightButtonPressed = true;
            glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else if (action == GLFW_RELEASE) {
            m_RightButtonPressed = false;
            m_FirstMouse = true;
            glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void InputController::onCursorPos(double xpos, double ypos) {
    if (m_FirstMouse) {
        m_LastX = (float)xpos;
        m_LastY = (float)ypos;
        m_FirstMouse = false;
    }

    float xoffset = (float)xpos - m_LastX;
    float yoffset = (float)ypos - m_LastY;

    m_LastX = (float)xpos;
    m_LastY = (float)ypos;

    if (m_RightButtonPressed) {
        processCameraMouse(xoffset, yoffset, true);
    }
}

void InputController::onScroll(double xoffset, double yoffset) {
    processCameraScroll((float)yoffset);
}

void InputController::processCameraMouse(float xoffset, float yoffset, bool constrainPitch) {
    Scene* scene = m_App.getActiveScene();
    if (!scene) return;
    Camera& camera = scene->getCamera();

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

void InputController::processCameraScroll(float yoffset) {
    Scene* scene = m_App.getActiveScene();
    if (!scene) return;
    Camera& camera = scene->getCamera();

    float fov = camera.getFOV();
    if (yoffset != 0) {
        fov -= glm::radians(yoffset * 1.0f);
        fov = std::min(std::max(fov, glm::radians(1.0f)), glm::radians(90.0f));
    }

    camera.setFOV(fov);
    camera.updateMatrices();
}

void InputController::processCameraKeyboard(CameraMovement direction, float velocity) {
    Scene* scene = m_App.getActiveScene();
    if (!scene) return;
    Camera& camera = scene->getCamera();

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
    case CameraMovement::FORWARD: position += target * velocity; break;
    case CameraMovement::BACKWARD: position -= target * velocity; break;
    case CameraMovement::LEFT: position -= right * velocity; break;
    case CameraMovement::RIGHT: position += right * velocity; break;
    }

    camera.setPosition(position);
    camera.updateMatrices();
}