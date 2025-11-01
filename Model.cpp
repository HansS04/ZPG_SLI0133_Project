#include "Model.h"
#include <iostream>

Model::Model(const float* points, size_t size, GLint mode)
    : gleumMode(mode), count(size / sizeof(float) / 6)
{
    if (size == 0 || count == 0) {
        std::cerr << "!!! CHYBA MODELU: Velikost dat ('size') je nulova, nebo se nepodarilo spocetat vertexy. Model NEBYL NACTEN!" << std::endl;
        std::cerr << "   Kontrola: Size: " << size << ", Count: " << count << std::endl;
    }
    else {
        std::cout << "Model nahran: Velikost dat (bytu): " << size
            << ", Pocet vertexu ('count'): " << count << std::endl;
    }

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, points, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    int stride = 6 * sizeof(float);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(3 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Model::~Model() {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void Model::draw() const {
    glBindVertexArray(vao);
    glDrawArrays(gleumMode, 0, count);
    glBindVertexArray(0);
}