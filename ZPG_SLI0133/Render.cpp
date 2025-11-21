#include "Render.h"

#include "Application.h"
#include "InputController.h"
#include "Scene.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

Render::Render(Application& app)
    : m_App(app) {
}

void Render::run() {
    GLFWwindow* window = m_App.getWindow();
    InputController* controller = m_App.getController();

    float lastTime = 0.0f;

    glClearStencil(0);

    while (!glfwWindowShouldClose(window)) {
        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        if (controller) {
            controller->processPollingInput(deltaTime);
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        Scene* scene = m_App.getActiveScene();
        int currentSceneIndex = m_App.getCurrentSceneIndex();

        if (scene) {
            scene->update(deltaTime, currentSceneIndex);
            scene->render();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}