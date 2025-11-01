#pragma once
#include <GL/glew.h>
#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "ICameraObserver.h" 
#include "ILightObserver.h" 

class Shader;
class Camera;
class Light;

class ShaderProgram : public ICameraObserver, public ILightObserver {
private:
    GLuint ID;
    std::unique_ptr<Shader> vs, fs;

    glm::mat4 cachedViewMatrix;
    glm::mat4 cachedProjectionMatrix;

private:
    void checkLinkErrors();
    GLint getUniformLocation(const std::string& name) const;

public:
    static const int MAX_ADDITIONAL_LIGHTS = 8;

    ShaderProgram(Shader& vs, Shader& fs);
    ~ShaderProgram();

    void update(Camera* cam) override;
    void onLightChanged(const Light* light) override;

    void use() const;

    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setVec4(const std::string& name, const glm::vec4& vec) const;
    void setVec3(const std::string& name, const glm::vec3& vec) const;
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;

    void setLightUniforms(const Light& light) const;
    void setAdditionalLights(const std::vector<std::unique_ptr<Light>>& lights) const;

    const glm::mat4& getViewMatrix() const { return cachedViewMatrix; }
    const glm::mat4& getProjectionMatrix() const { return cachedProjectionMatrix; }
};