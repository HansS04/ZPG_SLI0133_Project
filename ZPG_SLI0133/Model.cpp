#include "Model.h"
#include <iostream>
#include <vector>
#include <stdexcept>
#include "tiny_obj_loader.h" 

Model::Model(const float* points, size_t size, int stride, GLint mode)
    : gleumMode(mode), m_Stride(stride)
{
    this->count = static_cast<int>(size / sizeof(float) / m_Stride);

    if (size == 0 || count == 0) {
        std::cerr << "!!! CHYBA MODELU: Velikost dat ('size') je nulova, nebo se nepodarilo spocetat vertexy. Model NEBYL NACTEN!" << std::endl;
    }
    else {
        std::cout << "Model nahran: Velikost dat (bytu): " << size
            << ", Pocet vertexu ('count'): " << count << ", Stride: " << m_Stride << std::endl;
    }

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, points, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    int stride_bytes = m_Stride * sizeof(float);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride_bytes, (GLvoid*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride_bytes, (GLvoid*)(3 * sizeof(float)));

    if (m_Stride == 8) {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride_bytes, (GLvoid*)(6 * sizeof(float)));
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Model::Model(const char* name)
    : gleumMode(GL_TRIANGLES), count(0), m_Stride(0)
{
    std::string inputfile = name;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str(), "../assets/");
    if (!warn.empty()) std::cout << "Warn: " << warn << std::endl;
    if (!err.empty()) std::cerr << "Err: " << err << std::endl;
    if (!ret) throw std::runtime_error("Failed to load OBJ file!");

    std::vector<float> vertices;
    bool hasUVs = !attrib.texcoords.empty();
    m_Stride = hasUVs ? 8 : 6;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

            if (index.normal_index >= 0) {
                vertices.push_back(attrib.normals[3 * index.normal_index + 0]);
                vertices.push_back(attrib.normals[3 * index.normal_index + 1]);
                vertices.push_back(attrib.normals[3 * index.normal_index + 2]);
            }
            else {
                vertices.push_back(0.0f);
                vertices.push_back(1.0f);
                vertices.push_back(0.0f);
            }

            if (m_Stride == 8) {
                if (index.texcoord_index >= 0) {
                    vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
                    vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);
                }
                else {
                    vertices.push_back(0.0f);
                    vertices.push_back(0.0f);
                }
            }
        }
    }

    if (vertices.empty()) {
        std::cerr << "Model " << name << " neobsahuje zadne vertexy." << std::endl;
        return;
    }

    this->count = static_cast<int>(vertices.size()) / m_Stride;

    std::cout << "Model z OBJ nahran: " << name
        << ", Pocet vertexu ('count'): " << this->count << ", Stride: " << m_Stride << std::endl;

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    int stride_bytes = m_Stride * sizeof(float);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride_bytes, (GLvoid*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride_bytes, (GLvoid*)(3 * sizeof(float)));

    if (m_Stride == 8) {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride_bytes, (GLvoid*)(6 * sizeof(float)));
    }

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