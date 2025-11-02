#pragma once
#include <glm/glm.hpp>

/**
 * @brief Struktura obsahující všechny koeficienty pro Phongùv osvìtlovací model.
 * (ra, rd, rs, h)
 */
struct Material {
    glm::vec3 ambient;  // ra: Barva okolního odrazu
    glm::vec3 diffuse;  // rd: Základní barva (difuzní)
    glm::vec3 specular; // rs: Barva odlesku
    float shininess;  // h: Lesklost

    Material() :
        ambient(1.0f, 1.0f, 1.0f),
        diffuse(1.0f, 1.0f, 1.0f),
        specular(1.0f, 1.0f, 1.0f),
        shininess(32.0f) {
    }
};