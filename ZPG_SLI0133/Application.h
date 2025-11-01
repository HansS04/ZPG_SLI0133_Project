#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <memory>
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include <functional> 
#include <array>
#include "Scene.h" 
#include "Transformation.h"

class Scene;
class DrawableObject;
class InputController; // Dopøedná deklarace

extern float rotationSpeed;
extern float rotationAngle;
extern bool direction;

class Application {
private:
    GLFWwindow* window;
    std::unique_ptr<Scene> scene;
    std::unique_ptr<InputController> m_InputController;

    std::vector<std::function<void(Scene*)>> sceneInitializers;
    int currentScene = -1;

private:
    void setupScenes();
    void setupScene0(Scene* scene);
    void setupScene1(Scene* scene);
    void setupScene2(Scene* scene);
    void setupScene3(Scene* scene);
    static void error_callback(int error, const char* description);
    static void size_callback(GLFWwindow* window, int width, int height);

public:
    static const std::string DEFAULT_VERTEX_SHADER;
    static const std::string DEFAULT_FRAGMENT_SHADER;

    Application(int width, int height, const std::string& title);
    ~Application();
    void run();

    Scene* getActiveScene() { return scene.get(); }
    GLFWwindow* getWindow() { return window; }
    void loadScene(int index);

    // TOTO JE TA CHYBÌJÍCÍ FUNKCE
    InputController* getController() { return m_InputController.get(); }
};