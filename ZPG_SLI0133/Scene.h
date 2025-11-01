#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include "DrawableObject.h" 
#include "Camera.h" 
#include "Light.h"

class DrawableObject;
class ShaderProgram;
class Camera;
class Light;

class Scene {
private:
    std::vector<std::unique_ptr<DrawableObject>> objects;
    std::shared_ptr<ShaderProgram> colorShaderProgram;
    std::unique_ptr<Camera> camera;

    std::unique_ptr<Light> pointLight;

    std::vector<std::unique_ptr<Light>> m_AdditionalLights;
    std::vector<glm::vec3> m_AdditionalLightBasePositions;
    float m_FireflyTime;

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
    const Camera& getCamera() const { return *camera; }

    Light& getLight() { return *pointLight; }
    const Light& getLight() const { return *pointLight; }

    Light* addAdditionalLight(const glm::vec3& pos, const glm::vec3& col, float constant, float linear, float quadratic);
    const std::vector<std::unique_ptr<Light>>& getAdditionalLights() const { return m_AdditionalLights; }
};