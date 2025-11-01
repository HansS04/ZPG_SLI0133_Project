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
    if (location == -1) {
    }
    return location;
}

void ShaderProgram::update(Camera* cam) {
    if (cam) {
        cachedViewMatrix = cam->getViewMatrix();
        cachedProjectionMatrix = cam->getProjectionMatrix();
    }
}

void ShaderProgram::onLightChanged(const Light* light) {
    if (light) {
        this->use();
        this->setLightUniforms(*light);
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

void ShaderProgram::setLightUniforms(const Light& light) const {
    setVec3("u_PointLight.position", light.getPosition());
    setVec3("u_PointLight.color", light.getColor());

    setFloat("u_PointLight.constant", light.getConstant());
    setFloat("u_PointLight.linear", light.getLinear());
    setFloat("u_PointLight.quadratic", light.getQuadratic());
}

void ShaderProgram::setAdditionalLights(const std::vector<std::unique_ptr<Light>>& lights) const {

    int lightCount = static_cast<int>(lights.size());
    if (lightCount > MAX_ADDITIONAL_LIGHTS) {
        std::cerr << "Varovani: Pocet dalsich svetel (" << lights.size()
            << ") prekracuje shader limit (" << MAX_ADDITIONAL_LIGHTS << ")." << std::endl;
        lightCount = MAX_ADDITIONAL_LIGHTS;
    }

    setInt("u_AdditionalLightCount", lightCount);

    for (int i = 0; i < lightCount; ++i) {
        const auto& light = lights[i];
        std::string baseName = "u_AdditionalLights[" + std::to_string(i) + "].";

        setVec3(baseName + "position", light->getPosition());
        setVec3(baseName + "color", light->getColor());
        setFloat(baseName + "constant", light->getConstant());
        setFloat(baseName + "linear", light->getLinear());
        setFloat(baseName + "quadratic", light->getQuadratic());
    }
}