#include <iostream>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>

#pragma comment(lib,"SDL2.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib,"opengl32.lib")

#include "window.h"
#include "defines.h"
#include "vertexBuffer.h"
#include "shader.h"

int main(int argc, char** argv){
	
	Window window(800, 600, "FractalGenerator");

	Vertex vertices[] = {
		Vertex{-0.5f, -0.5f, 0.0f},
		Vertex{0.0f, 0.5f, 0.0f},
		Vertex{0.5f, -0.5f, 0.0f}
	};
	uint32_t numVertices = sizeof(vertices)/sizeof(Vertex);

	VertexBuffer vertexBuffer(vertices, numVertices);

	Shader shader("shader.vert", "shader.frac");
	shader.bind();

	while (!window.getClose()) {
		window.clear(0.0f, 0.0f, 0.0f, 1.0f);

		vertexBuffer.bind();
		glDrawArrays(GL_TRIANGLES, 0, numVertices);
		vertexBuffer.unbind();

		window.update();
	}

	return 0;
}

