#include <iostream>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#pragma comment(lib,"SDL2.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib,"opengl32.lib")

#include "window.h"
#include "defines.h"
#include "vertexBuffer.h"
#include "shader.h"
#include "sierpinskiTriangle.h"

void GLAPIENTRY openGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::cout << "[OpenGL Error] " << message << std::endl;
}

int main(int argc, char** argv) {

	Window window(1024, 576, "FractalGenerator");

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openGLDebugCallback, 0);
#endif

	Vertex border[] = {
		Vertex{10.0f, 100.0f, 0.0f},
		Vertex{810.0f, 100.0f, 0.0f},
		Vertex{810.0f, 800.0f, 0.0f},
		Vertex{10.0f, 800.0f, 0.0f}
	};
	uint32_t numVerticesBorder = sizeof(border) / sizeof(Vertex);

	VertexBuffer borderBuffer(border, numVerticesBorder);

	Vertex fractalBounds[] = {
		Vertex{0.0f, 900.0f, 0.0f},
		Vertex{10.0f, 900.0f, 0.0f},
		Vertex{0.0f, 0.0f, 0.0f},
		Vertex{10.0f, 100.0f, 0.0f},
		Vertex{10.0f, 0.0f, 0.0f},
		Vertex{1600.0f, 100.0f, 0.0f},
		Vertex{1600.0f, 0.0f, 0.0f},
		Vertex{1600.0f, 900.0f, 0.0f},
		Vertex{810.0f, 0.0f, 0.0f},
		Vertex{810.0f, 900.0f, 0.0f},
		Vertex{810.0f, 800.0f, 0.0f},
		Vertex{0.0f, 900.0f, 0.0f},
		Vertex{0.0f, 800.0f, 0.0f},
	};
	uint32_t numVerticesFracBounds = sizeof(fractalBounds) / sizeof(Vertex);

	VertexBuffer fractalBoundsBuffer(fractalBounds, numVerticesFracBounds);

	Shader shader("shader.vert", "shader.frac");
	shader.bind();

	shader.setColorUniform(1.0f, 0.0f, 1.0f, 1.0f);

	glm::mat4 model = glm::mat4(1.0f);
	//glm::vec3 transform = glm::vec3(5.0f, 0.0f, 0.0f);
	//model = glm::translate(model, transform);

	glm::mat4 projection = glm::ortho(0.0f, 1600.0f, 0.0f, 900.0f, -10.0f, 100.0f);

	glm::mat4 modelProj = projection * model;

	int modelProjMatrixLocation = glGetUniformLocation(shader.getShaderId(), "u_modelProj");


	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);		//Wireframe

	SierpinskiTriangle s_triangle(8, 1.0f, 0.0f, 0.0f, 1.0f);
	Vertex* triangle = s_triangle.getVertices();
	VertexBuffer s_triangleBuffer(triangle, 3);

	Vertex* lineVer = s_triangle.getLineVer();
	VertexBuffer lineVerBuffer(lineVer, 2);
	Vertex* lineHor = s_triangle.getLineHor();
	VertexBuffer lineHorBuffer(lineHor, 2);

	/*
	if (!window.getClose()){
		window.clear(0.4f, 0.4f, 0.4f, 1.0f);
		s_triangle_Buffer.bind();
		s_triangle.drawIterations(window);
		s_triangle_Buffer.unbind();
	}*/

	while (!window.getClose()) {
		
		window.update();

		window.clear(0.0f, 0.0f, 0.0f, 1.0f);

		float width = (float) window.getWidth();
		float height = (float) window.getHeight();
		if ((window.getZoom() || window.getLeftMouseButtonPressed() || window.getRightMouseButtonPressed()) && window.getMouseX() < width/2.0f && window.getMouseX() > width / 160.0f && window.getMouseY() > height / 9.0f && window.getMouseY() < height - height/9.0f) {
			s_triangle.handleZoom(2.0f,window.getMouseX(), window.getMouseY(), window.getWheelY(), window.getLeftMouseButtonPressed(), window.getRightMouseButtonPressed());
		}
		s_triangleBuffer.bind();
		s_triangle.draw(window.getWidth(), window.getHeight(), window.getZoom());
		s_triangleBuffer.unbind();

		shader.bind();
		shader.setColorUniform(0.1f, 0.1f, 0.1f, 1.0f);
		fractalBoundsBuffer.bind();
		glUniformMatrix4fv(modelProjMatrixLocation, 1, GL_FALSE, &modelProj[0][0]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, numVerticesFracBounds);
		fractalBoundsBuffer.unbind();

		int boxSizeFactor = 1;

		SDL_Surface* screen = window.getScreen();
		s_triangle.contentBoxes(boxSizeFactor, screen, window.getWidth(), window.getHeight());

		lineVerBuffer.bind();
		s_triangle.drawVerLines(boxSizeFactor, 1.0f, 1.0f, 1.0f, 1.0f);
		lineVerBuffer.unbind();
		lineHorBuffer.bind();
		s_triangle.drawHorLines(boxSizeFactor, 1.0f, 1.0f, 1.0f, 1.0f);
		lineHorBuffer.unbind();

		shader.bind();
		shader.setColorUniform(1.0f, 1.0f, 1.0f, 1.0f);
		borderBuffer.bind();
		glUniformMatrix4fv(modelProjMatrixLocation, 1, GL_FALSE, &modelProj[0][0]);
		glDrawArrays(GL_LINE_LOOP, 0, numVerticesBorder);
		borderBuffer.unbind();
	}
	return 0;
}
