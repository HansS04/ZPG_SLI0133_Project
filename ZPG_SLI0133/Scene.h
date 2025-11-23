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

struct GameTarget {
    unsigned int objectID;
    float t;                // Parame. of movement (0 - 2)
    float speed;            // Speed
    glm::vec3 pointA;       // Start
    glm::vec3 pointB;       // Up
    glm::vec3 pointC;       // End (down)
    int pointsValue;        // Value of point
    bool isHit;             // Hit detect
	float currentRotation;  // Actual rotation
};

class Scene
{
public:
    Scene();
    ~Scene() = default;

    void createShaders(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
    void addObject(const float* data, size_t size, int stride);
    void addObject(const char* modelName);

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
    void addTreeAt(glm::vec3 position, float scale = 0.5f);
    void addBushAt(glm::vec3 position, float scale = 0.5f);
    DrawableObject* getObjectByID(unsigned int id);

    // --- Game methods ---
    void setPlayerName(std::string name);
	void initGameMaterials(); // Init. materils for game targets
	void initForest();        // Generate static forest
	void spawnGameTarget();   // Create new target (Shrek and Fiona)
    void updateGame(float deltaTime); // Game Logic
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

    float m_FireflyTime;
    std::vector<PointLight*> m_FireflyPtrs;
    std::vector<glm::vec3> m_FireflyBasePositions;
    std::vector<DrawableObject*> m_FireflyBodyPtrs;

    std::shared_ptr<ShaderProgram> skyboxShader;
    std::unique_ptr<DrawableObject> skyboxObject;
    GLuint cubemapTexture;

    size_t m_ObjectCounter = 0;

    std::shared_ptr<Material> m_TreeMaterial;
    std::shared_ptr<Material> m_BushMaterial;
    std::shared_ptr<Material> m_MatShrek;
    std::shared_ptr<Material> m_MatFiona;

	// Game variables
    std::vector<GameTarget> m_GameTargets;
    float m_SpawnTimer = 0.0f;
    int m_Score = 0;
    std::string m_PlayerName;
    bool m_GameRunning = false;
    bool m_GameFinished = false;
};