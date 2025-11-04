#pragma once
#include "Light.h"

class SpotLight : public Light {
public:
    glm::vec3 position;
    glm::vec3 direction;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;

    SpotLight(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& col,
        float con, float lin, float quad, float cut, float outerCut)
        : Light(col), position(pos), direction(dir),
        constant(con), linear(lin), quadratic(quad),
        cutOff(cut), outerCutOff(outerCut) {
    }
};