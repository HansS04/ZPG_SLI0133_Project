#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <vector>
#include <algorithm>
#include "ICameraObserver.h" 

constexpr float YAW_DEFAULT = -90.0f;
constexpr float PITCH_DEFAULT = 0.0f;

class Camera {
private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    glm::vec3 right;

    float alpha;
    float fi;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    std::vector<ICameraObserver*> observers;

    float fov;
    float nearPlane;
    float farPlane;
    float aspectRatio;

    void notifyObservers() const;

public:
    Camera(glm::vec3 startPosition);

    void attach(ICameraObserver* observer);
    void detach(ICameraObserver* observer);
    void updateMatrices();

    void setPosition(const glm::vec3& newPos) { position = newPos; }
    const glm::vec3& getPosition() const { return position; }

    const glm::vec3& getFrontVector() const { return target; }
    const glm::vec3& getRightVector() const { return right; }
    const glm::vec3& getUpVector() const { return up; }

    void setAlpha(float newAlpha) { alpha = newAlpha; }
    float getAlpha() const { return alpha; }

    void setFi(float newFi) { fi = newFi; }
    float getFi() const { return fi; }

    void setFOV(float newFov) { fov = newFov; }
    float getFOV() const { return fov; }

    void setAspectRatio(float width, float height);

    const glm::mat4& getViewMatrix() const { return viewMatrix; }
    const glm::mat4& getProjectionMatrix() const { return projectionMatrix; }
};