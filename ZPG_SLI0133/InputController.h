#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Application.h" // <-- Musí includovat plnou definici
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>

class Camera;

// Enum byl pøesunut z CameraController.h
enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class InputController {
private:
    Application& m_App; // <-- Reference na Application
    GLFWwindow* m_Window;

    // Stav myši (pøesunuto z Application)
    bool m_RightButtonPressed = false;
    bool m_FirstMouse = true;
    float m_LastX;
    float m_LastY;

    // Citlivost myši
    const float MOUSE_SENSITIVITY = 0.1f;

    // Soukromé metody pro ovládání kamery
    void processCameraKeyboard(CameraMovement direction, float velocity);
    void processCameraMouse(float xoffset, float yoffset, bool constrainPitch);
    void processCameraScroll(float yoffset);

public:
    InputController(Application& app, GLFWwindow* window, float initialX, float initialY);

    // Metoda pro zpracování "poll" vstupù (držení kláves)
    void processPollingInput(float deltaTime);

    // Èlenské metody, které budou volány adaptéry (pro eventy)
    void onKey(int key, int scancode, int action, int mods);
    void onMouseButton(int button, int action, int mods);
    void onCursorPos(double xpos, double ypos);
    void onScroll(double xoffset, double yoffset);

    // ŽÁDNÉ statické adaptéry zde
};