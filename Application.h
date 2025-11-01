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
#include "CameraController.h" 
#include "Transformation.h"

class Scene;
class DrawableObject;

extern float rotationSpeed;
extern float rotationAngle;
extern bool direction;

class Application {
private:
    GLFWwindow* window;
    std::unique_ptr<Scene> scene;

    std::vector<std::function<void(Scene*)>> sceneInitializers;
    int currentScene = -1;

private:
    bool firstMouse = true;
    float lastX = 1024.0f / 2.0f;
    float lastY = 768.0f / 2.0f;
    bool rightButtonPressed = false;

    void setupScenes();
    void loadScene(int index);

    void setupScene0(Scene* scene);
    void setupScene1(Scene* scene);
    void setupScene2(Scene* scene);
    void setupScene3(Scene* scene);

private:
    void processInput(GLFWwindow* window, float deltaTime);

    static void error_callback(int error, const char* description);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void size_callback(GLFWwindow* window, int width, int height);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

public:
    static const std::string DEFAULT_VERTEX_SHADER;
    static const std::string DEFAULT_FRAGMENT_SHADER;

    Application(int width, int height, const std::string& title);
    ~Application();
    void run();
};