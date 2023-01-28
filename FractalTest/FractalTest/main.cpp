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

	//Window window(1024, 576, "FractalGenerator");
	Window window(2304, 1296, "FractalGenerator");

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

	glm::mat4 projection = glm::ortho(0.0f, 1600.0f, 0.0f, 900.0f, -10.0f, 100.0f);

	glm::mat4 modelProj = projection * model;

	int modelProjMatrixLocation = glGetUniformLocation(shader.getShaderId(), "u_modelProj");


	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);		//Wireframe

	SierpinskiTriangle s_triangle(8, 1.0f, 0.0f, 0.0f, 1.0f);

	int depth = 8;
	bool resetFractalZoom = false;
	float zoomSpeed = 2.0f;
	bool showDimensionBoxes = false;
	bool fillDimensionBoxes = false;
	int boxSizeFactor = 10;
	float colorFractal[3] = { 1.0f, 0.0f, 0.0f};
	float boxFillerOpacity = 0.5f;
	float percentageBoxFillerOpacity = 50.0f;
	int amountFilledBoxes = 0;
	bool graphCalculationActive = false;
	bool clearGraph = true;
	std::string predictedFunction = "";

	while (!window.getClose()) {

		window.updateBegin();

		window.clear(0.0f, 0.0f, 0.0f, 1.0f);

		float width = (float) window.getWidth();
		float height = (float) window.getHeight();
		if ((window.getZoom() || window.getLeftMouseButtonPressed() || window.getRightMouseButtonPressed()) && window.getMouseX() < width/2.0f && window.getMouseX() > width / 160.0f && window.getMouseY() > height / 9.0f && window.getMouseY() < height - height/9.0f) {
			s_triangle.handleZoom(zoomSpeed,window.getMouseX(), window.getMouseY(), window.getWheelY(), window.getLeftMouseButtonPressed(), window.getRightMouseButtonPressed());
		}

		s_triangle.reset(resetFractalZoom, depth);

		s_triangle.draw(window.getWidth(), window.getHeight(), window.getZoom(), colorFractal[0], colorFractal[1], colorFractal[2]);

		shader.bind();
		shader.setColorUniform(0.1f, 0.1f, 0.1f, 1.0f);
		fractalBoundsBuffer.bind();
		glUniformMatrix4fv(modelProjMatrixLocation, 1, GL_FALSE, &modelProj[0][0]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, numVerticesFracBounds);
		fractalBoundsBuffer.unbind();

		if (fillDimensionBoxes) {
			amountFilledBoxes = s_triangle.contentBoxes(boxSizeFactor, window.getWidth(), window.getHeight(), boxFillerOpacity);
		}

		if (showDimensionBoxes) {
			s_triangle.drawVerLines(boxSizeFactor, 1.0f, 1.0f, 1.0f, 1.0f);
			s_triangle.drawHorLines(boxSizeFactor, 1.0f, 1.0f, 1.0f, 1.0f);
		}

		shader.bind();
		shader.setColorUniform(1.0f, 1.0f, 1.0f, 1.0f);
		borderBuffer.bind();
		glUniformMatrix4fv(modelProjMatrixLocation, 1, GL_FALSE, &modelProj[0][0]);
		glDrawArrays(GL_LINE_LOOP, 0, numVerticesBorder);
		borderBuffer.unbind();

		if (!clearGraph) {
			int boxSizeFactorTemp = s_triangle.calculateDimensionGraph();
			if (graphCalculationActive) {
				boxSizeFactor = boxSizeFactorTemp;
			}
			if (boxSizeFactorTemp >= 512 && SDL_GetTicks() > s_triangle.getTimeLastIterationGraphDimension() + 1500 && graphCalculationActive) {
				graphCalculationActive = false;
				boxSizeFactor = 10;
				showDimensionBoxes = false;
			}
		}

		if (!graphCalculationActive && !clearGraph) {
			predictedFunction = s_triangle.drawPredictedFunction();
		}

		s_triangle.drawCoordinateSystem();

		ImGui::Begin("Box size window", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGuiInputTextFlags readOnly = NULL;
		if (graphCalculationActive) {
			readOnly = ImGuiInputTextFlags_ReadOnly;
		}
		ImGui::InputInt("Box size factor", &boxSizeFactor, 1, 10, readOnly);
		if (boxSizeFactor < 0) {
			boxSizeFactor = 0;
		}
		else if (boxSizeFactor > 512) {
			boxSizeFactor = 512;
		}
		ImGui::End();

		ImGui::Begin("Box appearance window", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::Checkbox("Show Boxes", &showDimensionBoxes);
		if (graphCalculationActive) {
			showDimensionBoxes = true;
		}
		if (showDimensionBoxes) {
			ImGui::Checkbox("Fill Boxes", &fillDimensionBoxes);
			if (graphCalculationActive) {
				fillDimensionBoxes = true;
			}
			resetFractalZoom = true;
			if (fillDimensionBoxes) {
				ImGui::InputFloat("Fill Color Opacity", &percentageBoxFillerOpacity, 1.0f, 10.0f, "%.1f%%");
				if (percentageBoxFillerOpacity > 100.0f) {
					percentageBoxFillerOpacity = 100.0f;
				}
				else if (percentageBoxFillerOpacity < 0.0f) {
					percentageBoxFillerOpacity = 0.0f;
				}
				boxFillerOpacity = percentageBoxFillerOpacity / 100.0f;
				if (boxSizeFactor > 0) {
					float fracDimension = log(amountFilledBoxes) / log(boxSizeFactor);
					ImGui::Text("Current Box Counting Dimension: \n log(%i)/log(%i) = %f", amountFilledBoxes, boxSizeFactor, fracDimension);
				}
			}
		}
		else {
			fillDimensionBoxes = false;
			resetFractalZoom = false;
		}
		ImGui::End();

		ImGui::Begin("Depth window", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		if (ImGui::SliderInt("Depth", &depth, 0, 10)) {
			s_triangle.setDepth(depth);
			resetFractalZoom = true;
			clearGraph = true;
			graphCalculationActive = false;
		}
		ImGui::End();

		ImGui::Begin("Reset zoom window", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		if (ImGui::Button("Reset Zoom", ImVec2(200,40))) {
			resetFractalZoom = true;
		}
		ImGui::End();

		ImGui::Begin("Zoom speed window", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::InputFloat("Zoom Speed", &zoomSpeed, 0.2f, 2.0f);
		if (zoomSpeed < 0.0f) {
			zoomSpeed = 0.0f;
		}
		ImGui::End();

		ImGui::Begin("Select fractal color window", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::ColorEdit3("Fractal Color", colorFractal);
		ImGui::End();

		ImGui::Begin("X-axis window", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::Text("log10(s)");
		ImGui::Text("s: sizefactor of the boxes");
		ImGui::End();

		ImGui::Begin("Y-axis window", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::Text("N: number of filled boxes");
		ImGui::Text("log10(N)");
		ImGui::End();
		for (int i = 1; i <= 10; i++) {
			std::string label = std::to_string(float(i) / 2.0f);
			std::string labelRounded = label.substr(0, label.find(".") + 2);
			ImGui::Begin("Lableing window x-axis" + i, NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			ImGui::Text(labelRounded.c_str());
			ImGui::End();
			ImGui::Begin("Lableing window y-axis" + i, NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			ImGui::Text(labelRounded.c_str());
			ImGui::End();
		}

		ImGui::Begin("Execute graph dimension window", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		if (ImGui::Button("Calculate Dimension with Graph", ImVec2(250, 30))) {
			s_triangle.setBoxSizeFactorCurrentIterationGraphDimension(1);
			Uint32 time = SDL_GetTicks();
			s_triangle.setTimeLastIterateionGraphDimension(time);
			s_triangle.resetPositionsDatapoints();
			graphCalculationActive = true;
			showDimensionBoxes = true;
			fillDimensionBoxes = true;
			clearGraph = false;
		}
		ImGui::End();

		ImGui::Begin("Predicted function window", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse /* | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove*/);
		if (!graphCalculationActive && !clearGraph) {
			ImGui::Text(predictedFunction.c_str());
			ImGui::Text("\n=> Calculated Fractal Dimension: %f", s_triangle.getCalculatedDimensionWithGraph());
		}
		ImGui::End();

		//Select Fractal ... clearGraph = true;

		window.updateEnd();
	}
	
	window.destroy();

	return 0;
}