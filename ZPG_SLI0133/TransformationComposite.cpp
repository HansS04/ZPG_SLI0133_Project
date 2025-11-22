#include "TransformationComposite.h"
#include "TransformationComponent.h"
#include "TransformationLeafs.h"

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

TransformationComposite::TransformationComposite() {
}

TransformationComposite::~TransformationComposite() {
}

glm::mat4 TransformationComposite::calculateMatrix() const {
    glm::mat4 currentMatrix(1.0f);
    for (const auto& component : components) {
        currentMatrix = component->apply(currentMatrix);
    }
    return currentMatrix;
}

glm::mat4 TransformationComposite::getMatrix() const {
    return calculateMatrix();
}

void TransformationComposite::reset() {
    components.clear();
}


TransformationComposite& TransformationComposite::rotate(float angle, const glm::vec3& axis) {
    components.push_back(std::make_unique<RotationTransformation>(angle, axis));
    return *this;
}

TransformationComposite& TransformationComposite::translate(const glm::vec3& translation) {
    components.push_back(std::make_unique<TranslationTransformation>(translation));
    return *this;
}

TransformationComposite& TransformationComposite::scale(const glm::vec3& scaleVector) {
    components.push_back(std::make_unique<ScaleTransformation>(scaleVector));
    return *this;
}
// Pøidejte do TransformationComposite.cpp
TransformationComposite& TransformationComposite::addMatrix20() {
    // Pøidáme naši speciální transformaci do seznamu
    components.push_back(std::make_unique<Matrix20Transformation>());
    return *this;
}