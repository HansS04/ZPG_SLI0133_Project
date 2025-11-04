#include "ShaderProgram.h"
#include "Shader.h"
#include "Camera.h"
#include "Lights.h"
#include "Material.h"
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

void ShaderProgram::setLights(const std::vector<std::unique_ptr<Light>>& lights) const {
    int dirLightIndex = 0;
    int pointLightIndex = 0;

    for (const auto& light : lights) {
        if (DirLight* dLight = dynamic_cast<DirLight*>(light.get())) {
            if (dirLightIndex < MAX_DIR_LIGHTS) {
                std::string base = "u_DirLights[" + std::to_string(dirLightIndex) + "].";
                setVec3(base + "direction", dLight->direction);
                setVec3(base + "color", dLight->color);
                dirLightIndex++;
            }
        }
        else if (PointLight* pLight = dynamic_cast<PointLight*>(light.get())) {
            if (pointLightIndex < MAX_POINT_LIGHTS) {
                std::string base = "u_PointLights[" + std::to_string(pointLightIndex) + "].";
                setVec3(base + "position", pLight->position);
                setVec3(base + "color", pLight->color);
                setFloat(base + "constant", pLight->constant);
                setFloat(base + "linear", pLight->linear);
                setFloat(base + "quadratic", pLight->quadratic);
                pointLightIndex++;
            }
        }
    }

    setInt("u_DirLightCount", dirLightIndex);
    setInt("u_PointLightCount", pointLightIndex);
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

void ShaderProgram::setSpotLights(const std::vector<std::unique_ptr<SpotLight>>& lights) const {
    int lightCount = static_cast<int>(lights.size());
    if (lightCount > MAX_SPOT_LIGHTS) {
        lightCount = MAX_SPOT_LIGHTS;
    }
    setInt("u_SpotLightCount", lightCount);

    for (int i = 0; i < lightCount; ++i) {
        const auto& light = lights[i];
        std::string base = "u_SpotLights[" + std::to_string(i) + "].";
        setVec3(base + "position", light->position);
        setVec3(base + "direction", light->direction);
        setVec3(base + "color", light->color);
        setFloat(base + "constant", light->constant);
        setFloat(base + "linear", light->linear);
        setFloat(base + "quadratic", light->quadratic);
        setFloat(base + "cutOff", light->cutOff);
        setFloat(base + "outerCutOff", light->outerCutOff);
    }
}

void ShaderProgram::setMaterial(const Material& mat) const {
    setVec3("u_Material.ambient", mat.ambient);
    setVec3("u_Material.diffuse", mat.diffuse);
    setVec3("u_Material.specular", mat.specular);
    setFloat("u_Material.shininess", mat.shininess);
}