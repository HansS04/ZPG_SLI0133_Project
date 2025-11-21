#pragma once
#include <GL/glew.h>
#include <cstddef>

class Model {
private:
    GLuint vao, vbo;
    GLint gleumMode;
    int count;
    int m_Stride; 

public:
    Model(const float* points, size_t size, int stride, GLint mode = GL_TRIANGLES);
    Model(const char* name);
    ~Model();
    void draw() const;
};