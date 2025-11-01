#include "TransformationLeafs.h"
#include <glm/gtc/matrix_transform.hpp> 

// --- RotationTransformation ---
RotationTransformation::RotationTransformation(float angle, const glm::vec3& axis)
    : angle(angle), axis(axis) {
}

glm::mat4 RotationTransformation::apply(const glm::mat4& matrix) const {
    return glm::rotate(matrix, angle, axis);
}

// --- TranslationTransformation ---
TranslationTransformation::TranslationTransformation(const glm::vec3& translation)
    : translationVector(translation) {
}

glm::mat4 TranslationTransformation::apply(const glm::mat4& matrix) const {
    return glm::translate(matrix, translationVector);
}

// --- ScaleTransformation ---
ScaleTransformation::ScaleTransformation(const glm::vec3& scaleVector)
    : scaleVector(scaleVector) {
}

glm::mat4 ScaleTransformation::apply(const glm::mat4& matrix) const {
    return glm::scale(matrix, scaleVector);
}