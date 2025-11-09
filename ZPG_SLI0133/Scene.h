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

class Scene
{
public:
    Scene();
    ~Scene() = default;

    void createShaders(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
    void addObject(const float* data, size_t size, int stride);
    void addObject(const char* modelName);
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

    // --- PØIDÁNO PRO SKYBOX ---
    std::shared_ptr<ShaderProgram> skyboxShader;
    std::unique_ptr<DrawableObject> skyboxObject;
    GLuint cubemapTexture;
};