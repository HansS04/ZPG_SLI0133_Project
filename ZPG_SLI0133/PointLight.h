#pragma once
#include "LIght.h"

class PointLight : public Light {
public:
    glm::vec3 position;
    float constant;
    float linear;
    float quadratic;

    PointLight(const glm::vec3& pos, const glm::vec3& col, float con = 1.0f, float lin = 0.09f, float quad = 0.032f)
        : Light(col), position(pos), constant(con), linear(lin), quadratic(quad) {
    }
};