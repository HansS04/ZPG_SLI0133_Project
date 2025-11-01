#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include "ILightObserver.h" 

class Light {
private:
    glm::vec3 position;
    glm::vec3 color;

    float constant;
    float linear;
    float quadratic;

    std::vector<ILightObserver*> m_Observers;
    void notify() const;

public:
    Light(const glm::vec3& pos = glm::vec3(0.0f, 2.5f, -9.0f),
        const glm::vec3& col = glm::vec3(1.0f, 1.0f, 1.0f),
        float con = 1.0f,
        float lin = 0.09f,
        float quad = 0.032f);

    const glm::vec3& getPosition() const { return position; }
    const glm::vec3& getColor() const { return color; }
    float getConstant() const { return constant; }
    float getLinear() const { return linear; }
    float getQuadratic() const { return quadratic; }

    void setPosition(const glm::vec3& pos);
    void setColor(const glm::vec3& col);

    void attach(ILightObserver* observer);
    void detach(ILightObserver* observer);
}; 