#pragma once
#include "TransformationComponent.h"
#include <glm/vec3.hpp>


class RotationTransformation : public TransformationComponent {
private:
    float angle;
    glm::vec3 axis;
public:
    RotationTransformation(float angle, const glm::vec3& axis);
    glm::mat4 apply(const glm::mat4& matrix) const override;
};


class TranslationTransformation : public TransformationComponent {
private:
    glm::vec3 translationVector;
public:
    TranslationTransformation(const glm::vec3& translation);
    glm::mat4 apply(const glm::mat4& matrix) const override;
};


class ScaleTransformation : public TransformationComponent {
private:
    glm::vec3 scaleVector;
public:
    ScaleTransformation(const glm::vec3& scaleVector);
    glm::mat4 apply(const glm::mat4& matrix) const override;
};

class Matrix20Transformation : public TransformationComponent {
public:
    glm::mat4 apply(const glm::mat4& matrix) const override;
};