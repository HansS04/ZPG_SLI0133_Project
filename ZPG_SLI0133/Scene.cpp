#include "Scene.h"
#include "DrawableObject.h"
#include "ShaderProgram.h"
#include "Model.h"
#include "TransformationComposite.h"
#include "Camera.h"
#include "Shader.h" 
#include "Light.h"
#include "TextureLoader.h" 
#include "tree.h"
#include "bushes.h" // Predpokladam existenci bushes.h
#include "Material.h" 
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
    m_FlashlightOn(false),
    m_Score(0),
    m_SpawnTimer(0.0f),
    m_GameRunning(false),
    m_GameFinished(false),
    m_PlayerName("Hrac")
{
    m_Flashlight = std::make_unique<SpotLight>(
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, -1),
        glm::vec3(1.0f, 1.0f, 1.0f),
        1.0f, 0.09f, 0.032f,
        glm::cos(glm::radians(12.5f)),
        glm::cos(glm::radians(15.5f))
    );
}

void Scene::InitSkybox()
{
    auto vert = std::make_unique<Shader>(GL_VERTEX_SHADER, "skybox.vert");
    auto frag = std::make_unique<Shader>(GL_FRAGMENT_SHADER, "skybox.frag");
    skyboxShader = std::make_shared<ShaderProgram>(*vert, *frag);

    auto model = std::make_unique<Model>("assets/sky/skybox.obj");

    skyboxObject = std::make_unique<DrawableObject>(std::move(model), skyboxShader);

    std::vector<std::string> faces =
    {
        "assets/cubemap/posx.jpg",
        "assets/cubemap/negx.jpg",
        "assets/cubemap/posy.jpg",
        "assets/cubemap/negy.jpg",
        "assets/cubemap/posz.jpg",
        "assets/cubemap/negz.jpg"
    };
    cubemapTexture = TextureLoader::loadCubemap(faces);
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

void Scene::addObject(const float* data, size_t size, int stride) {
    std::unique_ptr<Model> m = std::make_unique<Model>(data, size, stride);
    std::unique_ptr<DrawableObject> obj = std::make_unique<DrawableObject>(std::move(m), colorShaderProgram);

    m_ObjectCounter++;
    obj->setID(m_ObjectCounter);

    objects.push_back(std::move(obj));
}

void Scene::addObject(const char* modelName) {
    std::unique_ptr<Model> m = std::make_unique<Model>(modelName);
    std::unique_ptr<DrawableObject> obj = std::make_unique<DrawableObject>(std::move(m), colorShaderProgram);

    m_ObjectCounter++;
    obj->setID(m_ObjectCounter);

    objects.push_back(std::move(obj));
}

DrawableObject* Scene::addGameObject(const char* modelName) {
    std::unique_ptr<Model> m = std::make_unique<Model>(modelName);
    std::unique_ptr<DrawableObject> obj = std::make_unique<DrawableObject>(std::move(m), colorShaderProgram);

    m_ObjectCounter++;
    obj->setID(m_ObjectCounter);

    DrawableObject* ptr = obj.get();
    objects.push_back(std::move(obj));
    return ptr;
}

void Scene::clearObjects() {
    objects.clear();
    m_Lights.clear();
    m_SpotLights.clear();
    m_FireflyPtrs.clear();
    m_FireflyBasePositions.clear();
    m_FireflyBodyPtrs.clear();

    m_GameTargets.clear();
    m_Score = 0;
    m_GameRunning = false;
    m_GameFinished = false;
}

void Scene::DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix) const
{
    if (!skyboxShader || !skyboxObject) return;
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    skyboxShader->use();
    skyboxShader->setMat4("view", viewMatrix);
    skyboxShader->setMat4("projection", projectionMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    skyboxShader->setInt("skybox", 0);

    skyboxObject->draw();

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

void Scene::render() const {
    if (!camera) return;

    glm::mat4 viewMatrix = camera->getViewMatrix();
    glm::mat4 projectionMatrix = camera->getProjectionMatrix();

    glEnable(GL_STENCIL_TEST);

    glStencilMask(0x00);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    DrawSkybox(viewMatrix, projectionMatrix);
    glStencilMask(0xFF);

    if (colorShaderProgram) {
        colorShaderProgram->use();

        colorShaderProgram->setVec3("u_ViewPos", camera->getPosition());
        colorShaderProgram->setAmbientLight(m_AmbientLightColor);
        colorShaderProgram->setLights(m_Lights);
        colorShaderProgram->setSpotLights(m_SpotLights);
        colorShaderProgram->setFlashlight(*m_Flashlight, m_FlashlightOn);
    }

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    for (const auto& obj : objects) {
        glStencilFunc(GL_ALWAYS, obj->getID(), 0xFF);
        obj->draw();
    }

    glDisable(GL_STENCIL_TEST);
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

    // Update pro herni scenu (4)
    if (currentSceneIndex == 4) {
        // Pohyb svetlusek
        m_FireflyTime += deltaTime * 0.5f;
        for (size_t i = 0; i < m_FireflyPtrs.size(); ++i) {
            glm::vec3 basePos = m_FireflyBasePositions[i];
            glm::vec3 offset;
            // Nahodny pohyb svetlusek
            offset.x = sin(m_FireflyTime + i * 2.1f) * 2.0f;
            offset.y = cos(m_FireflyTime + i * 1.7f) * 1.0f;
            offset.z = sin(m_FireflyTime + i * 0.8f) * 2.0f;

            glm::vec3 newPos = basePos + offset;
            m_FireflyPtrs[i]->position = newPos;

            if (i < m_FireflyBodyPtrs.size()) {
                m_FireflyBodyPtrs[i]->getTransformation().reset();
                m_FireflyBodyPtrs[i]->getTransformation().translate(newPos);
                m_FireflyBodyPtrs[i]->getTransformation().scale(glm::vec3(0.1f)); // Mala svetluska
            }
        }

        // Herni logika
        updateGame(deltaTime);
    }
}

void Scene::setPlayerName(std::string name) {
    m_PlayerName = name;
    m_GameRunning = true;
    std::cout << "Vitejte ve hre, " << m_PlayerName << "! Cilem je ziskat 100 bodu." << std::endl;
}

void Scene::initGameMaterials() {
    if (!m_MatShrek) {
        m_MatShrek = std::make_shared<Material>();
        m_MatShrek->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        m_MatShrek->specular = glm::vec3(0.1f, 0.1f, 0.1f);
        m_MatShrek->shininess = 32.0f;
        m_MatShrek->diffuseTextureID = TextureLoader::LoadTexture("assets/shrek/shrek.png");
    }
    if (!m_MatFiona) {
        m_MatFiona = std::make_shared<Material>();
        m_MatFiona->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        m_MatFiona->specular = glm::vec3(0.1f, 0.1f, 0.1f);
        m_MatFiona->shininess = 32.0f;
        m_MatFiona->diffuseTextureID = TextureLoader::LoadTexture("assets/shrek/fiona.png");
    }
    if (!m_TreeMaterial) {
        m_TreeMaterial = std::make_shared<Material>();
        m_TreeMaterial->diffuse = glm::vec3(0.2f, 0.5f, 0.2f);
        m_TreeMaterial->ambient = glm::vec3(0.1f, 0.3f, 0.1f);
    }
    if (!m_BushMaterial) {
        m_BushMaterial = std::make_shared<Material>();
        m_BushMaterial->diffuse = glm::vec3(0.1f, 0.4f, 0.1f);
    }
}

void Scene::initForest() {
    // Staticke generovani stromu a keru po plose
    int range = 35; // Velikost lesa

    for (int i = 0; i < 60; ++i) { // 60 stromu
        float x = (rand() % (2 * range)) - range;
        float z = (rand() % (2 * range)) - range;

        // Nechceme stromy uplne uprostred, kde se spawnuje nejvic
        if (abs(x) < 5 && abs(z) < 5) continue;

        addTreeAt(glm::vec3(x, 0.0f, z), 0.8f + (rand() % 50) / 100.0f);
    }

    for (int i = 0; i < 40; ++i) { // 40 keru
        float x = (rand() % (2 * range)) - range;
        float z = (rand() % (2 * range)) - range;
        addBushAt(glm::vec3(x, 0.0f, z), 0.5f + (rand() % 50) / 100.0f);
    }
}

void Scene::spawnGameTarget() {
    if (!m_GameRunning || m_GameFinished) return;

    // Vetsi rozsah pro spawn (les)
    float randomX = (std::rand() % 50 - 25) * 1.0f;
    float randomZ = (std::rand() % 40 - 20) * 1.0f;

    bool isShrek = (std::rand() % 2) == 0;

    DrawableObject* newObj = nullptr;
    if (isShrek) {
        newObj = addGameObject("assets/shrek/shrek.obj");
        newObj->setMaterial(m_MatShrek);
    }
    else {
        newObj = addGameObject("assets/shrek/fiona.obj");
        newObj->setMaterial(m_MatFiona);
    }

    // Startovaci a koncove body (zapadnuti do zeme)
    glm::vec3 A = glm::vec3(randomX, -4.0f, randomZ);
    glm::vec3 B = glm::vec3(randomX, 2.0f, randomZ); // Vyskoci vys
    glm::vec3 C = glm::vec3(randomX + (isShrek ? 3.0f : -3.0f), -4.0f, randomZ);

    GameTarget target;
    target.objectID = newObj->getID();
    target.t = 0.0f;
    target.speed = 0.8f + (std::rand() % 120) / 100.0f; // Nahodna rychlost
    target.pointA = A;
    target.pointB = B;
    target.pointC = C;
    target.pointsValue = isShrek ? 10 : -50;
    target.isHit = false;
    target.currentRotation = (float)(std::rand() % 360);

    // Pocatecni nastaveni
    newObj->getTransformation().reset();
    newObj->getTransformation().translate(A);
    newObj->getTransformation().scale(glm::vec3(2.0f)); // VETSI MODELY
    newObj->getTransformation().rotate(target.currentRotation, glm::vec3(0, 1, 0));

    m_GameTargets.push_back(target);
}

void Scene::updateGame(float deltaTime) {
    if (m_GameFinished) return;

    // Kontrola vitezstvi
    if (m_Score >= 100) {
        m_GameFinished = true;
        m_GameRunning = false;
        std::cout << "\n******************************************" << std::endl;
        std::cout << "GRATULUJEME! Hrac " << m_PlayerName << " dosahl 100 bodu!" << std::endl;
        std::cout << "Hra dokoncena." << std::endl;
        std::cout << "******************************************\n" << std::endl;
        return;
    }

    m_SpawnTimer += deltaTime;
    float spawnInterval = 1.0f; // Rychlejsi spawn
    if (m_SpawnTimer > spawnInterval) {
        spawnGameTarget();
        m_SpawnTimer = 0.0f;
    }

    for (auto it = m_GameTargets.begin(); it != m_GameTargets.end(); ) {
        GameTarget& tgt = *it;

        if (!tgt.isHit) {
            tgt.t += tgt.speed * deltaTime;
        }
        else {
            tgt.t += tgt.speed * deltaTime * 5.0f; // Po zasahu zmizi rychle
        }

        glm::vec3 currentPos;
        if (tgt.t <= 1.0f) {
            currentPos = tgt.pointA + tgt.t * (tgt.pointB - tgt.pointA);
        }
        else {
            float t2 = tgt.t - 1.0f;
            currentPos = tgt.pointB + t2 * (tgt.pointC - tgt.pointB);
        }

        tgt.currentRotation += 100.0f * deltaTime;

        DrawableObject* obj = getObjectByID(tgt.objectID);
        if (obj) {
            obj->getTransformation().reset();
            obj->getTransformation().translate(currentPos);

            if (tgt.isHit) {
                obj->getTransformation().scale(glm::vec3(0.5f));
            }
            else {
                obj->getTransformation().scale(glm::vec3(2.0f)); // VETSI
            }
            obj->getTransformation().rotate(tgt.currentRotation, glm::vec3(0, 1, 0));
        }

        // Smazani objektu kdyz zajede zpet dolu (t > 2.0)
        if (tgt.t >= 2.0f) {
            // Najdeme a smazeme z vykreslovaciho seznamu 'objects'
            for (auto oIt = objects.begin(); oIt != objects.end(); ++oIt) {
                if ((*oIt)->getID() == tgt.objectID) {
                    objects.erase(oIt); // Uvolni pamet (unique_ptr)
                    break;
                }
            }
            // Smazeme z herniho seznamu
            it = m_GameTargets.erase(it);
        }
        else {
            ++it;
        }
    }
}

void Scene::hitObject(unsigned int id) {
    if (m_GameFinished) return;
    if (id == 0) return;

    for (auto& tgt : m_GameTargets) {
        if (tgt.objectID == id && !tgt.isHit) {
            tgt.isHit = true;
            m_Score += tgt.pointsValue;

            // Vypis do konzole
            std::cout << "---------------------------------" << std::endl;
            std::cout << "Hrac: " << m_PlayerName << " | Skore: " << m_Score << std::endl;
            if (tgt.pointsValue > 0) std::cout << "-> Zasah SHREKA (+10)" << std::endl;
            else std::cout << "-> Zasah FIONY (-50)" << std::endl;
            std::cout << "---------------------------------" << std::endl;
            return;
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

DirLight* Scene::addDirLight(const glm::vec3& dir, const glm::vec3& col) {
    auto newLight = std::make_unique<DirLight>(dir, col);
    DirLight* ptr = newLight.get();
    m_Lights.push_back(std::move(newLight));
    return ptr;
}

PointLight* Scene::addPointLight(const glm::vec3& pos, const glm::vec3& col, float constant, float linear, float quadratic) {
    auto newLight = std::make_unique<PointLight>(pos, col, constant, linear, quadratic);
    PointLight* ptr = newLight.get();
    m_Lights.push_back(std::move(newLight));
    return ptr;
}

PointLight* Scene::addFirefly(const glm::vec3& pos, const glm::vec3& col, float constant, float linear, float quadratic) {
    auto newLight = std::make_unique<PointLight>(pos, col, constant, linear, quadratic);
    PointLight* ptr = newLight.get();
    m_Lights.push_back(std::move(newLight));

    m_FireflyPtrs.push_back(ptr);
    m_FireflyBasePositions.push_back(pos);

    return ptr;
}

SpotLight* Scene::addSpotLight(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& col, float c, float l, float q, float cut, float outerCut) {
    auto newLight = std::make_unique<SpotLight>(pos, dir, col, c, l, q, cut, outerCut);
    SpotLight* ptr = newLight.get();
    m_SpotLights.push_back(std::move(newLight));
    return ptr;
}

void Scene::addFireflyBody(DrawableObject* body) {
    m_FireflyBodyPtrs.push_back(body);
}

void Scene::toggleFlashlight() {
    m_FlashlightOn = !m_FlashlightOn;
}

DrawableObject* Scene::getObjectByID(unsigned int id) {
    for (const auto& obj : objects) {
        if (obj->getID() == id) {
            return obj.get();
        }
    }
    return nullptr;
}

void Scene::selectObjectByID(unsigned int id) {
    // Volitelny debug
}

void Scene::addTreeAt(glm::vec3 position, float scale) {
    // Pridani stromu
    std::unique_ptr<Model> m = std::make_unique<Model>(tree, sizeof(tree), 6);
    std::unique_ptr<DrawableObject> obj = std::make_unique<DrawableObject>(std::move(m), colorShaderProgram);

    m_ObjectCounter++;
    obj->setID(m_ObjectCounter);
    obj->setMaterial(m_TreeMaterial);
    obj->getTransformation().translate(position).scale(glm::vec3(scale));

    objects.push_back(std::move(obj));
}

void Scene::addBushAt(glm::vec3 position, float scale) {
    // Pridani kere
    std::unique_ptr<Model> m = std::make_unique<Model>(bushes, sizeof(bushes), 6);
    std::unique_ptr<DrawableObject> obj = std::make_unique<DrawableObject>(std::move(m), colorShaderProgram);

    m_ObjectCounter++;
    obj->setID(m_ObjectCounter);
    obj->setMaterial(m_BushMaterial);
    obj->getTransformation().translate(position).scale(glm::vec3(scale));

    objects.push_back(std::move(obj));
}