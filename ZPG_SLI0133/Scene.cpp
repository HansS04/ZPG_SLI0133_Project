#include "Scene.h"
#include "DrawableObject.h"
#include "ShaderProgram.h"
#include "Model.h"
#include "TransformationComposite.h"
#include "Camera.h"
#include "Shader.h" 
#include "Light.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <stdexcept>
#include <algorithm> 
#include <glm/gtc/constants.hpp>

extern float rotationSpeed;
extern float rotationAngle;
extern bool direction;

Scene::Scene()
    : m_FireflyTime(0.0f),
    m_AmbientLightColor(0.1f, 0.1f, 0.1f),
    m_DirLightOn(false),
    m_FlashlightOn(false)
{
    m_DirLight = std::make_unique<DirLight>();
    m_DirLight->direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    m_DirLight->color = glm::vec3(1.0f, 1.0f, 1.0f);

    m_Flashlight = std::make_unique<SpotLight>();
    m_Flashlight->color = glm::vec3(1.0f, 1.0f, 1.0f);
    m_Flashlight->constant = 1.0f;
    m_Flashlight->linear = 0.09f;
    m_Flashlight->quadratic = 0.032f;
    m_Flashlight->cutOff = glm::cos(glm::radians(12.5f));
    m_Flashlight->outerCutOff = glm::cos(glm::radians(15.5f));
}

void Scene::createShaders(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) {

    Shader vs(GL_VERTEX_SHADER, vertexShaderFile.c_str());
    Shader fs(GL_FRAGMENT_SHADER, fragmentShaderFile.c_str());

    colorShaderProgram = std::make_shared<ShaderProgram>(vs, fs);


    int width = 1024, height = 768;

    camera = std::make_unique<Camera>(glm::vec3(0.0f, 1.0f, 3.0f));

    if (height > 0) {
        camera->setAspectRatio((float)width, (float)height);
    }

    camera->attach(colorShaderProgram.get());
    camera->updateMatrices();
}

void Scene::addObject(const float* data, size_t size, const glm::vec4& color) {
    std::unique_ptr<Model> m = std::make_unique<Model>(data, size);
    std::unique_ptr<DrawableObject> obj = std::make_unique<DrawableObject>(std::move(m), colorShaderProgram);
    obj->setColor(color);
    objects.push_back(std::move(obj));
}

void Scene::clearObjects() {
    objects.clear();
    m_PointLights.clear();
    m_PointLightBasePositions.clear();
}


void Scene::render() const {
    if (!camera) return;

    if (colorShaderProgram) {
        colorShaderProgram->use();

        colorShaderProgram->setVec3("u_ViewPos", camera->getPosition());

        colorShaderProgram->setAmbientLight(m_AmbientLightColor);
        colorShaderProgram->setDirLight(*m_DirLight, m_DirLightOn);
        colorShaderProgram->setPointLights(m_PointLights);
        colorShaderProgram->setFlashlight(*m_Flashlight, m_FlashlightOn);
    }

    for (const auto& obj : objects) {
        obj->draw();
    }
}

extern float earthOrbitAngle;
extern float moonOrbitAngle;
extern float SCALE_EARTH;
extern float SCALE_MOON;
extern float EARTH_SUN_DISTANCE;
extern float MOON_EARTH_DISTANCE;

void Scene::update(float deltaTime, int currentSceneIndex) {

    if (m_FlashlightOn) {
        const float rightOffset = 0.15f;
        const float upOffset = 0.05f;

        glm::vec3 camPos = camera->getPosition();
        glm::vec3 camFront = camera->getFrontVector();
        glm::vec3 camRight = camera->getRightVector();
        glm::vec3 camUp = camera->getUpVector();

        m_Flashlight->position = camPos + (camRight * rightOffset) + (camUp * upOffset);
        m_Flashlight->direction = camFront;
    }

    if (currentSceneIndex == 2) {
        const float EARTH_ORBIT_SPEED_RAD = 0.5f;
        const float MOON_ORBIT_SPEED_RAD = 2.0f;

        if (objects.size() < 3) return;
        DrawableObject* earth = objects[1].get();
        DrawableObject* moon = objects[2].get();
        if (!earth || !moon) return;

        earthOrbitAngle += EARTH_ORBIT_SPEED_RAD * deltaTime;
        moonOrbitAngle += MOON_ORBIT_SPEED_RAD * deltaTime;

        earth->getTransformation().reset();
        earth->getTransformation().rotate(earthOrbitAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        earth->getTransformation().translate(glm::vec3(EARTH_SUN_DISTANCE, 0.0f, 0.0f));
        earth->getTransformation().scale(glm::vec3(SCALE_EARTH));

        moon->getTransformation().reset();
        moon->getTransformation().rotate(earthOrbitAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        moon->getTransformation().translate(glm::vec3(EARTH_SUN_DISTANCE, 0.0f, 0.0f));
        moon->getTransformation().rotate(moonOrbitAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        moon->getTransformation().translate(glm::vec3(MOON_EARTH_DISTANCE, 0.0f, 0.0f));
        moon->getTransformation().scale(glm::vec3(SCALE_MOON));
    }

    if (currentSceneIndex == 3) {
        m_FireflyTime += deltaTime * 0.7f;

        for (size_t i = 0; i < m_PointLights.size(); ++i) {
            glm::vec3 basePos = m_PointLightBasePositions[i];

            glm::vec3 offset;
            offset.x = sin(m_FireflyTime + i * 2.1f) * 0.5f;
            offset.y = cos(m_FireflyTime + i * 1.7f) * 0.3f;
            offset.z = sin(m_FireflyTime + i * 0.8f) * 0.5f;

            m_PointLights[i]->position = basePos + offset;
        }
    }
}

DrawableObject* Scene::getFirstObject() {
    if (objects.empty()) return nullptr;
    return objects.front().get();
}

DrawableObject* Scene::getObject(size_t index) {
    if (index >= objects.size()) return nullptr;
    return objects[index].get();
}

void Scene::setAmbientLight(const glm::vec3& color) {
    m_AmbientLightColor = color;
}

void Scene::setDirLight(const glm::vec3& direction, const glm::vec3& color, bool on) {
    m_DirLight->direction = direction;
    m_DirLight->color = color;
    m_DirLightOn = on;
}

PointLight* Scene::addPointLight(const glm::vec3& pos, const glm::vec3& col, float constant, float linear, float quadratic) {
    auto newLight = std::make_unique<PointLight>(pos, col, constant, linear, quadratic);
    m_PointLights.emplace_back(std::move(newLight));
    m_PointLightBasePositions.push_back(pos);
    return m_PointLights.back().get();
}

void Scene::toggleFlashlight() {
    m_FlashlightOn = !m_FlashlightOn;
    std::cout << "Baterka " << (m_FlashlightOn ? "ZAPNUTA" : "VYPNUTA") << std::endl;
}