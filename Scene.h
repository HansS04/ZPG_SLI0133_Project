#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include "DrawableObject.h" 
#include "Camera.h" 
// #include "CameraController.h" // <- ODSTRANÌNO
#include "Light.h"

class DrawableObject;
class ShaderProgram;
class Camera;
// class CameraController; // <- ODSTRANÌNO
class Light;

class Scene {
private:
    std::vector<std::unique_ptr<DrawableObject>> objects;
    std::shared_ptr<ShaderProgram> colorShaderProgram;
    std::unique_ptr<Camera> camera;
    // std::unique_ptr<CameraController> cameraController; // <- ODSTRANÌNO
    std::unique_ptr<Light> pointLight;

public:
    Scene();
    void createShaders(const std::string& vsSource, const std::string& fsSource);
    void addObject(const float* data, size_t size, const glm::vec4& color);
    void clearObjects() { objects.clear(); }
    size_t getObjectCount() const { return objects.size(); }

    void render() const;
    void update(float deltaTime, int currentSceneIndex = 0);

    DrawableObject* getFirstObject();
    DrawableObject* getObject(size_t index);

    Camera& getCamera() { return *camera; }
    const Camera& getCamera() const { return *camera; }
    // CameraController& getCameraController() { return *cameraController; } // <- ODSTRANÌNO

    Light& getLight() { return *pointLight; }
    const Light& getLight() const { return *pointLight; }
};