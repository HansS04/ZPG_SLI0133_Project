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

extern float rotationSpeed;
extern float rotationAngle;
extern bool direction;

Scene::Scene() : m_FireflyTime(0.0f)
{
    pointLight = std::make_unique<Light>();
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

    if (pointLight && colorShaderProgram) {
        pointLight->attach(colorShaderProgram.get());

        colorShaderProgram->use();
        colorShaderProgram->setLightUniforms(*pointLight);
    }
}

void Scene::addObject(const float* data, size_t size, const glm::vec4& color) {
    std::unique_ptr<Model> m = std::make_unique<Model>(data, size);
    std::unique_ptr<DrawableObject> obj = std::make_unique<DrawableObject>(std::move(m), colorShaderProgram);
    obj->setColor(color);
    objects.push_back(std::move(obj));
}

void Scene::clearObjects() {
    objects.clear();
    m_AdditionalLights.clear();
    m_AdditionalLightBasePositions.clear();
}


void Scene::render() const {
    if (!camera) return;

    if (colorShaderProgram) {
        colorShaderProgram->use();
        colorShaderProgram->setAdditionalLights(m_AdditionalLights);
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

        for (size_t i = 0; i < m_AdditionalLights.size(); ++i) {
            glm::vec3 basePos = m_AdditionalLightBasePositions[i];

            glm::vec3 offset;
            offset.x = sin(m_FireflyTime + i * 2.1f) * 0.5f;
            offset.y = cos(m_FireflyTime + i * 1.7f) * 0.3f;
            offset.z = sin(m_FireflyTime + i * 0.8f) * 0.5f;

            m_AdditionalLights[i]->setPosition(basePos + offset);
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

Light* Scene::addAdditionalLight(const glm::vec3& pos, const glm::vec3& col, float constant, float linear, float quadratic) {
    auto newLight = std::make_unique<Light>(pos, col, constant, linear, quadratic);
    m_AdditionalLights.emplace_back(std::move(newLight));
    m_AdditionalLightBasePositions.push_back(pos);
    return m_AdditionalLights.back().get();
}