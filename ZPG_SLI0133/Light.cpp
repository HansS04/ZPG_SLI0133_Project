#include "Light.h"

Light::Light(const glm::vec3& pos, const glm::vec3& col, float con, float lin, float quad)
    : position(pos), color(col), constant(con), linear(lin), quadratic(quad)
{
}