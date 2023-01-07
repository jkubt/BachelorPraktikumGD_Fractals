#pragma once

#include <string>

#include "defines.h"
#define GLEW_STATIC
#include <GL/glew.h>

struct Shader {
	Shader(const char* vertexShaderFilename, const char* fragmentShaderFilename);
	virtual ~Shader();

	void bind();
	void unbind();

	GLuint getShaderId() { return shaderId; };
	void setColorUniform(float r, float g, float b, float a);
private:
	std::string parse(const char* filename);
	GLuint compile(std::string shaderSource, GLenum type);
	GLuint createShader(const char* vertexShaderFilename, const char* fragmentShaderFilename);

	GLuint shaderId;
	int colorUniformLocation;
};

