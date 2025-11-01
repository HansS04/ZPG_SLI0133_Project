#include "Shader.h"
#include <stdexcept>
#include <fstream>
#include <iostream>

void Shader::checkCompileErrors() {
    GLint success;
    glGetShaderiv(this->id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(this->id, 512, NULL, infoLog);

        fprintf(stderr, "SHADER_COMPILATION_ERROR:\n%s\n", infoLog);
        throw std::runtime_error("Shader compilation failed.");
    }
}

void Shader::createShader(GLenum shaderType, const char* shaderCode) {
    GLuint shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, &shaderCode, NULL);
    glCompileShader(shaderID);
    this->id = shaderID;

    checkCompileErrors();
}

Shader::Shader(GLenum shaderType, const char* shaderFile) : id(0)
{
	createShaderFromFile(shaderType, shaderFile);
}

void Shader::createShaderFromFile(GLenum shaderType, const char* shaderFile)
{
    std::ifstream file(shaderFile);
    if (!file.is_open())
    {
        std::cout << "Unable to open file " << shaderFile << std::endl;
        exit(-1);
    }
    std::string shaderCode((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

   createShader(shaderType, shaderCode.c_str());

}



void Shader::attachShader(GLuint program_id)
{
    if (this->id != 0 && program_id != 0) {
        glAttachShader(program_id, this->id);
    }
}