#include "DrawableObject.h"
#include "Model.h"
#include "ShaderProgram.h"
#include "Transformation.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

DrawableObject::DrawableObject(std::unique_ptr<Model> m, std::shared_ptr<ShaderProgram> s)
    : model(std::move(m)), shaderProgram(std::move(s)) {
    transformation = std::make_unique<Transformation>();
}

void DrawableObject::draw() const {
    if (!shaderProgram) {
        std::cerr << "Chyba: ShaderProgram neni inicializovan pro DrawableObject." << std::endl;
        return;
    }

    shaderProgram->use();

    glm::mat4 modelMatrix = transformation->getMatrix();
    shaderProgram->setMat4("u_ModelMatrix", modelMatrix);
    shaderProgram->setMat4("u_ViewMatrix", shaderProgram->getViewMatrix());
    shaderProgram->setMat4("u_ProjectionMatrix", shaderProgram->getProjectionMatrix());

    shaderProgram->setVec4("u_Color", color);

    model->draw();
}

Transformation& DrawableObject::getTransformation() {
    return *transformation;
}