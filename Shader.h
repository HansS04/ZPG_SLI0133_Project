#pragma once
#include <GL/glew.h>
#include <stdlib.h>
#include <stdio.h>

class Shader
{
public:
	Shader(GLenum shaderType, const char* shaderFile);
	void createShaderFromFile(GLenum shaderType, const char* shaderFile);
	void createShader(GLenum shaderType, const char* shaderCode);
	void attachShader(GLuint shader_id);
private:
	GLuint id;
	void checkCompileErrors();
};