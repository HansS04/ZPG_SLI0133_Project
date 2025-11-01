#include "TransformationComposite.h"
#include "TransformationComponent.h"
#include "TransformationLeafs.h" // Potøebujeme pro vytváøení nových listù

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

TransformationComposite::TransformationComposite() {
    // Konstruktor
}

// Destruktor je nutné definovat v .cpp souboru,
// aby unique_ptr vìdìl, jak smazat dopøednì deklarované typy.
TransformationComposite::~TransformationComposite() {
    // Destruktor
}

glm::mat4 TransformationComposite::calculateMatrix() const {
    glm::mat4 currentMatrix(1.0f);
    // Aplikujeme transformace ve stejném poøadí, v jakém byly pøidány
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

// --- Builder metody ---

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