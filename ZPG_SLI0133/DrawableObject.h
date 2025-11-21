#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "Model.h"          
#include "ShaderProgram.h"  
#include "Material.h" 

class Model;
class ShaderProgram;
class TransformationComposite;

class DrawableObject {
private:
    std::unique_ptr<Model> model;
    std::unique_ptr<TransformationComposite> transformation;
    std::shared_ptr<ShaderProgram> shaderProgram;

    std::shared_ptr<Material> m_Material;
    bool isUnlit = false;
    unsigned int m_ID = 0;

public:
    DrawableObject(std::unique_ptr<Model> m, std::shared_ptr<ShaderProgram> s);
    ~DrawableObject();

    void draw() const;
    TransformationComposite& getTransformation();

    void setMaterial(std::shared_ptr<Material> material) { m_Material = material; }
    void setUnlit(bool unlit) { isUnlit = unlit; }

    void setID(unsigned int id) { m_ID = id; }
    unsigned int getID() const { return m_ID; }
};