#include "Application.h"
#include "Scene.h"
#include "DrawableObject.h" 
#include "InputController.h" 
#include "Render.h"
#include "Camera.h"
#include "Shader.h"
#include "TransformationComposite.h"
#include "Material.h"
#include "TextureLoader.h" 
#include <stdexcept>
#include <glm/glm.hpp> 
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <random> 
#include <ctime> 
#include <glm/gtc/constants.hpp>


#include "plain.h"
#include "tree.h" 
#include "bushes.h"
#include "bench.h" 
#include "sphere.h"

extern const float PLAIN_VERTICES[];
extern const size_t PLAIN_VERTICES_SIZE;
extern const float BENCH_VERTICES[];
extern const size_t BENCH_DATA_SIZE;

extern const float SPHERE_VERTICES[];
const size_t SPHERE_VERTICES_SIZE = sizeof(sphere);

extern const float TREE_VERTICES[];
const size_t TREE_DATA_SIZE = sizeof(tree);

extern const float BUSHES_VERTICES[];
const size_t BUSHES_DATA_SIZE = sizeof(bushes);

float rotationSpeed = 5.0f;
float rotationAngle = 0.0f;
bool direction = false;

const float TEST_TRIANGLE[] = {
    0.0f,  1.0f,  0.0f,     0.0f, 0.0f, 1.0f,   0.5f, 1.0f,
   -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
    1.0f, -1.0f, 0.0f,     0.0f, 0.0f, 1.0f,   1.0f, 0.0f
};
const size_t TEST_TRIANGLE_SIZE = sizeof(TEST_TRIANGLE);

static void key_callback_adapter(GLFWwindow* w, int k, int s, int a, int m) {
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(w));
    if (app) app->getController()->onKey(k, s, a, m);
}
static void mouse_button_callback_adapter(GLFWwindow* w, int b, int a, int m) {
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(w));
    if (app) app->getController()->onMouseButton(b, a, m);
}
static void cursor_position_callback_adapter(GLFWwindow* w, double x, double y) {
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(w));
    if (app) app->getController()->onCursorPos(x, y);
}
static void scroll_callback_adapter(GLFWwindow* w, double x, double y) {
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(w));
    if (app) app->getController()->onScroll(x, y);
}

Application::Application(int width, int height, const std::string& title)
    : m_RandomEngine(static_cast<unsigned int>(std::time(nullptr)))
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);

    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);
    m_InputController = std::make_unique<InputController>(*this, window, width / 2.0f, height / 2.0f);
    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, size_callback);
    glfwSetKeyCallback(window, key_callback_adapter);
    glfwSetMouseButtonCallback(window, mouse_button_callback_adapter);
    glfwSetCursorPosCallback(window, cursor_position_callback_adapter);
    glfwSetScrollCallback(window, scroll_callback_adapter);

    if (glewInit() != GLEW_OK)
        throw std::runtime_error("Failed to initialize GLEW");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glViewport(0, 0, width, height);

    setupScenes();
    loadScene(3);

    m_Render = std::make_unique<Render>(*this);
}

Application::~Application() {
    glfwTerminate();
}

void Application::start() {
    if (m_Render) {
        m_Render->run();
    }
}

void Application::error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

void Application::size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app && app->scene && height > 0) {
        app->scene->getCamera().setAspectRatio((float)width, (float)height);
        app->scene->getCamera().updateMatrices();
    }
}

void Application::setupScenes() {
    sceneInitializers.push_back([this](Scene* s) { setupScene0(s); });
    sceneInitializers.push_back([this](Scene* s) { setupScene1(s); });
    sceneInitializers.push_back([this](Scene* s) { setupScene2(s); });
    sceneInitializers.push_back([this](Scene* s) { setupScene3(s); });
    sceneInitializers.push_back([this](Scene* s) { setupScene4(s); });
}

void Application::loadScene(int index) {
    if (index >= 0 && index < sceneInitializers.size()) {
        scene = std::make_unique<Scene>();

        if (index == 3) {
            scene->createShaders("basic_vertexShader.vert", "basic_Blinn_fragmentShader.frag");
        }
        else {
            scene->createShaders("basic_vertexShader.vert", "basic_Blinn_fragmentShader.frag");
        }

        sceneInitializers[index](scene.get());
        currentScene = index;
        std::cout << "Nactena scena: " << index << std::endl;
    }
}

void Application::setupScene0(Scene* s) {
    s->setAmbientLight(glm::vec3(0.1f, 0.1f, 0.1f));
    s->addDirLight(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f));
    auto triMaterial = std::make_shared<Material>();
    triMaterial->diffuse = glm::vec3(1.0f, 0.0f, 0.0f);
    triMaterial->ambient = glm::vec3(1.0f, 0.0f, 0.0f);
    triMaterial->specular = glm::vec3(0.5f, 0.5f, 0.5f);
    triMaterial->shininess = 32.0f;
    s->addObject(TEST_TRIANGLE, TEST_TRIANGLE_SIZE, 8);
    DrawableObject* obj = s->getFirstObject();
    if (obj) {
        obj->setMaterial(triMaterial);
        obj->getTransformation().translate(glm::vec3(0.0f, 1.0f, 0.0f));
    }
}

void Application::setupScene1(Scene* scene) {
    scene->clearObjects();
    scene->InitSkybox();
    scene->setAmbientLight(glm::vec3(0.1f, 0.1f, 0.1f));
    auto sphereMaterial = std::make_shared<Material>();
    sphereMaterial->diffuse = glm::vec3(0.9f, 0.9f, 0.9f);
    sphereMaterial->ambient = glm::vec3(0.9f, 0.9f, 0.9f);
    sphereMaterial->specular = glm::vec3(1.0f, 1.0f, 1.0f);
    sphereMaterial->shininess = 64.0f;
    auto lightBulbMaterial = std::make_shared<Material>();
    lightBulbMaterial->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    const float OBJECT_RADIUS = 1.0f;
    const float OBJECT_OFFSET = 1.5f;
    const float OBJECT_Y_POS = 0.0f;
    const glm::vec3 LIGHT_POS(0.0f, 3.0f, 0.0f);
    scene->addObject(sphere, SPHERE_VERTICES_SIZE, 6);
    DrawableObject* s1 = scene->getObject(scene->getObjectCount() - 1);
    s1->setMaterial(sphereMaterial);
    s1->getTransformation().scale(glm::vec3(OBJECT_RADIUS));
    s1->getTransformation().translate(glm::vec3(-OBJECT_OFFSET, OBJECT_Y_POS, OBJECT_OFFSET));
    scene->addObject(sphere, SPHERE_VERTICES_SIZE, 6);
    DrawableObject* s2 = scene->getObject(scene->getObjectCount() - 1);
    s2->setMaterial(sphereMaterial);
    s2->getTransformation().scale(glm::vec3(OBJECT_RADIUS));
    s2->getTransformation().translate(glm::vec3(OBJECT_OFFSET, OBJECT_Y_POS, OBJECT_OFFSET));
    scene->addObject(sphere, SPHERE_VERTICES_SIZE, 6);
    DrawableObject* s3 = scene->getObject(scene->getObjectCount() - 1);
    s3->setMaterial(sphereMaterial);
    s3->getTransformation().scale(glm::vec3(OBJECT_RADIUS));
    s3->getTransformation().translate(glm::vec3(-OBJECT_OFFSET, OBJECT_Y_POS, -OBJECT_OFFSET));
    scene->addObject(sphere, SPHERE_VERTICES_SIZE, 6);
    DrawableObject* s4 = scene->getObject(scene->getObjectCount() - 1);
    s4->setMaterial(sphereMaterial);
    s4->getTransformation().scale(glm::vec3(OBJECT_RADIUS));
    s4->getTransformation().translate(glm::vec3(OBJECT_OFFSET, OBJECT_Y_POS, -OBJECT_OFFSET));
    scene->addPointLight(LIGHT_POS, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f);
    scene->addObject(sphere, SPHERE_VERTICES_SIZE, 6);
    DrawableObject* lightBulb = scene->getObject(scene->getObjectCount() - 1);
    lightBulb->setMaterial(lightBulbMaterial);
    lightBulb->setUnlit(true);
    lightBulb->getTransformation().scale(glm::vec3(0.15f));
    lightBulb->getTransformation().translate(LIGHT_POS);
    scene->getCamera().setPosition(glm::vec3(0.0f, 10.0f, 0.0f));
    scene->getCamera().setFi(glm::radians(0.0f));
    scene->getCamera().setAlpha(glm::radians(-90.0f));
    scene->getCamera().updateMatrices();
}

float SCALE_EARTH = 1.2f;
float SCALE_MOON = SCALE_EARTH / 5.0f;
float EARTH_SUN_DISTANCE = 15.0f;
float MOON_EARTH_DISTANCE = 2.4f;
float earthOrbitAngle = 0.0f;
float moonOrbitAngle = 0.0f;

void Application::setupScene2(Scene* scene) {
    scene->clearObjects();
    scene->setAmbientLight(glm::vec3(0.1f, 0.1f, 0.1f));
    auto sunMaterial = std::make_shared<Material>();
    sunMaterial->diffuse = glm::vec3(1.0f, 1.0f, 0.6f);
    auto earthMaterial = std::make_shared<Material>();
    earthMaterial->diffuse = glm::vec3(0.0f, 0.0f, 0.8f);
    earthMaterial->specular = glm::vec3(0.5f, 0.5f, 0.5f);
    earthMaterial->shininess = 32.0f;
    auto moonMaterial = std::make_shared<Material>();
    moonMaterial->diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    moonMaterial->specular = glm::vec3(0.1f, 0.1f, 0.1f);
    moonMaterial->shininess = 8.0f;
    scene->addObject(sphere, SPHERE_VERTICES_SIZE, 6);
    DrawableObject* sun = scene->getObject(scene->getObjectCount() - 1);
    sun->setMaterial(sunMaterial);
    sun->setUnlit(true);
    sun->getTransformation().scale(glm::vec3(2.5f));
    sun->getTransformation().translate(glm::vec3(0.0f, 0.0f, 0.0f));
    scene->addPointLight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 10.0f, 10.0f), 1.0f, 0.007f, 0.0002f);
    scene->addObject(sphere, SPHERE_VERTICES_SIZE, 6);
    DrawableObject* earth = scene->getObject(scene->getObjectCount() - 1);
    earth->setMaterial(earthMaterial);
    earth->getTransformation().scale(glm::vec3(SCALE_EARTH));
    earth->getTransformation().translate(glm::vec3(EARTH_SUN_DISTANCE, 0.0f, 0.0f));
    scene->addObject(sphere, SPHERE_VERTICES_SIZE, 6);
    DrawableObject* moon = scene->getObject(scene->getObjectCount() - 1);
    moon->setMaterial(moonMaterial);
    moon->getTransformation().scale(glm::vec3(SCALE_MOON));
    moon->getTransformation().translate(glm::vec3(MOON_EARTH_DISTANCE, 0.0f, 0.0f));
    scene->getCamera().setPosition(glm::vec3(EARTH_SUN_DISTANCE + 5.0f, 3.0f, 5.0f));
    scene->getCamera().setFi(glm::radians(-180.0f));
    scene->getCamera().setAlpha(glm::radians(-10.0f));
    scene->getCamera().updateMatrices();
}

void Application::setupScene3(Scene* scene) {
    scene->clearObjects();
    scene->initGameMaterials();

    auto mat_grass = std::make_shared<Material>();
    mat_grass->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    mat_grass->specular = glm::vec3(0.0f, 0.0f, 0.0f);
    mat_grass->shininess = 16.0f;
    mat_grass->diffuseTextureID = TextureLoader::LoadTexture("assets/multipletexture/grass.png");

    auto mat_skydome = std::make_shared<Material>();
    mat_skydome->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    mat_skydome->diffuseTextureID = TextureLoader::LoadTexture("assets/sky/skydome.png");

    scene->setAmbientLight(glm::vec3(0.3f, 0.3f, 0.3f));
    scene->addDirLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.8f, 0.8f, 0.8f));

    const float sceneSize = 50.0f;

    scene->addObject(plain, sizeof(plain), 8);
    DrawableObject* ground = scene->getObject(scene->getObjectCount() - 1);
    ground->setMaterial(mat_grass);
    ground->getTransformation().scale(glm::vec3(sceneSize / 2.0f));

    scene->addObject("assets/sky/skydome.obj");
    DrawableObject* skydome = scene->getObject(scene->getObjectCount() - 1);
    skydome->setMaterial(mat_skydome);
    skydome->setUnlit(true);
    skydome->getTransformation().scale(glm::vec3(sceneSize * 0.8f));

    std::cout << "================================" << std::endl;
    std::cout << "   HRA WHAC-A-MOLE (3D) START   " << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << "CIL: Klikni na SHREKA (+10 bodu)" << std::endl;
    std::cout << "POZOR: Neklikej na FIONU (-50 bodu)" << std::endl;

    scene->getCamera().setPosition(glm::vec3(0.0f, 12.0f, 25.0f));
    scene->getCamera().setFi(glm::radians(-90.0f));
    scene->getCamera().setAlpha(glm::radians(-25.0f));
    scene->getCamera().updateMatrices();
}

void Application::setupScene4(Scene* s) {
    s->clearObjects();
    s->InitSkybox();
    s->setAmbientLight(glm::vec3(0.2f, 0.2f, 0.2f));
    s->addDirLight(glm::vec3(-0.5f, -1.0f, -0.5f), glm::vec3(0.8f, 0.8f, 0.8f));

    s->getCamera().setPosition(glm::vec3(0.0f, 1.0f, 3.0f));
}