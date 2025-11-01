#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>

class TransformationComponent; // Dopøedná deklarace

/**
 * @brief Skládá více transformací dohromady.
 * Toto je "Composite" v návrhovém vzoru Kompozit.
 * Døíve se jmenoval "Transformation".
 */
class TransformationComposite {
private:
    std::vector<std::unique_ptr<TransformationComponent>> components;
    glm::mat4 calculateMatrix() const;

public:
    TransformationComposite();
    ~TransformationComposite(); // Pøidáno pro správnou správu unique_ptr s dopøednou deklarací

    glm::mat4 getMatrix() const;
    void reset();

    // Fluent interface (Builder pattern)
    TransformationComposite& rotate(float angle, const glm::vec3& axis);
    TransformationComposite& translate(const glm::vec3& translation);
    TransformationComposite& scale(const glm::vec3& scaleVector);
};