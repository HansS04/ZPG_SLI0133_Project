#pragma once
#include <vector>
#include <memory>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Lights.h"

class DrawableObject;
class ShaderProgram;
class Camera;
class Model;
class TransformationComposite;
class DirLight;
class PointLight;
class SpotLight;
class Material;

// Struktura pro herni cil (Shrek/Fiona)
struct GameTarget {
    unsigned int objectID;
    float t;                // Parametr pohybu (0 az 2)
    float speed;            // Rychlost
    glm::vec3 pointA;       // Start (dole)
    glm::vec3 pointB;       // Vrchol (nahore)
    glm::vec3 pointC;       // Konec (dole)
    int pointsValue;        // Hodnota bodu
    bool isHit;             // Zasah
    float currentRotation;  // Aktualni rotace
};

class Scene
{
public:
    Scene();
    ~Scene() = default;

    void createShaders(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
    void addObject(const float* data, size_t size, int stride);
    void addObject(const char* modelName);

    // Pridani herniho objektu (vraci pointer pro manipulaci)
    DrawableObject* addGameObject(const char* modelName);

    void clearObjects();
    void render() const;
    void update(float deltaTime, int currentSceneIndex);

    DrawableObject* getFirstObject();
    DrawableObject* getObject(size_t index);
    size_t getObjectCount() const { return objects.size(); }
    Camera& getCamera() { return *camera; }

    void setAmbientLight(const glm::vec3& color);
    DirLight* addDirLight(const glm::vec3& dir, const glm::vec3& col);
    PointLight* addPointLight(const glm::vec3& pos, const glm::vec3& col, float constant, float linear, float quadratic);
    PointLight* addFirefly(const glm::vec3& pos, const glm::vec3& col, float constant, float linear, float quadratic);
    SpotLight* addSpotLight(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& col, float c, float l, float q, float cut, float outerCut);
    void addFireflyBody(DrawableObject* body);
    void toggleFlashlight();
    void InitSkybox();
    void DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix) const;

    void selectObjectByID(unsigned int id);
    void addTreeAt(glm::vec3 position, float scale);
    void addBushAt(glm::vec3 position, float scale);
    DrawableObject* getObjectByID(unsigned int id);

    // --- HERNI METODY ---
    void setPlayerName(std::string name);
    void initGameMaterials(); // Pripravi materialy
    void initForest();        // Vygeneruje staticky les
    void spawnGameTarget();   // Vytvori krtka
    void updateGame(float deltaTime); // Logika hry
    void hitObject(unsigned int id);  // Zasah

private:
    std::vector<std::unique_ptr<DrawableObject>> objects;
    std::shared_ptr<ShaderProgram> colorShaderProgram;
    std::unique_ptr<Camera> camera;

    std::vector<std::unique_ptr<Light>> m_Lights;
    std::vector<std::unique_ptr<SpotLight>> m_SpotLights;
    std::unique_ptr<SpotLight> m_Flashlight;
    bool m_FlashlightOn;
    glm::vec3 m_AmbientLightColor;

    // Svetlusky
    float m_FireflyTime;
    std::vector<PointLight*> m_FireflyPtrs;
    std::vector<glm::vec3> m_FireflyBasePositions;
    std::vector<DrawableObject*> m_FireflyBodyPtrs;

    std::shared_ptr<ShaderProgram> skyboxShader;
    std::unique_ptr<DrawableObject> skyboxObject;
    GLuint cubemapTexture;

    size_t m_ObjectCounter = 0;

    // Materialy (cache)
    std::shared_ptr<Material> m_TreeMaterial;
    std::shared_ptr<Material> m_BushMaterial;
    std::shared_ptr<Material> m_MatShrek;
    std::shared_ptr<Material> m_MatFiona;

    // Herni promenne
    std::vector<GameTarget> m_GameTargets;
    float m_SpawnTimer = 0.0f;
    int m_Score = 0;
    std::string m_PlayerName;
    bool m_GameRunning = false;
    bool m_GameFinished = false;
};