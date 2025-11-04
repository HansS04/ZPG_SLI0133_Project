#pragma once
#include <glm/glm.hpp>

class Light {
public:
    glm::vec3 color;

    Light(const glm::vec3& col) : color(col) {}
    virtual ~Light() = default;
};