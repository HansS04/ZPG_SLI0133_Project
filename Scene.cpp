#include "Scene.h"
#include "DrawableObject.h"
#include "ShaderProgram.h"
#include "Model.h"
#include "Transformation.h"
#include "Camera.h"
#include "CameraController.h" 
#include "Shader.h" 
#include "Light.h" // <--- NOVINKA: Zahrnutí hlavièkového souboru pro Light
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <stdexcept>
#include <algorithm> // Potøebné pro std::remove, i když zde není pøímo použito

// Externí promìnné (deklarace, definice jsou v Application.cpp)
extern float rotationSpeed;
extern float rotationAngle;
extern bool direction;

Scene::Scene()
{
    // NOVINKA: Inicializace bodového svìtla s výchozími parametry
    pointLight = std::make_unique<Light>();
}

void Scene::createShaders(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) {

    Shader vs(GL_VERTEX_SHADER, vertexShaderFile.c_str());
    Shader fs(GL_FRAGMENT_SHADER, fragmentShaderFile.c_str());

    colorShaderProgram = std::make_shared<ShaderProgram>(vs, fs);


    int width = 1024, height = 768; // Pevné rozlišení, nebo získat z GLFW

    camera = std::make_unique<Camera>(glm::vec3(0.0f, 1.0f, 3.0f));
    cameraController = std::make_unique<CameraController>(*camera);

    if (height > 0) {
        camera->setAspectRatio((float)width, (float)height);
    }

    // Shader program se zaregistruje jako observer kamery
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

    // NOVINKA: Pøed renderováním všech objektù nastavíme uniformy pro svìtlo
    if (colorShaderProgram && pointLight) {
        // Musíme použít shader PØED nastavením uniform
        colorShaderProgram->use();
        // Odeslání pozice, barvy a parametrù útlumu do shaderu
        colorShaderProgram->setLightUniforms(*pointLight);
    }

    for (const auto& obj : objects) {
        // obj->draw() uvnitø volá shaderProgram->use() a nastavuje ModelMatrix
        obj->draw();
    }
}


// Externí promìnné (deklarace, definice jsou v Application.cpp)
extern float rotationSpeed;
extern float rotationAngle;
extern bool direction;

// Rychlosti rotace (lokální konstanty pro update)
constexpr float EARTH_ORBIT_SPEED = 50.f;
constexpr float MOON_ORBIT_SPEED = 300.0f;

// Externí promìnné pro obìžné úhly
extern float earthOrbitAngle;
extern float moonOrbitAngle;

// Externí promìnné pro rozmìry scény 2 (BEZ 'const')
extern float SCALE_EARTH;
extern float SCALE_MOON;
extern float EARTH_SUN_DISTANCE;
extern float MOON_EARTH_DISTANCE;


void Scene::update(float deltaTime, int currentSceneIndex) {
    if (currentSceneIndex == 2) {

        // Nastavíme konstanty pro tuto scénu (jsou stejné jako v setupScene2)
        const float SCALE_MOON = 0.5f;
        const float EARTH_SUN_DISTANCE = 8.0f;
        const float MOON_EARTH_DISTANCE = 2.0f;

        // Konstanty pro rychlost obìhu
        const float EARTH_ORBIT_SPEED = 0.5f; // Radiany za sekundu
        const float MOON_ORBIT_SPEED = 2.0f;  // Radiany za sekundu

        // Statické promìnné pro akumulaci úhlu
        static float earthOrbitAngle = 0.0f;
        static float moonOrbitAngle = 0.0f;

        // Získáme objekty (pøedpoklad: 0=Slunce, 1=Zemì, 2=Mìsíc)
        DrawableObject* earth = objects[1].get();
        DrawableObject* moon = objects[2].get();

        if (!earth || !moon) return;

        // 1. Aktualizace úhlù
        earthOrbitAngle += EARTH_ORBIT_SPEED * deltaTime;
        moonOrbitAngle += MOON_ORBIT_SPEED * deltaTime;

        // 2. Transformace ZEMÌ: Obíhá Slunce
        earth->getTransformation().reset();
        earth->getTransformation().rotate(earthOrbitAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        earth->getTransformation().translate(glm::vec3(EARTH_SUN_DISTANCE, 0.0f, 0.0f));

        // 3. Transformace MÌSÍCE: Obíhá Zemi
        // Musíme složit transformace: (Rotace Zemì + Posun Zemì) + (Rotace Mìsíce + Posun Mìsíce)
        moon->getTransformation().reset();

        // --- A) Pohyb Mìsíce se Zemí kolem Slunce ---
        // Rotace Zemì kolem (0,0,0) a posun Zemì
        moon->getTransformation().rotate(earthOrbitAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        moon->getTransformation().translate(glm::vec3(EARTH_SUN_DISTANCE, 0.0f, 0.0f));

        // --- B) Lokální pohyb Mìsíce kolem Zemì ---
        // Vzhledem k novému poèátku (pozice Zemì) rotujeme a posouváme Mìsíc
        moon->getTransformation().rotate(moonOrbitAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        moon->getTransformation().translate(glm::vec3(MOON_EARTH_DISTANCE, 0.0f, 0.0f));

        // 4. Mìøítko Mìsíce (musí být poslední)
        moon->getTransformation().scale(glm::vec3(SCALE_MOON));

    }
    // ... (ostatní aktualizaèní logika)
}

DrawableObject* Scene::getFirstObject() {
    if (objects.empty()) return nullptr;
    return objects.front().get();
}

DrawableObject* Scene::getObject(size_t index) {
    if (index >= objects.size()) return nullptr;
    return objects[index].get();
}