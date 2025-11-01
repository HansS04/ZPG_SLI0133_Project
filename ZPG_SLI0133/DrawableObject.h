#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "Model.h"          
#include "ShaderProgram.h"  
#include "Transformation.h"

class Model;
class ShaderProgram;
class Transformation;

class DrawableObject {
private:
    std::unique_ptr<Model> model;
    std::unique_ptr<Transformation> transformation;
    std::shared_ptr<ShaderProgram> shaderProgram;
    glm::vec4 color;

public:
    DrawableObject(std::unique_ptr<Model> m, std::shared_ptr<ShaderProgram> s);
    void draw() const;
    Transformation& getTransformation();

    void setColor(const glm::vec4& c) { color = c; }
    const glm::vec4& getColor() const { return color; }
};