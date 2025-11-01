#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include <algorithm>

class BasicTransformation {
public:
    virtual glm::mat4 apply(const glm::mat4& matrix) const = 0;
    virtual ~BasicTransformation() = default;
};

class RotationTransformation : public BasicTransformation {
private:
    float angle;
    glm::vec3 axis;
public:
    RotationTransformation(float angle, const glm::vec3& axis);
    glm::mat4 apply(const glm::mat4& matrix) const override;
};

class TranslationTransformation : public BasicTransformation {
private:
    glm::vec3 translationVector;
public:
    TranslationTransformation(const glm::vec3& translation);
    glm::mat4 apply(const glm::mat4& matrix) const override;
};

class ScaleTransformation : public BasicTransformation {
private:
    glm::vec3 scaleVector;
public:
    ScaleTransformation(const glm::vec3& scaleVector);
    glm::mat4 apply(const glm::mat4& matrix) const override;
};

class Transformation {
private:
    std::vector<std::unique_ptr<BasicTransformation>> components;

    glm::mat4 calculateMatrix() const;

public:
    Transformation();

    glm::mat4 getMatrix() const;
    void reset();

    Transformation& rotate(float angle, const glm::vec3& axis);
    Transformation& translate(const glm::vec3& translation);
    Transformation& scale(const glm::vec3& scaleVector);
};