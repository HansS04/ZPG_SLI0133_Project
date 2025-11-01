#include "Application.h"
#include "Scene.h"
#include "DrawableObject.h" 
#include "InputController.h" 
#include "Render.h"
#include "Camera.h"
#include "Shader.h"
#include "TransformationComposite.h"
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

extern const float SPHERE_VERTICES[];
extern const size_t SPHERE_VERTICES_SIZE;
extern const float PLAIN_VERTICES[];
extern const size_t PLAIN_VERTICES_SIZE;

float rotationSpeed = 5.0f;
float rotationAngle = 0.0f;
bool direction = false;

const float TEST_TRIANGLE[] = {
    0.0f,  1.0f,  0.0f,     0.0f, 0.0f, 1.0f,
   -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 0.0f,     0.0f, 0.0f, 1.0f
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
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
    glViewport(0, 0, width, height);

    setupScenes();
    loadScene(0);

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
}

void Application::loadScene(int index) {
    if (index == currentScene) return;
    if (index >= 0 && index < sceneInitializers.size()) {
        scene = std::make_unique<Scene>();
        scene->createShaders("basic_vertexShader.vert", "basic_Blinn_fragmentShader.frag");
        sceneInitializers[index](scene.get());
        currentScene = index;
        std::cout << "Nactena scena: " << index << std::endl;
    }
}

void Application::setupScene0(Scene* s) {
    s->setAmbientLight(glm::vec3(0.1f, 0.1f, 0.1f));
    s->setDirLight(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f), false);

    s->addObject(TEST_TRIANGLE, TEST_TRIANGLE_SIZE, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    DrawableObject* obj = s->getFirstObject();
    if (obj) {
        obj->getTransformation().translate(glm::vec3(0.0f, 1.0f, 0.0f));
    }
}

const size_t SPHERE_VERTICES_SIZE = sizeof(sphere);

void Application::setupScene1(Scene* scene) {
    scene->clearObjects();

    scene->setAmbientLight(glm::vec3(0.1f, 0.1f, 0.1f));

    const glm::vec4 SPHERE_COLOR(0.9f, 0.9f, 0.9f, 1.0f);
    const glm::vec4 LIGHT_BULB_COLOR(1.0f, 1.0f, 1.0f, 1.0f);
    const float OBJECT_RADIUS = 1.0f;
    const float OBJECT_OFFSET = 1.5f;
    const float OBJECT_Y_POS = 0.0f;
    const glm::vec3 LIGHT_POS(0.0f, 3.0f, 0.0f);

    scene->addObject(sphere, SPHERE_VERTICES_SIZE, SPHERE_COLOR);
    DrawableObject* s1 = scene->getObject(scene->getObjectCount() - 1);
    s1->getTransformation().scale(glm::vec3(OBJECT_RADIUS));
    s1->getTransformation().translate(glm::vec3(-OBJECT_OFFSET, OBJECT_Y_POS, OBJECT_OFFSET));

    scene->addObject(sphere, SPHERE_VERTICES_SIZE, SPHERE_COLOR);
    DrawableObject* s2 = scene->getObject(scene->getObjectCount() - 1);
    s2->getTransformation().scale(glm::vec3(OBJECT_RADIUS));
    s2->getTransformation().translate(glm::vec3(OBJECT_OFFSET, OBJECT_Y_POS, OBJECT_OFFSET));

    scene->addObject(sphere, SPHERE_VERTICES_SIZE, SPHERE_COLOR);
    DrawableObject* s3 = scene->getObject(scene->getObjectCount() - 1);
    s3->getTransformation().scale(glm::vec3(OBJECT_RADIUS));
    s3->getTransformation().translate(glm::vec3(-OBJECT_OFFSET, OBJECT_Y_POS, -OBJECT_OFFSET));

    scene->addObject(sphere, SPHERE_VERTICES_SIZE, SPHERE_COLOR);
    DrawableObject* s4 = scene->getObject(scene->getObjectCount() - 1);
    s4->getTransformation().scale(glm::vec3(OBJECT_RADIUS));
    s4->getTransformation().translate(glm::vec3(OBJECT_OFFSET, OBJECT_Y_POS, -OBJECT_OFFSET));

    scene->addPointLight(LIGHT_POS, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f);

    scene->addObject(sphere, SPHERE_VERTICES_SIZE, LIGHT_BULB_COLOR);
    DrawableObject* lightBulb = scene->getObject(scene->getObjectCount() - 1);
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

    scene->setAmbientLight(glm::vec3(0.2f, 0.2f, 0.2f));

    const float SCALE_SUN = 2.5f;
    const glm::vec4 COLOR_SUN_BRIGHT(1.0f, 1.0f, 0.6f, 1.0f);
    const glm::vec4 COLOR_EARTH(0.0f, 0.0f, 0.8f, 1.0f);
    const glm::vec4 COLOR_MOON(0.5f, 0.5f, 0.5f, 1.0f);

    scene->addObject(sphere, SPHERE_VERTICES_SIZE, COLOR_SUN_BRIGHT);
    DrawableObject* sun = scene->getObject(scene->getObjectCount() - 1);
    sun->getTransformation().scale(glm::vec3(SCALE_SUN));
    sun->getTransformation().translate(glm::vec3(0.0f, 0.0f, 0.0f));

    scene->addPointLight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 10.0f, 10.0f), 1.0f, 0.007f, 0.0002f);

    scene->addObject(sphere, SPHERE_VERTICES_SIZE, COLOR_EARTH);
    DrawableObject* earth = scene->getObject(scene->getObjectCount() - 1);
    earth->getTransformation().scale(glm::vec3(SCALE_EARTH));
    earth->getTransformation().translate(glm::vec3(EARTH_SUN_DISTANCE, 0.0f, 0.0f));

    scene->addObject(sphere, SPHERE_VERTICES_SIZE, COLOR_MOON);
    DrawableObject* moon = scene->getObject(scene->getObjectCount() - 1);
    moon->getTransformation().scale(glm::vec3(SCALE_MOON));
    moon->getTransformation().translate(glm::vec3(MOON_EARTH_DISTANCE, 0.0f, 0.0f));

    scene->getCamera().setPosition(glm::vec3(EARTH_SUN_DISTANCE + 5.0f, 3.0f, 5.0f));
    scene->getCamera().setFi(glm::radians(-180.0f));
    scene->getCamera().setAlpha(glm::radians(-10.0f));
    scene->getCamera().updateMatrices();
}

extern const size_t BENCH_DATA_SIZE;

void Application::setupScene3(Scene* scene) {
    const glm::vec4 BROWN = glm::vec4(0.55f, 0.27f, 0.07f, 1.0f);
    const glm::vec4 BEIGE = glm::vec4(0.85f, 0.75f, 0.60f, 1.0f);
    const glm::vec4 FOREST_GREEN = glm::vec4(0.1f, 0.35f, 0.1f, 1.0f);
    const glm::vec4 TREE_BUSH_COLOR = glm::vec4(0.1f, 0.4f, 0.1f, 1.0f);
    const float SCENE_LENGTH = 100.0f;
    const float PATH_WIDTH = 2.0f;
    const float FOREST_WIDTH = 10.0f;
    const int NUM_TREES = 75;
    const int NUM_BUSHES = 75;
    const int NUM_BENCHES = 5;
    const float OBJ_SCALE = 0.35f;
    const float BENCH_SCALE = 0.6f;
    const float PATH_Y_OFFSET = 0.001f;

    scene->addObject(plain, sizeof(plain), FOREST_GREEN);
    DrawableObject* groundObj = scene->getObject(scene->getObjectCount() - 1);
    groundObj->getTransformation().scale(glm::vec3(FOREST_WIDTH, 1.0f, SCENE_LENGTH / 2.0f));

    scene->addObject(plain, sizeof(plain), BEIGE);
    DrawableObject* pathObj = scene->getObject(scene->getObjectCount() - 1);
    pathObj->getTransformation().scale(glm::vec3(PATH_WIDTH / 2.0f, 1.0f, SCENE_LENGTH / 2.0f));
    pathObj->getTransformation().translate(glm::vec3(0.0f, PATH_Y_OFFSET, 0.0f));

    const float MIN_X_DIST = PATH_WIDTH / 2.0f + 1.0f;
    const float MAX_X_DIST = FOREST_WIDTH - 0.5f;

    std::uniform_real_distribution<float> randZ_dist(-SCENE_LENGTH / 2.0f, SCENE_LENGTH / 2.0f);
    std::uniform_real_distribution<float> randX_dist(MIN_X_DIST, MAX_X_DIST);
    std::uniform_int_distribution<int> randSign_dist(0, 1);

    for (int i = 0; i < NUM_TREES; ++i) {
        float randZ = randZ_dist(m_RandomEngine);
        float randXOffset = randX_dist(m_RandomEngine);
        if (randSign_dist(m_RandomEngine) == 0) randXOffset = -randXOffset;

        scene->addObject(tree, sizeof(tree), TREE_BUSH_COLOR);
        DrawableObject* treeObj = scene->getObject(scene->getObjectCount() - 1);
        treeObj->getTransformation().translate(glm::vec3(randXOffset, 0.0f, randZ));
        treeObj->getTransformation().scale(glm::vec3(OBJ_SCALE));
    }
    for (int i = 0; i < NUM_BUSHES; ++i) {
        float randZ = randZ_dist(m_RandomEngine);
        float randXOffset = randX_dist(m_RandomEngine);
        if (randSign_dist(m_RandomEngine) == 0) randXOffset = -randXOffset;

        scene->addObject(bushes, sizeof(bushes), TREE_BUSH_COLOR);
        DrawableObject* bushObj = scene->getObject(scene->getObjectCount() - 1);
        bushObj->getTransformation().translate(glm::vec3(randXOffset, 0.0f, randZ));
        bushObj->getTransformation().scale(glm::vec3(OBJ_SCALE));
    }

    const float BENCH_SPACING = SCENE_LENGTH / (NUM_BENCHES + 1);
    float currentBenchZ = -(SCENE_LENGTH / 2.0f) + BENCH_SPACING;
    const float BENCH_X_OFFSET = PATH_WIDTH / 2.0f + 0.1f;
    for (int i = 0; i < NUM_BENCHES; ++i) {
        scene->addObject(bench, BENCH_DATA_SIZE, BROWN);
        DrawableObject* benchL = scene->getObject(scene->getObjectCount() - 1);
        benchL->getTransformation().translate(glm::vec3(-BENCH_X_OFFSET - 0.75f, PATH_Y_OFFSET, currentBenchZ));
        benchL->getTransformation().scale(glm::vec3(BENCH_SCALE));
        benchL->getTransformation().rotate(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        scene->addObject(bench, BENCH_DATA_SIZE, BROWN);
        DrawableObject* benchR = scene->getObject(scene->getObjectCount() - 1);
        benchR->getTransformation().translate(glm::vec3(BENCH_X_OFFSET + 0.75f, PATH_Y_OFFSET, currentBenchZ));
        benchR->getTransformation().scale(glm::vec3(BENCH_SCALE));
        benchR->getTransformation().rotate(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        currentBenchZ += BENCH_SPACING;
    }

    scene->setAmbientLight(glm::vec3(0.02f, 0.02f, 0.05f));
    scene->setDirLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.1f, 0.1f, 0.15f), true);

    std::cout << "Pridavam svetlusky do sceny 3..." << std::endl;
    const int numFireflies = 8;
    const glm::vec3 fireflyColor = glm::vec3(4.0f, 5.0f, 2.0f);
    const float con = 1.0f;
    const float lin = 0.7f;
    const float quad = 1.8f;

    std::uniform_real_distribution<float> randY_dist(0.5f, 2.5f);

    for (int i = 0; i < numFireflies; ++i) {
        float x = randX_dist(m_RandomEngine);
        if (randSign_dist(m_RandomEngine) == 0) x = -x;
        float y = randY_dist(m_RandomEngine);
        float z = randZ_dist(m_RandomEngine);
        scene->addPointLight(glm::vec3(x, y, z), fireflyColor, con, lin, quad);
    }

    scene->getCamera().setPosition(glm::vec3(0.0f, 1.7f, 5.0f));
}