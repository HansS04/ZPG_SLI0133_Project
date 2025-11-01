#pragma once
#include <glm/glm.hpp>


class TransformationComponent {
public:
    virtual glm::mat4 apply(const glm::mat4& matrix) const = 0;
    virtual ~TransformationComponent() = default;
};