#pragma once
#include <glm/glm.hpp>

/**
 * @brief Abstraktní základní tøída pro všechny typy svìtel.
 */
class Light {
public:
    glm::vec3 color;

    Light(const glm::vec3& col) : color(col) {}
    virtual ~Light() = default;
};

/**
 * @brief Smìrové svìtlo (Slunce, Mìsíc). Dìdí z Light.
 */
class DirLight : public Light {
public:
    glm::vec3 direction;

    DirLight(const glm::vec3& dir, const glm::vec3& col)
        : Light(col), direction(dir) {
    }
};

/**
 * @brief Bodové svìtlo (žárovka, svìtluška). Dìdí z Light.
 * Toto byla vaše pùvodní tøída PointLight.
 */
class PointLight : public Light {
public:
    glm::vec3 position;
    float constant;
    float linear;
    float quadratic;

    PointLight(const glm::vec3& pos, const glm::vec3& col, float con = 1.0f, float lin = 0.09f, float quad = 0.032f)
        : Light(col), position(pos), constant(con), linear(lin), quadratic(quad) {
    }
};

/**
 * @brief Reflektor (baterka). Dìdí z Light.
 */
class SpotLight : public Light {
public:
    glm::vec3 position;
    glm::vec3 direction;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;

    SpotLight(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& col,
        float con, float lin, float quad, float cut, float outerCut)
        : Light(col), position(pos), direction(dir),
        constant(con), linear(lin), quadratic(quad),
        cutOff(cut), outerCutOff(outerCut) {
    }
};