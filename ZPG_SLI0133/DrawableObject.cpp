#include "DrawableObject.h"
#include "Model.h"
#include "ShaderProgram.h"
#include "TransformationComposite.h" // Tento include je klíèový
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

DrawableObject::DrawableObject(std::unique_ptr<Model> m, std::shared_ptr<ShaderProgram> s)
    : model(std::move(m)), shaderProgram(std::move(s)) {
    transformation = std::make_unique<TransformationComposite>();
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

TransformationComposite& DrawableObject::getTransformation() {
    return *transformation;
}

// --- PØIDEJTE TENTO PRÁZDNÝ DESTRUKTOR ---
DrawableObject::~DrawableObject() {
    // Tento destruktor je zámìrnì definován zde, i když je prázdný.
    // Tím se zajistí, že std::unique_ptr<TransformationComposite>
    // bude smazán až zde, kde je TransformationComposite plnì definován
    // (díky #include "TransformationComposite.h" na zaèátku souboru).
}