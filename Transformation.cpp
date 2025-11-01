#include "Transformation.h"
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

RotationTransformation::RotationTransformation(float angle, const glm::vec3& axis)
    : angle(angle), axis(axis) {
}

glm::mat4 RotationTransformation::apply(const glm::mat4& matrix) const {
    return glm::rotate(matrix, angle, axis);
}

TranslationTransformation::TranslationTransformation(const glm::vec3& translation)
    : translationVector(translation) {
}

glm::mat4 TranslationTransformation::apply(const glm::mat4& matrix) const {
    return glm::translate(matrix, translationVector);
}

ScaleTransformation::ScaleTransformation(const glm::vec3& scaleVector)
    : scaleVector(scaleVector) {
}

glm::mat4 ScaleTransformation::apply(const glm::mat4& matrix) const {
    return glm::scale(matrix, scaleVector);
}

Transformation::Transformation() {
}

glm::mat4 Transformation::calculateMatrix() const {
    glm::mat4 currentMatrix(1.0f);

    for (const auto& component : components) {
        currentMatrix = component->apply(currentMatrix);
    }

    return currentMatrix;
}

glm::mat4 Transformation::getMatrix() const {
    return calculateMatrix();
}

void Transformation::reset() {
    components.clear();
}

Transformation& Transformation::rotate(float angle, const glm::vec3& axis) {
    components.push_back(std::make_unique<RotationTransformation>(angle, axis));
    return *this;
}

Transformation& Transformation::translate(const glm::vec3& translation) {
    components.push_back(std::make_unique<TranslationTransformation>(translation));
    return *this;
}

Transformation& Transformation::scale(const glm::vec3& scaleVector) {
    components.push_back(std::make_unique<ScaleTransformation>(scaleVector));
    return *this;
}