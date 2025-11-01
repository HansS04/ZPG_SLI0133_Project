#include "Scene.h"
#include "DrawableObject.h"
#include "ShaderProgram.h"
#include "Model.h"
#include "Transformation.h"
#include "Camera.h"
// #include "CameraController.h" // <- Odstranìno
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

Scene::Scene()
{
    pointLight = std::make_unique<Light>();
}

void Scene::createShaders(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) {

    Shader vs(GL_VERTEX_SHADER, vertexShaderFile.c_str());
    // OPRAVA CHYBY 'c_st' na 'c_str()'
    Shader fs(GL_FRAGMENT_SHADER, fragmentShaderFile.c_str());

    colorShaderProgram = std::make_shared<ShaderProgram>(vs, fs);


    int width = 1024, height = 768; // Pevné rozlišení

    camera = std::make_unique<Camera>(glm::vec3(0.0f, 1.0f, 3.0f));

    // Odstranìna inicializace CameraController

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

void Scene::render() const {
    if (!camera) return;
    if (colorShaderProgram && pointLight) {
        colorShaderProgram->use();
        colorShaderProgram->setLightUniforms(*pointLight);
    }
    for (const auto& obj : objects) {
        obj->draw();
    }
}

// Externí promìnné pro Scénu 2
extern float earthOrbitAngle;
extern float moonOrbitAngle;
extern float SCALE_EARTH;
extern float SCALE_MOON;
extern float EARTH_SUN_DISTANCE;
extern float MOON_EARTH_DISTANCE;

void Scene::update(float deltaTime, int currentSceneIndex) {
    if (currentSceneIndex == 2) {
        // Použijeme globální promìnné pro nastavení scény 2
        // Místo lokálních statických
        const float EARTH_ORBIT_SPEED_RAD = 0.5f;
        const float MOON_ORBIT_SPEED_RAD = 2.0f;

        // Pøedpokládáme poøadí: 0=Slunce, 1=Zemì, 2=Mìsíc
        if (objects.size() < 3) return;

        DrawableObject* earth = objects[1].get();
        DrawableObject* moon = objects[2].get();
        if (!earth || !moon) return;

        // Aktualizujeme globální úhly
        earthOrbitAngle += EARTH_ORBIT_SPEED_RAD * deltaTime;
        moonOrbitAngle += MOON_ORBIT_SPEED_RAD * deltaTime;

        // Transformace Zemì
        earth->getTransformation().reset();
        earth->getTransformation().rotate(earthOrbitAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        earth->getTransformation().translate(glm::vec3(EARTH_SUN_DISTANCE, 0.0f, 0.0f));
        earth->getTransformation().scale(glm::vec3(SCALE_EARTH)); // Mìøítko Zemì

        // Transformace Mìsíce (složená)
        moon->getTransformation().reset();
        // 1. Obìh se Zemí kolem Slunce
        moon->getTransformation().rotate(earthOrbitAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        moon->getTransformation().translate(glm::vec3(EARTH_SUN_DISTANCE, 0.0f, 0.0f));
        // 2. Lokální obìh Mìsíce kolem Zemì
        moon->getTransformation().rotate(moonOrbitAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        moon->getTransformation().translate(glm::vec3(MOON_EARTH_DISTANCE, 0.0f, 0.0f));
        moon->getTransformation().scale(glm::vec3(SCALE_MOON)); // Mìøítko Mìsíce
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