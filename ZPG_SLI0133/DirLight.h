#pragma once
#include "Light.h"

class DirLight : public Light {
public:
    glm::vec3 direction;

    DirLight(const glm::vec3& dir, const glm::vec3& col)
        : Light(col), direction(dir) {
    }
};