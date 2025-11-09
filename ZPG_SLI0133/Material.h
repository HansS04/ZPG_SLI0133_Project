#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    GLuint diffuseTextureID;

    Material() :
        ambient(1.0f, 1.0f, 1.0f),
        diffuse(1.0f, 1.0f, 1.0f),
        specular(1.0f, 1.0f, 1.0f),
        shininess(32.0f),
        diffuseTextureID(0) {
    }
};