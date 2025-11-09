#pragma once
#include <string>
#include <vector>
#include <GL/glew.h>

class TextureLoader {
public:
    static GLuint LoadTexture(const std::string& path, bool flip = true);
    static GLuint loadCubemap(std::vector<std::string> faces);
};