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
    // Start with Scene 2 (Solar System)
    loadScene(2);

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
    sceneInitializers.push_back([this](Scene* s) { setupScene2(s); }); // Solar System
    sceneInitializers.push_back([this](Scene* s) { setupScene3(s); });
    sceneInitializers.push_back([this](Scene* s) { setupScene4(s); });
}

void Application::loadScene(int index) {
    if (index >= 0 && index < sceneInitializers.size()) {
        scene = std::make_unique<Scene>();

        if (index == 3 || index == 4) {
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

extern float earthOrbitAngle;
extern float moonOrbitAngle;

// --- UPRAVENA SCENA 2 PRO SOLRNI SYSTEM ---
void Application::setupScene2(Scene* scene) {
    scene->clearObjects();

    // Tmavsi ambientni svetlo pro vesmir
    scene->setAmbientLight(glm::vec3(0.1f, 0.1f, 0.1f));

    // Skybox pro hvezdy (pokud mate skybox textury nactene v InitSkybox)

    // Zavolame vytvoreni planet (definovano v Scene.cpp)
    scene->initSolarSystem();

    // Nastaveni kamery "Super angle" - vysoko a daleko, pohled na celou soustavu
    // Pozice: X=0, Y=40 (vyska), Z=60 (dalka)
    scene->getCamera().setPosition(glm::vec3(0.0f, 40.0f, 60.0f));

    // Fi = -90 stupnu (kouka smerem k -Z, tedy dopredu do hloubky)
    scene->getCamera().setFi(glm::radians(-90.0f));

    // Alpha = -30 stupnu (kouka sikmo dolu na rovinu orbit)
    scene->getCamera().setAlpha(glm::radians(-30.0f));

    scene->getCamera().updateMatrices();
}

void Application::setupScene3(Scene* scene) {
    scene->clearObjects();

    // Naètení textur a materiálù
    auto mat_grass = std::make_shared<Material>();
    mat_grass->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    mat_grass->specular = glm::vec3(0.0f, 0.0f, 0.0f);
    mat_grass->shininess = 16.0f;
    mat_grass->diffuseTextureID = TextureLoader::LoadTexture("assets/multipletexture/grass.png");

    auto mat_swamp = std::make_shared<Material>();
    mat_swamp->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    mat_swamp->specular = glm::vec3(0.2f, 0.2f, 0.1f);
    mat_swamp->shininess = 8.0f;
    mat_swamp->diffuseTextureID = TextureLoader::LoadTexture("assets/multipletexture/mud.jpg");

    auto mat_shrek = std::make_shared<Material>();
    mat_shrek->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    mat_shrek->specular = glm::vec3(0.1f, 0.1f, 0.1f);
    mat_shrek->shininess = 32.0f;
    mat_shrek->diffuseTextureID = TextureLoader::LoadTexture("assets/shrek/shrek.png");

    auto mat_fiona = std::make_shared<Material>();
    mat_fiona->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    mat_fiona->specular = glm::vec3(0.1f, 0.1f, 0.1f);
    mat_fiona->shininess = 32.0f;
    mat_fiona->diffuseTextureID = TextureLoader::LoadTexture("assets/shrek/fiona.png");

    auto mat_toilet = std::make_shared<Material>();
    mat_toilet->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    mat_toilet->specular = glm::vec3(0.8f, 0.8f, 0.8f);
    mat_toilet->shininess = 128.0f;
    mat_toilet->diffuseTextureID = TextureLoader::LoadTexture("assets/shrek/toiled.jpg");

    auto mat_tree = std::make_shared<Material>();
    mat_tree->diffuse = glm::vec3(0.1f, 0.4f, 0.1f);
    mat_tree->ambient = glm::vec3(0.1f, 0.4f, 0.1f);
    mat_tree->specular = glm::vec3(0.1f, 0.1f, 0.1f);
    mat_tree->shininess = 16.0f;

    auto mat_firefly_body = std::make_shared<Material>();
    mat_firefly_body->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);

    auto mat_skydome = std::make_shared<Material>();
    mat_skydome->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    mat_skydome->diffuseTextureID = TextureLoader::LoadTexture("assets/sky/skydome.png");

    scene->setAmbientLight(glm::vec3(0.02f, 0.02f, 0.05f));
    scene->addDirLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.1f, 0.1f, 0.15f));

    const float sceneSize = 50.0f;
    const float swampSize = 10.0f;
    const float treeScale = 0.5f;

    scene->addObject(plain, sizeof(plain), 8);
    DrawableObject* ground = scene->getObject(scene->getObjectCount() - 1);
    ground->setMaterial(mat_grass);
    ground->getTransformation().scale(glm::vec3(sceneSize / 2.0f));

    scene->addObject(plain, sizeof(plain), 8);
    DrawableObject* swamp = scene->getObject(scene->getObjectCount() - 1);
    swamp->setMaterial(mat_swamp);
    swamp->getTransformation()
        .scale(glm::vec3(swampSize / 2.0f))
        .translate(glm::vec3(0.0f, 0.01f, 0.0f));

    scene->addObject("assets/shrek/shrek.obj");
    DrawableObject* shrek = scene->getObject(scene->getObjectCount() - 1);
    shrek->setMaterial(mat_shrek);
    shrek->getTransformation()
        .translate(glm::vec3(-1.0f, 0.02f, 0.0f))
        .scale(glm::vec3(0.5f))
        .addMatrix20();//Transformation 

    scene->addObject("assets/shrek/fiona.obj");
    DrawableObject* fiona = scene->getObject(scene->getObjectCount() - 1);
    fiona->setMaterial(mat_fiona);
    fiona->getTransformation()
        .translate(glm::vec3(1.0f, 0.02f, 0.0f))
        .scale(glm::vec3(0.5f));

    scene->addObject("assets/shrek/toiled.obj");
    DrawableObject* toilet = scene->getObject(scene->getObjectCount() - 1);
    toilet->setMaterial(mat_toilet);
    toilet->getTransformation()
        .translate(glm::vec3(0.0f, 0.02f, 0.0f))
        .scale(glm::vec3(0.5f));

    scene->addObject("assets/sky/skydome.obj");
    DrawableObject* skydome = scene->getObject(scene->getObjectCount() - 1);
    skydome->setMaterial(mat_skydome);
    skydome->setUnlit(true);
    skydome->getTransformation().scale(glm::vec3(sceneSize * 0.8f));

    const int numTrees = 150;
    std::uniform_real_distribution<float> randPos(-sceneSize / 2.0f, sceneSize / 2.0f);

    for (int i = 0; i < numTrees; ++i) {
        float x = randPos(m_RandomEngine);
        float z = randPos(m_RandomEngine);

        if (std::abs(x) < (swampSize / 2.0f + 1.0f) && std::abs(z) < (swampSize / 2.0f + 1.0f)) {
            continue;
        }

        scene->addObject(tree, TREE_DATA_SIZE, 6);
        DrawableObject* treeObj = scene->getObject(scene->getObjectCount() - 1);
        treeObj->setMaterial(mat_tree);
        treeObj->getTransformation()
            .translate(glm::vec3(x, 0.0f, z))
            .scale(glm::vec3(treeScale));
    }

    const int numFireflies = 15;
    const glm::vec3 fireflyLightColor = glm::vec3(1.0f, 1.2f, 0.5f);
    const float con = 1.0f;
    const float lin = 1.0f;
    const float quad = 2.0f;
    std::uniform_real_distribution<float> randEdge(-swampSize / 2.0f, swampSize / 2.0f);
    std::uniform_real_distribution<float> randY_dist(0.5f, 2.0f);
    std::uniform_int_distribution<int> randSide(0, 3);

    for (int i = 0; i < numFireflies; ++i) {
        float x = 0.0f, z = 0.0f;
        int side = randSide(m_RandomEngine);
        if (side == 0) { x = -swampSize / 2.0f; z = randEdge(m_RandomEngine); }
        else if (side == 1) { x = swampSize / 2.0f; z = randEdge(m_RandomEngine); }
        else if (side == 2) { z = -swampSize / 2.0f; x = randEdge(m_RandomEngine); }
        else { z = swampSize / 2.0f; x = randEdge(m_RandomEngine); }
        float y = randY_dist(m_RandomEngine);

        scene->addFirefly(glm::vec3(x, y, z), fireflyLightColor, con, lin, quad);

        scene->addObject(sphere, SPHERE_VERTICES_SIZE, 6);
        DrawableObject* fireflyBody = scene->getObject(scene->getObjectCount() - 1);
        fireflyBody->setMaterial(mat_firefly_body);
        fireflyBody->setUnlit(true);
        scene->addFireflyBody(fireflyBody);
    }

    scene->getCamera().setPosition(glm::vec3(0.0f, 3.0f, 15.0f));
}

void Application::setupScene4(Scene* scene) {
    scene->clearObjects();
    scene->initGameMaterials();

    std::string nick;
    std::cout << "\n========================================" << std::endl;
    std::cout << "Zadejte svuj NICK pro start hry: ";
    std::cin >> nick;
    scene->setPlayerName(nick);
    std::cout << "========================================\n" << std::endl;

    auto mat_grass = std::make_shared<Material>();
    mat_grass->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    mat_grass->specular = glm::vec3(0.0f, 0.0f, 0.0f);
    mat_grass->shininess = 16.0f;
    mat_grass->diffuseTextureID = TextureLoader::LoadTexture("assets/multipletexture/grass.png");

    auto mat_skydome = std::make_shared<Material>();
    mat_skydome->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    mat_skydome->diffuseTextureID = TextureLoader::LoadTexture("assets/sky/skydome.png");

    auto mat_firefly = std::make_shared<Material>();
    mat_firefly->diffuse = glm::vec3(1.0f, 1.0f, 0.5f);

    scene->setAmbientLight(glm::vec3(0.15f, 0.15f, 0.25f));
    scene->addDirLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.4f, 0.4f, 0.5f));

    const float sceneSize = 80.0f;

    scene->addObject(plain, sizeof(plain), 8);
    DrawableObject* ground = scene->getObject(scene->getObjectCount() - 1);
    ground->setMaterial(mat_grass);
    ground->getTransformation().scale(glm::vec3(sceneSize));

    scene->addObject("assets/sky/skydome.obj");
    DrawableObject* skydome = scene->getObject(scene->getObjectCount() - 1);
    skydome->setMaterial(mat_skydome);
    skydome->setUnlit(true);
    skydome->getTransformation().scale(glm::vec3(sceneSize * 1.2f));

    scene->initForest();

    const int numFireflies = 10;
    for (int i = 0; i < numFireflies; ++i) {
        float x = (std::rand() % 40) - 20.0f;
        float z = (std::rand() % 40) - 20.0f;
        float y = 2.0f + (std::rand() % 20) / 10.0f;

        scene->addFirefly(glm::vec3(x, y, z), glm::vec3(1.0f, 0.8f, 0.2f), 1.0f, 0.7f, 1.8f);

        scene->addObject(sphere, SPHERE_VERTICES_SIZE, 6);
        DrawableObject* ffBody = scene->getObject(scene->getObjectCount() - 1);
        ffBody->setMaterial(mat_firefly);
        ffBody->setUnlit(true);
        ffBody->getTransformation().translate(glm::vec3(x, y, z)).scale(glm::vec3(0.15f));

        scene->addFireflyBody(ffBody);
    }

    scene->getCamera().setPosition(glm::vec3(0.0f, 20.0f, 40.0f));
    scene->getCamera().setFi(glm::radians(-90.0f));
    scene->getCamera().setAlpha(glm::radians(-30.0f));
    scene->getCamera().updateMatrices();
}