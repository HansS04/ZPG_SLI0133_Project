#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include "DrawableObject.h" 
#include "Camera.h" 
#include "Light.h"

class DrawableObject;
class ShaderProgram;
class Camera;

class Scene {
private:
    std::vector<std::unique_ptr<DrawableObject>> objects;
    std::shared_ptr<ShaderProgram> colorShaderProgram;
    std::unique_ptr<Camera> camera;

    glm::vec3 m_AmbientLightColor;
    std::unique_ptr<DirLight> m_DirLight;
    bool m_DirLightOn;

    std::vector<std::unique_ptr<PointLight>> m_PointLights;
    std::vector<glm::vec3> m_PointLightBasePositions;
    float m_FireflyTime;

    std::unique_ptr<SpotLight> m_Flashlight;
    bool m_FlashlightOn;

public:
    Scene();
    void createShaders(const std::string& vsSource, const std::string& fsSource);
    void addObject(const float* data, size_t size, const glm::vec4& color);
    void clearObjects();
    size_t getObjectCount() const { return objects.size(); }

    void render() const;
    void update(float deltaTime, int currentSceneIndex = 0);

    DrawableObject* getFirstObject();
    DrawableObject* getObject(size_t index);

    Camera& getCamera() { return *camera; }

    void setAmbientLight(const glm::vec3& color);
    void setDirLight(const glm::vec3& direction, const glm::vec3& color, bool on = true);
    PointLight* addPointLight(const glm::vec3& pos, const glm::vec3& col, float con, float lin, float quad);

    void toggleFlashlight();
};