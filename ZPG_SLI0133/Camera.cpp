#include "Camera.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <glm/gtc/constants.hpp>

Camera::Camera(glm::vec3 startPosition)
    : position(startPosition),
    up(glm::vec3(0.0f, 1.0f, 0.0f)),
    target(glm::vec3(0.0f, 0.0f, -1.0f)),
    alpha(glm::radians(PITCH_DEFAULT)),
    fi(glm::radians(YAW_DEFAULT)),
    fov(glm::radians(45.0f)),
    nearPlane(0.1f),
    farPlane(100.0f),
    aspectRatio(4.0f / 3.0f)
{
    updateMatrices();
}

void Camera::attach(ICameraObserver* observer) {
    if (observer) {
        observers.push_back(observer);
    }
}

void Camera::detach(ICameraObserver* observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void Camera::notifyObservers() const {
    for (ICameraObserver* observer : observers) {
        observer->update(const_cast<Camera*>(this));
    }
}

void Camera::updateMatrices() {
    target.x = cos(fi) * cos(alpha);
    target.y = sin(alpha);
    target.z = sin(fi) * cos(alpha);
    target = glm::normalize(target);
    viewMatrix = glm::lookAt(position, position + target, up);

    projectionMatrix = glm::perspective(fov, aspectRatio, nearPlane, farPlane);

    notifyObservers();
}

void Camera::setAspectRatio(float width, float height) {
    if (height > 0.001f) {
        aspectRatio = width / height;
    }
}
