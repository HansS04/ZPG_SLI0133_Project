#pragma once
#include <GL/glew.h>
#include <cstddef>

class Model {
private:
    GLuint vao, vbo;
    GLint gleumMode;
    int count;

public:
    Model(const float* points, size_t size, GLint mode = GL_TRIANGLES);
    ~Model();
    void draw() const;
};