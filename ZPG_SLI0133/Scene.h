#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include "DrawableObject.h" 
#include "Camera.h" 
#include "Lights.h"

class DrawableObject;
class ShaderProgram;
class Camera;

class Scene {
private:
    std::vector<std::unique_ptr<DrawableObject>> objects;
    std::shared_ptr<ShaderProgram> colorShaderProgram;
    std::unique_ptr<Camera> camera;

    glm::vec3 m_AmbientLightColor;

    std::vector<std::unique_ptr<Light>> m_Lights;
    std::vector<std::unique_ptr<SpotLight>> m_SpotLights;

    std::unique_ptr<SpotLight> m_Flashlight;
    bool m_FlashlightOn;

    std::vector<PointLight*> m_FireflyPtrs;
    std::vector<glm::vec3> m_FireflyBasePositions;
    std::vector<DrawableObject*> m_FireflyBodyPtrs;
    float m_FireflyTime;

public:
    Scene();
    void createShaders(const std::string& vsSource, const std::string& fsSource);
    void addObject(const float* data, size_t size);
    void addObject(const char* modelName);
    void clearObjects();
    size_t getObjectCount() const { return objects.size(); }

    void render() const;
    void update(float deltaTime, int currentSceneIndex = 0);

    DrawableObject* getFirstObject();
    DrawableObject* getObject(size_t index);

    Camera& getCamera() { return *camera; }

    const std::vector<std::unique_ptr<Light>>& getLights() const { return m_Lights; }
    const SpotLight* getFlashlight() const { return m_Flashlight.get(); }
    bool isFlashlightOn() const { return m_FlashlightOn; }

    void setAmbientLight(const glm::vec3& color);
    DirLight* addDirLight(const glm::vec3& dir, const glm::vec3& col);
    PointLight* addPointLight(const glm::vec3& pos, const glm::vec3& col, float c, float l, float q);
    PointLight* addFirefly(const glm::vec3& pos, const glm::vec3& col, float c, float l, float q);
    SpotLight* addSpotLight(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& col, float c, float l, float q, float cut, float outerCut);
    void addFireflyBody(DrawableObject* body);

    void toggleFlashlight();
};