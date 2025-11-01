#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "Model.h"          
#include "ShaderProgram.h"  

class Model;
class ShaderProgram;
class TransformationComposite; // Dopøedná deklarace

class DrawableObject {
private:
    std::unique_ptr<Model> model;
    std::unique_ptr<TransformationComposite> transformation;
    std::shared_ptr<ShaderProgram> shaderProgram;
    glm::vec4 color;

public:
    DrawableObject(std::unique_ptr<Model> m, std::shared_ptr<ShaderProgram> s);
    ~DrawableObject(); // <-- PØIDEJTE TENTO ØÁDEK

    void draw() const;
    TransformationComposite& getTransformation();

    void setColor(const glm::vec4& c) { color = c; }
    const glm::vec4& getColor() const { return color; }
};