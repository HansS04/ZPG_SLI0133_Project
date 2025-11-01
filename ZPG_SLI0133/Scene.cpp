#include "Scene.h"
#include "DrawableObject.h"
#include "ShaderProgram.h"
#include "Model.h"
#include "Transformation.h"
#include "TransformationComposite.h"
#include "Camera.h"
// #include "CameraController.h" // <- Správnì odstranìno
#include "Shader.h" 
#include "Light.h" 
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <stdexcept>
#include <algorithm> // Potøebné pro std::remove

// Externí promìnné (deklarace, definice jsou v Application.cpp)
extern float rotationSpeed;
extern float rotationAngle;
extern bool direction;

Scene::Scene()
{
    // Inicializace bodového svìtla s výchozími parametry
    pointLight = std::make_unique<Light>();
}

void Scene::createShaders(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) {

    Shader vs(GL_VERTEX_SHADER, vertexShaderFile.c_str());
    Shader fs(GL_FRAGMENT_SHADER, fragmentShaderFile.c_str());

    colorShaderProgram = std::make_shared<ShaderProgram>(vs, fs);


    int width = 1024, height = 768; // Pevné rozlišení

    camera = std::make_unique<Camera>(glm::vec3(0.0f, 1.0f, 3.0f));

    // cameraController již není potøeba

    if (height > 0) {
        camera->setAspectRatio((float)width, (float)height);
    }

    // Shader program se zaregistruje jako observer kamery
    camera->attach(colorShaderProgram.get());
    camera->updateMatrices();

    // --- NOVÁ LOGIKA PRO OBSERVER ---
    if (pointLight && colorShaderProgram) {
        // 1. Zaregistrujeme shader jako pozorovatele svìtla
        pointLight->attach(colorShaderProgram.get());

        // 2. Nastavíme poèáteèní stav svìtla v shaderu
        //    (protože attach() sám o sobì nevyvolá update)
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


void Scene::render() const {
    if (!camera) return;

    // -----------------------------------------------------------------
    // TENTO BLOK JE ODSTRANÌN DÍKY OBSERVER VZORU
    // Uniformy svìtla se nyní nastavují automaticky pouze pøi zmìnì.
    // -----------------------------------------------------------------
    /*
    if (colorShaderProgram && pointLight) {
        colorShaderProgram->use();
        colorShaderProgram->setLightUniforms(*pointLight);
    }
    */

    for (const auto& obj : objects) {
        // obj->draw() uvnitø volá shaderProgram->use()
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

        // Konstanty pro rychlost obìhu
        const float EARTH_ORBIT_SPEED_RAD = 0.5f; // Radiany za sekundu
        const float MOON_ORBIT_SPEED_RAD = 2.0f;  // Radiany za sekundu

        // Pøedpokládáme poøadí: 0=Slunce, 1=Zemì, 2=Mìsíc
        if (objects.size() < 3) return; // Ochrana proti pádu

        DrawableObject* earth = objects[1].get();
        DrawableObject* moon = objects[2].get();

        if (!earth || !moon) return;

        // 1. Aktualizace globálních úhlù
        earthOrbitAngle += EARTH_ORBIT_SPEED_RAD * deltaTime;
        moonOrbitAngle += MOON_ORBIT_SPEED_RAD * deltaTime;

        // 2. Transformace ZEMÌ
        earth->getTransformation().reset();
        earth->getTransformation().rotate(earthOrbitAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        earth->getTransformation().translate(glm::vec3(EARTH_SUN_DISTANCE, 0.0f, 0.0f));
        earth->getTransformation().scale(glm::vec3(SCALE_EARTH)); // Aplikujeme mìøítko

        // 3. Transformace MÌSÍCE (složená)
        moon->getTransformation().reset();
        // A) Pohyb Mìsíce se Zemí kolem Slunce
        moon->getTransformation().rotate(earthOrbitAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        moon->getTransformation().translate(glm::vec3(EARTH_SUN_DISTANCE, 0.0f, 0.0f));
        // B) Lokální pohyb Mìsíce kolem Zemì
        moon->getTransformation().rotate(moonOrbitAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        moon->getTransformation().translate(glm::vec3(MOON_EARTH_DISTANCE, 0.0f, 0.0f));
        // C) Aplikujeme mìøítko Mìsíce
        moon->getTransformation().scale(glm::vec3(SCALE_MOON));

        // Pøíklad, jak by se použil Observer pro svìtlo:
        // Kdybychom chtìli, aby svìtlo (Slunce) pulzovalo:
        // float intensity = 10.0f + (sin(glfwGetTime() * 2.0f) * 5.0f);
        // pointLight->setColor(glm::vec3(intensity, intensity, intensity));
        // --> Volání setColor() by automaticky zavolalo notify() a shader by se sám updatoval.
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