#include <algorithm>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "glm/gtc/matrix_transform.hpp"

#include "fractalDimension.h"
#include "linearRegression.h"

FractalDimension::FractalDimension(float r, float g, float b, float a) {
	lineVer[0] = Vertex{ 20.0f, 112.25f, 0.0f };
	lineVer[1] = Vertex{ 20.0f, 787.75f, 0.0f };
	lineHor[0] = Vertex{ 20.0f, 112.25f, 0.0f };
	lineHor[1] = Vertex{ 800.0f, 112.25f, 0.0f };
	box[0] = Vertex{ 20.0f, 112.25f, 0.0f };
	box[1] = Vertex{ 800.0f, 112.25f, 0.0f };
	box[2] = Vertex{ 20.0f, 787.75f, 0.0f };
	box[3] = Vertex{ 800.0f, 787.75f, 0.0f };
	CoSy_x_y[0] = Vertex{ 1450.0f, 100.0f, 0.0f };
	CoSy_x_y[1] = Vertex{ 900.0f, 100.0f, 0.0f };
	CoSy_x_y[2] = Vertex{ 900.0f, 650.0f, 0.0f };
	datapoint[0] = Vertex{ 897.0f, 97.0f, 0.0f };
	datapoint[1] = Vertex{ 897.0f, 103.0f, 0.0f };
	datapoint[2] = Vertex{ 903.0f, 97.0f, 0.0f };
	datapoint[3] = Vertex{ 903.0f, 103.0f, 0.0f };
	for (int i = 0; i < sizeof(positionsDatapoints) / sizeof(*positionsDatapoints); i++) {
		positionsDatapoints[i] = -1.0f;
	}
	for (int i = 0; i < sizeof(datapointConnections) / sizeof(*datapointConnections); i++) {
		datapointConnections[i] = Vertex{ -1.0f, -1.0f, 0.0f };
	}
	redValue255 = int(r * 255);
	greenValue255 = int(g * 255);
	blueValue255 = int(b * 255);
	alphaValue255 = int(a * 255);
	boxSizeFactorCurrentIterationGraphDimension = 0;
	timeLastIterationGraphDimension = 0;
	writeToPositionsDatapoints = false;
	calculatedDimensionWithGraph = -1.0f;
	boxesTotal = 0;
	boxesWithContent = 0;
	model = glm::mat4(1.0f);
	projection = glm::ortho(0.0f, 1600.0f, 0.0f, 900.0f, -10.0f, 100.0f);
}

void FractalDimension::drawVerLines(int boxSizeFactor, float r, float g, float b, float a) {
	float width = 780.0f / boxSizeFactor;
	VertexBuffer lineVerBuffer(lineVer, 2);
	shader.bind();
	shader.setColorUniform(r, g, b, a);
	lineVerBuffer.bind();
	for (int i = 0; i <= boxSizeFactor; i++) {
		glm::vec3 transform = glm::vec3(i * width, 0.0f, 0.0f);
		glm::mat4 model_draw = glm::translate(glm::mat4(1.0f), transform);
		glm::mat4 modelProj = projection * model_draw;
		int modelProjMatrixLocation = glGetUniformLocation(shader.getShaderId(), "u_modelProj");
		glUniformMatrix4fv(modelProjMatrixLocation, 1, GL_FALSE, &modelProj[0][0]);
		glDrawArrays(GL_LINES, 0, 3);
	}
	lineVerBuffer.unbind();
	shader.setColorUniform(1.0f, 0.0f, 0.0f, 1.0f);
}

void FractalDimension::drawHorLines(int boxSizeFactor, float r, float g, float b, float a) {
	float height = 675.5f / boxSizeFactor;
	VertexBuffer lineHorBuffer(lineHor, 2);
	shader.bind();
	shader.setColorUniform(r, g, b, a);
	lineHorBuffer.bind();
	for (int i = 0; i <= boxSizeFactor; i++) {
		glm::vec3 transform = glm::vec3(0.0f, i * height, 0.0f);
		glm::mat4 model_draw = glm::translate(glm::mat4(1.0f), transform);
		glm::mat4 modelProj = projection * model_draw;
		int modelProjMatrixLocation = glGetUniformLocation(shader.getShaderId(), "u_modelProj");
		glUniformMatrix4fv(modelProjMatrixLocation, 1, GL_FALSE, &modelProj[0][0]);
		glDrawArrays(GL_LINES, 0, 3);
	}
	lineHorBuffer.unbind();
	shader.setColorUniform(1.0f, 0.0f, 0.0f, 1.0f);
}

int FractalDimension::contentBoxes(int boxSizeFactor, int windowWidth, int windowHeight, float alphaValue) {
	if (boxSizeFactor < 1) {
		return 0;
	}
	boxesWithContent = 0;
	boxesTotal = boxSizeFactor * boxSizeFactor;
	float width = (box[1].x - box[0].x) / boxSizeFactor;
	float height = (box[3].y - box[0].y) / boxSizeFactor;
	unsigned char* pixels = new unsigned char[windowWidth * windowHeight * 4];
	Uint8 red, green, blue, alpha;

	glReadPixels(0, 0, windowWidth, windowHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	float complementRed = float(std::max({ redValue255, greenValue255, blueValue255 }) + std::min({ redValue255, greenValue255, blueValue255 }) - redValue255);
	float complementGreen = float(std::max({ redValue255, greenValue255, blueValue255 }) + std::min({ redValue255, greenValue255, blueValue255 }) - greenValue255);
	float complementBlue = float(std::max({ redValue255, greenValue255, blueValue255 }) + std::min({ redValue255, greenValue255, blueValue255 }) - blueValue255);

	VertexBuffer boxBuffer(box, 4);

	shader.bind();
	shader.setColorUniform(complementRed, complementGreen, complementBlue, alphaValue);

	boxBuffer.bind();
	for (int i = 0; i < boxSizeFactor; i++) {
		for (int j = 0; j < boxSizeFactor; j++) {

			glm::vec4 clipSpaceVertex = projection * glm::vec4(20.0f + width * i, 112.25f + height * j, 0.0f, 1.0f);
			glm::vec2 ndc = glm::vec2(clipSpaceVertex.x, clipSpaceVertex.y);
			glm::vec2 windowSpaceCoordinates = ndc * 0.5f + 0.5f;
			int startX = int(windowSpaceCoordinates.x * windowWidth);
			int startY = int(windowSpaceCoordinates.y * windowHeight);

			clipSpaceVertex = projection * glm::vec4(20.0f + width * (i + 1), 112.25f + height * (j + 1), 0.0f, 1.0f);
			ndc = glm::vec2(clipSpaceVertex.x, clipSpaceVertex.y);
			windowSpaceCoordinates = ndc * 0.5f + 0.5f;
			int endX = int(windowSpaceCoordinates.x * windowWidth);
			int endY = int(windowSpaceCoordinates.y * windowHeight);

			bool exit = false;
			for (int x = startX; x < endX; x++) {
				for (int y = startY; y < endY; y++) {
					red = pixels[(y * windowWidth + x) * 4];
					green = pixels[(y * windowWidth + x) * 4 + 1];
					blue = pixels[(y * windowWidth + x) * 4 + 2];
					alpha = pixels[(y * windowWidth + x) * 4 + 3];
					if (red == redValue255 && green == greenValue255 && blue == blueValue255 && alpha == alphaValue255) {
						boxesWithContent++;
						drawBox(float(i) * width, float(j) * height, boxSizeFactor);
						exit = true;
						break;
					}
				}
				if (exit) {
					break;
				}
			}
		}
	}

	boxBuffer.unbind();
	shader.setColorUniform(float(redValue255) / 255.0f, float(greenValue255) / 255.0f, float(blueValue255) / 255.0f, float(alphaValue255) / 255.0f);

	return boxesWithContent;
}

void FractalDimension::drawBox(float xShift, float yShift, int boxSizeFactor) {

	float size = 1.0f / float(boxSizeFactor);
	float deltaX = box[0].x - (box[0].x * size);
	float deltaY = box[0].y - (box[0].y * size);
	glm::vec3 transform = glm::vec3(xShift, yShift, 0.0f);
	transform += glm::vec3(deltaX, deltaY, 0.0f);
	shader.bind();
	glm::mat4 model_draw = glm::translate(model, transform);
	model_draw = glm::scale(model_draw, glm::vec3(size));
	glm::mat4 modelProj = projection * model_draw;
	int modelProjMatrixLocation = glGetUniformLocation(shader.getShaderId(), "u_modelProj");
	glUniformMatrix4fv(modelProjMatrixLocation, 1, GL_FALSE, &modelProj[0][0]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void FractalDimension::drawCoordinateSystem() {
	VertexBuffer coSyBuffer(CoSy_x_y, 3);
	shader.bind();
	shader.setColorUniform(1.0f, 1.0f, 1.0f, 1.0f);
	coSyBuffer.bind();
	glm::mat4 modelProj = projection;
	int modelProjMatrixLocation = glGetUniformLocation(shader.getShaderId(), "u_modelProj");
	glUniformMatrix4fv(modelProjMatrixLocation, 1, GL_FALSE, &modelProj[0][0]);
	glDrawArrays(GL_LINE_STRIP, 0, 3);
	coSyBuffer.unbind();

	Vertex xLabeling[2] = {
		Vertex{0.0f, 0.0f, 0.0f},
		Vertex{0.0f, 13.0f, 0.0f}
	};
	VertexBuffer xLabelingBuffer(xLabeling, 2);
	shader.bind();
	shader.setColorUniform(1.0f, 1.0f, 1.0f, 1.0f);
	xLabelingBuffer.bind();
	for (int i = 1; i <= 10; i++) {
		float downwardsAmount = 6.0f;
		if (i % 2 == 1) {
			downwardsAmount /= 2.0f;
		}
		float deltaX = 900.0f + (float(i) / 2.0f) * 100.0f;
		float deltaY = 100.0f - downwardsAmount;
		glm::vec3 transform = glm::vec3(deltaX, deltaY, 0.0f);
		glm::mat4 model_draw = glm::translate(model, transform);
		if (i % 2 == 1) {
			model_draw = glm::scale(model_draw, glm::vec3(0.5f));
		}
		glm::mat4 modelProj = projection * model_draw;
		int modelProjMatrixLocation = glGetUniformLocation(shader.getShaderId(), "u_modelProj");
		glUniformMatrix4fv(modelProjMatrixLocation, 1, GL_FALSE, &modelProj[0][0]);
		glDrawArrays(GL_LINES, 0, 2);
	}
	xLabelingBuffer.unbind();

	Vertex yLabeling[2] = {
		Vertex{0.0f, 0.0f, 0.0f},
		Vertex{13.0f, 0.0f, 0.0f}
	};
	VertexBuffer yLabelingBuffer(yLabeling, 2);
	shader.bind();
	shader.setColorUniform(1.0f, 1.0f, 1.0f, 1.0f);
	yLabelingBuffer.bind();
	for (int i = 1; i <= 10; i++) {
		float sidewardsAmount = 7.0f;
		if (i % 2 == 1) {
			sidewardsAmount /= 2.0f;
		}
		float deltaX = 900.0f - sidewardsAmount;
		float deltaY = 100.0f + (float(i) / 2.0f) * 100.0f;
		glm::vec3 transform = glm::vec3(deltaX, deltaY, 0.0f);
		glm::mat4 model_draw = glm::translate(model, transform);
		if (i % 2 == 1) {
			model_draw = glm::scale(model_draw, glm::vec3(0.5f));
		}
		glm::mat4 modelProj = projection * model_draw;
		int modelProjMatrixLocation = glGetUniformLocation(shader.getShaderId(), "u_modelProj");
		glUniformMatrix4fv(modelProjMatrixLocation, 1, GL_FALSE, &modelProj[0][0]);
		glDrawArrays(GL_LINES, 0, 2);
	}
	yLabelingBuffer.unbind();
}

int FractalDimension::calculateDimensionGraph() {
	Uint32 time = SDL_GetTicks();
	if (writeToPositionsDatapoints) {
		float logN = log10(boxesWithContent);
		float logS = log10(boxSizeFactorCurrentIterationGraphDimension);
		int yIndex = log2(boxSizeFactorCurrentIterationGraphDimension) * 2 - 1;
		positionsDatapoints[yIndex - 1] = logS;
		positionsDatapoints[yIndex] = logN;
		float coSyOriginX = CoSy_x_y[1].x;
		float coSyOriginY = CoSy_x_y[1].y;
		datapointConnections[(yIndex - 1) / 2] = Vertex{ coSyOriginX + (logS * 100.0f), coSyOriginY + (logN * 100.0f), 0.0f };
		writeToPositionsDatapoints = false;
	}
	if (boxSizeFactorCurrentIterationGraphDimension < 512 && boxSizeFactorCurrentIterationGraphDimension > 0) {
		if (time > timeLastIterationGraphDimension + 1500) {
			boxSizeFactorCurrentIterationGraphDimension = boxSizeFactorCurrentIterationGraphDimension * 2;
			timeLastIterationGraphDimension = time;
			writeToPositionsDatapoints = true;
		}
	}
	shader.bind();
	int condition = (log2(boxSizeFactorCurrentIterationGraphDimension) - 1) * 2;
	if (writeToPositionsDatapoints) {
		condition = (log2(boxSizeFactorCurrentIterationGraphDimension / 2) - 1) * 2;
	}
	VertexBuffer datapointBuffer(datapoint, 4);
	for (int i = 0; i <= condition; i += 2) {
		if (i + 2 <= condition) {
			shader.setColorUniform(0.0f, 0.6f, 0.9f, 1.0f);
			Vertex tempConnection[2] = {
				datapointConnections[i / 2],
				datapointConnections[(i / 2) + 1]
			};
			VertexBuffer dataPointConnectionBuffer(tempConnection, 2);
			dataPointConnectionBuffer.bind();
			glm::mat4 modelProj = projection * model;
			int modelProjMatrixLocation = glGetUniformLocation(shader.getShaderId(), "u_modelProj");
			glUniformMatrix4fv(modelProjMatrixLocation, 1, GL_FALSE, &modelProj[0][0]);
			glDrawArrays(GL_LINES, 0, 2);
			dataPointConnectionBuffer.unbind();
		}
		shader.setColorUniform(0.9f, 0.6f, 0.0f, 1.0f);
		datapointBuffer.bind();
		float deltaX = positionsDatapoints[i] * 100.0f;
		float deltaY = positionsDatapoints[i + 1] * 100.0f;
		glm::vec3 transform = glm::vec3(deltaX, deltaY, 0.0f);
		glm::mat4 model_draw = glm::translate(model, transform);
		glm::mat4 modelProj = projection * model_draw;
		int modelProjMatrixLocation = glGetUniformLocation(shader.getShaderId(), "u_modelProj");
		glUniformMatrix4fv(modelProjMatrixLocation, 1, GL_FALSE, &modelProj[0][0]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		datapointBuffer.unbind();
	}
	shader.setColorUniform(1.0f, 0.0f, 0.0f, 1.0f);

	return std::max(boxSizeFactorCurrentIterationGraphDimension, 1);
}

std::string FractalDimension::drawPredictedFunction() {
	LinearRegression linearRegresion = LinearRegression();
	float* predictedGraph = linearRegresion.calculateLinearRegression(positionsDatapoints);
	Vertex predictedFunction[2] = {
		Vertex{*predictedGraph, *(predictedGraph + 1), 0.0f},
		Vertex{*(predictedGraph + 2), *(predictedGraph + 3), 0.0f}
	};
	VertexBuffer predictedFunctionBuffer(predictedFunction, 2);
	shader.bind();
	shader.setColorUniform(1.0f, 0.0f, 0.0f, 1.0f);
	predictedFunctionBuffer.bind();
	float deltaX = CoSy_x_y[1].x;
	float deltaY = CoSy_x_y[1].y;
	glm::vec3 transform = glm::vec3(deltaX, deltaY, 0.0f);
	glm::mat4 model_draw = glm::translate(model, transform);
	glm::mat4 modelProj = projection * model_draw;
	int modelProjMatrixLocation = glGetUniformLocation(shader.getShaderId(), "u_modelProj");
	glUniformMatrix4fv(modelProjMatrixLocation, 1, GL_FALSE, &modelProj[0][0]);
	glDrawArrays(GL_LINES, 0, 2);
	predictedFunctionBuffer.unbind();
	float slope = linearRegresion.getSlope();
	float intercept = linearRegresion.getIntercept();
	std::string functionString = "Predicted Function: " + std::to_string(slope) + "x + " + std::to_string(intercept);
	calculatedDimensionWithGraph = slope;
	return functionString;
}

void FractalDimension::resetPositionsDatapoints() {
	for (int i = 0; i < sizeof(positionsDatapoints) / sizeof(*positionsDatapoints); i++) {
		positionsDatapoints[i] = -1.0f;
	}
}

void FractalDimension::setColorOfFractal(float r, float g, float b) {
	redValue255 = int(r * 255.0f);
	greenValue255 = int(g * 255.0f);
	blueValue255 = int(b * 255.0f);
}