#pragma once
#include "Camera.h" 
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>

class Camera;

constexpr float MOUSE_SENSITIVITY = 0.1f;

class CameraController {
private:
    Camera& camera;

public:
    enum class Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    CameraController(Camera& cam) : camera(cam) {}

    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch);
    void processScroll(float yoffset);
    void processKeyboard(Movement direction, float velocity);
};