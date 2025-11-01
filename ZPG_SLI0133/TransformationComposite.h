#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>

class TransformationComponent; 

class TransformationComposite {
private:
    std::vector<std::unique_ptr<TransformationComponent>> components;
    glm::mat4 calculateMatrix() const;

public:
    TransformationComposite();
    ~TransformationComposite();

    glm::mat4 getMatrix() const;
    void reset();

    TransformationComposite& rotate(float angle, const glm::vec3& axis);
    TransformationComposite& translate(const glm::vec3& translation);
    TransformationComposite& scale(const glm::vec3& scaleVector);
};