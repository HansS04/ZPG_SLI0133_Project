#pragma once
#include <GL/glew.h>
#include <cstddef>

class Model {
private:
    GLuint vao, vbo;
    GLint gleumMode;
    int count;
    int m_Stride; // Kolik floatù má jeden vrchol (6 nebo 8)

public:
    Model(const float* points, size_t size, int stride, GLint mode = GL_TRIANGLES);
    Model(const char* name); // Tento konstruktor si stride zjistí sám
    ~Model();
    void draw() const;
};