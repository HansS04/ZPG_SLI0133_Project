#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <algorithm> // Pro std::remove
#include "ILightObserver.h" // Zahrnutí rozhraní

class Light {
private:
    glm::vec3 position;
    glm::vec3 color;

    float constant;
    float linear;
    float quadratic;

    // Seznam pozorovatelù
    std::vector<ILightObserver*> m_Observers;

    // Soukromá metoda pro informování všech
    void notify() const;

public:
    Light(const glm::vec3& pos = glm::vec3(0.0f, 2.5f, -9.0f),
        const glm::vec3& col = glm::vec3(1.0f, 1.0f, 1.0f),
        float con = 1.0f,
        float lin = 0.09f,
        float quad = 0.032f);

    // Gettery
    const glm::vec3& getPosition() const { return position; }
    const glm::vec3& getColor() const { return color; }
    float getConstant() const { return constant; }
    float getLinear() const { return linear; }
    float getQuadratic() const { return quadratic; }

    // Settery (volají notify)
    void setPosition(const glm::vec3& pos);
    void setColor(const glm::vec3& col);

    // Metody pro správu pozorovatelù
    void attach(ILightObserver* observer);
    void detach(ILightObserver* observer);
};