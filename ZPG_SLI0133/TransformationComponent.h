#pragma once
#include <glm/glm.hpp>

/**
 * @brief Základní rozhraní pro všechny transformaèní komponenty.
 * Toto je "Component" v návrhovém vzoru Kompozit.
 * Døíve se jmenoval "BasicTransformation".
 */
class TransformationComponent {
public:
    virtual glm::mat4 apply(const glm::mat4& matrix) const = 0;
    virtual ~TransformationComponent() = default;
};