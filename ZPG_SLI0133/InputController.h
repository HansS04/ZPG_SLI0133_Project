#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Application.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>

class Camera;

enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class InputController {
private:
    Application& m_App;
    GLFWwindow* m_Window;

    bool m_RightButtonPressed = false;
    bool m_FirstMouse = true;
    float m_LastX;
    float m_LastY;

    const float MOUSE_SENSITIVITY = 0.1f;

    void processCameraKeyboard(CameraMovement direction, float velocity);
    void processCameraMouse(float xoffset, float yoffset, bool constrainPitch);
    void processCameraScroll(float yoffset);

public:
    InputController(Application& app, GLFWwindow* window, float initialX, float initialY);

    void processPollingInput(float deltaTime);

    void onKey(int key, int scancode, int action, int mods);
    void onMouseButton(int button, int action, int mods);
    void onCursorPos(double xpos, double ypos);
    void onScroll(double xoffset, double yoffset);

};