#pragma once
#include <GL/glew.h>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "ICameraObserver.h" 
#include "ILightObserver.h" // <-- NOVÝ INCLUDE

class Shader;
class Camera;
class Light;

// ShaderProgram nyní implementuje OBÌ rozhraní
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
    ShaderProgram(Shader& vs, Shader& fs);
    ~ShaderProgram();

    // Metoda z ICameraObserver
    void update(Camera* cam) override;

    // NOVÁ METODA z ILightObserver
    void onLightChanged(const Light* light) override;

    void use() const;

    // ... (všechny settery: setMat4, setVec4, setLightUniforms, atd. zùstávají) ...
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setVec4(const std::string& name, const glm::vec4& vec) const;
    void setVec3(const std::string& name, const glm::vec3& vec) const;
    void setFloat(const std::string& name, float value) const;
    void setLightUniforms(const Light& light) const;

    const glm::mat4& getViewMatrix() const { return cachedViewMatrix; }
    const glm::mat4& getProjectionMatrix() const { return cachedProjectionMatrix; }
};