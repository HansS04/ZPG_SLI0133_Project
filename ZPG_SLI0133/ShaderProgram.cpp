#include "ShaderProgram.h"
#include "Shader.h"
#include "Camera.h"
#include "Light.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <string>

ShaderProgram::ShaderProgram(Shader& vs, Shader& fs) {
    ID = glCreateProgram();
    vs.attachShader(ID);
    fs.attachShader(ID);
    glLinkProgram(ID);
    checkLinkErrors();
}

ShaderProgram::~ShaderProgram() {
    if (ID != 0) {
        glDeleteProgram(ID);
    }
}

void ShaderProgram::use() const {
    glUseProgram(ID);
}

void ShaderProgram::checkLinkErrors() {
    GLint success;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cerr << "SHADER_PROGRAM_LINKING_ERROR:\n" << infoLog << std::endl;
        throw std::runtime_error("Shader program linking failed.");
    }
}

GLint ShaderProgram::getUniformLocation(const std::string& name) const {
    GLint location = glGetUniformLocation(ID, name.c_str());
    return location;
}

void ShaderProgram::update(Camera* cam) {
    if (cam) {
        cachedViewMatrix = cam->getViewMatrix();
        cachedProjectionMatrix = cam->getProjectionMatrix();
    }
}

void ShaderProgram::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::setVec4(const std::string& name, const glm::vec4& vec) const {
    glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(vec));
}


void ShaderProgram::setVec3(const std::string& name, const glm::vec3& vec) const {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(vec));
}

void ShaderProgram::setFloat(const std::string& name, float value) const {
    glUniform1f(getUniformLocation(name), value);
}

void ShaderProgram::setInt(const std::string& name, int value) const {
    glUniform1i(getUniformLocation(name), value);
}

void ShaderProgram::setBool(const std::string& name, bool value) const {
    glUniform1i(getUniformLocation(name), (int)value);
}

void ShaderProgram::setAmbientLight(const glm::vec3& color) const {
    setVec3("u_AmbientLight", color);
}

void ShaderProgram::setDirLight(const DirLight& light, bool on) const {
    setBool("u_DirLightOn", on);
    setVec3("u_DirLight.direction", light.direction);
    setVec3("u_DirLight.color", light.color);
}

void ShaderProgram::setFlashlight(const SpotLight& light, bool on) const {
    setBool("u_FlashlightOn", on);
    setVec3("u_Flashlight.position", light.position);
    setVec3("u_Flashlight.direction", light.direction);
    setVec3("u_Flashlight.color", light.color);
    setFloat("u_Flashlight.constant", light.constant);
    setFloat("u_Flashlight.linear", light.linear);
    setFloat("u_Flashlight.quadratic", light.quadratic);
    setFloat("u_Flashlight.cutOff", light.cutOff);
    setFloat("u_Flashlight.outerCutOff", light.outerCutOff);
}

void ShaderProgram::setPointLights(const std::vector<std::unique_ptr<PointLight>>& lights) const {
    int lightCount = static_cast<int>(lights.size());
    if (lightCount > MAX_POINT_LIGHTS) {
        std::cerr << "Varovani: Pocet bodovych svetel (" << lights.size()
            << ") prekracuje shader limit (" << MAX_POINT_LIGHTS << ")." << std::endl;
        lightCount = MAX_POINT_LIGHTS;
    }

    setInt("u_PointLightCount", lightCount);

    for (int i = 0; i < lightCount; ++i) {
        const auto& light = lights[i];
        std::string baseName = "u_PointLights[" + std::to_string(i) + "].";

        setVec3(baseName + "position", light->position);
        setVec3(baseName + "color", light->color);
        setFloat(baseName + "constant", light->constant);
        setFloat(baseName + "linear", light->linear);
        setFloat(baseName + "quadratic", light->quadratic);
    }
}