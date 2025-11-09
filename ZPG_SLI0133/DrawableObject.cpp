#include "DrawableObject.h"
#include "Model.h"
#include "ShaderProgram.h"
#include "TransformationComposite.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

DrawableObject::DrawableObject(std::unique_ptr<Model> m, std::shared_ptr<ShaderProgram> s)
    : model(std::move(m)), shaderProgram(std::move(s)) {
    transformation = std::make_unique<TransformationComposite>();
    m_Material = std::make_shared<Material>();
}

void DrawableObject::draw() const {
    if (!shaderProgram) {
        std::cerr << "ERROR: ShaderProgram in not initialized for DrawableObject." << std::endl;
        return;
    }
    if (!m_Material) {
        std::cerr << "ERROR: Material neni nastaven pro DrawableObject." << std::endl;
        return;
    }

    shaderProgram->use();
    shaderProgram->setBool("u_IsUnlit", this->isUnlit);

    shaderProgram->setMaterial(*m_Material);

    if (m_Material->diffuseTextureID != 0) {
        shaderProgram->setBool("u_HasDiffuseTexture", true);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_Material->diffuseTextureID);
    }
    else {
        shaderProgram->setBool("u_HasDiffuseTexture", false);
    }

    glm::mat4 modelMatrix = transformation->getMatrix();
    shaderProgram->setMat4("u_ModelMatrix", modelMatrix);
    shaderProgram->setMat4("u_ViewMatrix", shaderProgram->getViewMatrix());
    shaderProgram->setMat4("u_ProjectionMatrix", shaderProgram->getProjectionMatrix());

    model->draw();

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

TransformationComposite& DrawableObject::getTransformation() {
    return *transformation;
}

DrawableObject::~DrawableObject() {
}