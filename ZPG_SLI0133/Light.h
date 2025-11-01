#pragma once
#include <glm/glm.hpp>

/**
 * @brief Smìrové svìtlo (Slunce, Mìsíc). Nemá pozici, jen smìr.
 */
struct DirLight {
    glm::vec3 direction;
    glm::vec3 color;
};

/**
 * @brief Bodové svìtlo (žárovka, svìtluška). Má pozici a útlum.
 * Toto je vaše pùvodní tøída Light, pøejmenovaná a zjednodušená.
 */
class PointLight {
public:
    glm::vec3 position;
    glm::vec3 color;

    float constant;
    float linear;
    float quadratic;

    PointLight(const glm::vec3& pos, const glm::vec3& col, float con = 1.0f, float lin = 0.09f, float quad = 0.032f)
        : position(pos), color(col), constant(con), linear(lin), quadratic(quad) {
    }
};

/**
 * @brief Reflektor (baterka). Je to bodové svìtlo s kuželem.
 */
struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;

    float constant;
    float linear;
    float quadratic;

    float cutOff; // Vnitøní úhel (cos)
    float outerCutOff; // Vnìjší úhel (cos)
};