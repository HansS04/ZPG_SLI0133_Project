#include "Light.h"

Light::Light(const glm::vec3& pos, const glm::vec3& col, float con, float lin, float quad)
    : position(pos), color(col), constant(con), linear(lin), quadratic(quad)
{
    // m_Observers je automaticky prázdný
}

void Light::setPosition(const glm::vec3& pos) {
    if (position != pos) {
        position = pos;
        notify(); // Informuj pozorovatele o zmìnì
    }
}

void Light::setColor(const glm::vec3& col) {
    if (color != col) {
        color = col;
        notify(); // Informuj pozorovatele o zmìnì
    }
}

void Light::attach(ILightObserver* observer) {
    if (observer) {
        m_Observers.push_back(observer);
    }
}

void Light::detach(ILightObserver* observer) {
    m_Observers.erase(
        std::remove(m_Observers.begin(), m_Observers.end(), observer),
        m_Observers.end()
    );
}

void Light::notify() const {
    // Projdi všechny pozorovatele a zavolej jejich metodu
    for (ILightObserver* observer : m_Observers) {
        observer->onLightChanged(this);
    }
}