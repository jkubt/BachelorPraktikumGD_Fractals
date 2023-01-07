#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "glm/gtc/matrix_transform.hpp"

#include "window.h"
#include "sierpinskiTriangle.h"


SierpinskiTriangle::SierpinskiTriangle(int depth, float r, float g, float b, float a) {
	triangle[0] = Vertex{ 20.0f, 112.25f, 0.0f};
	triangle[1] = Vertex{ 800.0f, 112.25f, 0.0f};
	triangle[2] = Vertex{ 410.0f, 787.75f, 0.0f};
	lineVer[0] = Vertex{10.0f, 100.0f, 0.0f};
	lineVer[1] = Vertex{10.0f, 800.0f, 0.0f};
	lineHor[0] = Vertex{10.0f, 100.0f, 0.0f};
	lineHor[1] = Vertex{810.f, 100.0f, 0.0f};
	this->depth = depth;
	size = 1.0f / ((float) pow(2, depth));
	zoomLevel = 1.0f;
	zoomLevel_lastDetail = 1.0f;
	zoomLevel_out = (float) pow(1.08006, 9 * 2);
	countReplaced = 0;
	last = false;
	mouseX = 0;
	mouseY = 0;
	wheelY = 0;
	boxesWithContent = 0;
	boxesWithoutContent = 0;
	shader.bind();
	shader.setColorUniform(r, g, b, a);
	model = glm::mat4(1.0f);
	projection = glm::ortho(0.0f, 1600.0f, 0.0f, 900.0f, -10.0f, 100.0f);
}

void SierpinskiTriangle::drawIterations(Window window) {
	for (int i = 0; i <= depth; i++) {
		window.clear(0.4f, 0.4f, 0.4f, 1.0f);
		std::this_thread::sleep_for(std::chrono::seconds(1));
		size = 1.0f / ((float)pow(2, i));
		drawRec(i, glm::vec3(0.0f, 0.0f, 0.0f));
	}
}

void SierpinskiTriangle::draw(int windowWidth, int windowHeight, bool zoom) {
	if (zoomLevel > zoomLevel_lastDetail*2.0f) {
		if (zoomLevel > 4.0f) {
			depth--;
			size = 1.0f / ((float)pow(2, depth));
			zoomLevel = zoomLevel_lastDetail;
			countReplaced++;
		}
		depth++;
		size = 1.0f / ((float)pow(2, depth));
		zoomLevel_lastDetail = zoomLevel;
		if (countReplaced == 0) {
			last = true;
		}
	}
	if (zoomLevel < 2.0f && countReplaced > 0) {
		depth++;
		size = 1.0f / ((float)pow(2, depth));
		zoomLevel *= 2;
		countReplaced--;
		depth--;
		size = 1.0f / ((float)pow(2, depth));
	}
	if (last && zoomLevel < 2.0f) {
		depth--;
		size = 1.0f / ((float)pow(2, depth));
		zoomLevel_lastDetail = 1.0f;
		last = false;
	}
	drawRec(depth, glm::vec3(0.0f, 0.0f, 0.0f));
}

void SierpinskiTriangle::drawRec(int depth, glm::vec3 transform) {
	if (depth == 0) {
		float deltaX = triangle[0].x - (triangle[0].x * size*zoomLevel);
		float deltaY = triangle[0].y - (triangle[0].y * size*zoomLevel);
		transform += glm::vec3(deltaX, deltaY, 0.0f);
		shader.bind();
		glm::mat4 model_draw = glm::translate(model, transform);
		model_draw = glm::scale(model_draw, glm::vec3(size*zoomLevel));;
		glm::mat4 modelProj = projection * model_draw;
		int modelProjMatrixLocation = glGetUniformLocation(shader.getShaderId(), "u_modelProj");
		glUniformMatrix4fv(modelProjMatrixLocation, 1, GL_FALSE, &modelProj[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		return;
	}
	float deltaX = 0.0f;
	float deltaY = 0.0f;
	glm::vec3 transform_temp = glm::vec3(deltaX, deltaY, 0.0f);
	glm::vec3 transform_new = transform + transform_temp;
	drawRec(depth - 1, transform_new);

	deltaX = (780.0f * zoomLevel) / ((float)pow(2, depth));
	deltaY = 0.0f;
	transform_temp = glm::vec3(deltaX, deltaY, 0.0f);
	transform_new = transform + transform_temp;
	drawRec(depth - 1, transform_new);

	deltaX = (390.0f * zoomLevel) / ((float)pow(2, depth));
	deltaY = (675.5f*zoomLevel) / ((float) pow(2, depth));
	transform_temp = glm::vec3(deltaX, deltaY, 0.0f);
	transform_new = transform + transform_temp;
	drawRec(depth - 1, transform_new);
}

void SierpinskiTriangle::handleZoom(float speedMultiplier, int mouseX, int mouseY, Sint32 wheelY, bool leftButton, bool rightButton) {
	float mult = 0.04003f * speedMultiplier;
	if (countReplaced == 0 && zoomLevel < 2.0f) {
		mult /= 2;
	}
	this->mouseX = mouseX;
	this->mouseY = mouseY;
	this->wheelY = wheelY;
	if (zoomLevel > 1.0f || wheelY > 0) {
		if (wheelY > 0) {
			zoomLevel *= (1+mult);
		}
		else if (wheelY < 0) {
			zoomLevel /= (1+mult);
		}
	}
	if (zoomLevel > 1.0f || leftButton) {
		if (leftButton) {
			zoomLevel *= (1+mult);
		}
		if (rightButton) {
			zoomLevel /= (1+mult);
		}
	}
}

void SierpinskiTriangle::drawVerLines(int boxSizeFactor, float r, float g, float b, float a) {
	float width = 800.0f / boxSizeFactor;
	shader.bind();
	shader.setColorUniform(r, g, b, a);
	for (int i = 1; i < boxSizeFactor ; i++) {
		glm::vec3 transform = glm::vec3(i*width, 0.0f, 0.0f);
		glm::mat4 model_draw = glm::translate(glm::mat4(1.0f), transform);
		glm::mat4 modelProj = projection * model_draw;
		int modelProjMatrixLocation = glGetUniformLocation(shader.getShaderId(), "u_modelProj");
		glUniformMatrix4fv(modelProjMatrixLocation, 1, GL_FALSE, &modelProj[0][0]);
		glDrawArrays(GL_LINES, 0, 3);
	}
	shader.setColorUniform(1.0f, 0.0f, 0.0f, 1.0f);
}

void SierpinskiTriangle::drawHorLines(int boxSizeFactor, float r, float g, float b, float a) {
	float height = 700.0f / boxSizeFactor;
	shader.bind();
	shader.setColorUniform(r, g, b, a);
	for (int i = 1; i < boxSizeFactor; i++) {
		glm::vec3 transform = glm::vec3(0.0f, i*height, 0.0f);
		glm::mat4 model_draw = glm::translate(glm::mat4(1.0f), transform);
		glm::mat4 modelProj = projection * model_draw;
		int modelProjMatrixLocation = glGetUniformLocation(shader.getShaderId(), "u_modelProj");
		glUniformMatrix4fv(modelProjMatrixLocation, 1, GL_FALSE, &modelProj[0][0]);
		glDrawArrays(GL_LINES, 0, 3);
	}
	shader.setColorUniform(1.0f, 0.0f, 0.0f, 1.0f);
}

void SierpinskiTriangle::contentBoxes(int boxSizeFactor, SDL_Surface* screen, int windowWidth, int windowHeight) {
	if (boxSizeFactor < 1) {
		std::cout << "Error: boxes with content can not be calculated with boxSizeFactor < 1" << std::endl;
		return;
	}
	float width = 800.0f / boxSizeFactor;
	float height = 700.0f / boxSizeFactor;
	Uint32 pixel;
	Uint8 r, g, b, a;

	glm::vec4 clipSpaceVertex = projection * glm::vec4(10.0f, 100.0f, 0.0f, 1.0f);
	glm::vec2 ndc = glm::vec2(clipSpaceVertex.x / clipSpaceVertex.w, clipSpaceVertex.y / clipSpaceVertex.w);
	glm::vec2 windowSpaceCoordinates = ndc * 0.5f + 0.5f;
	int x = int(windowSpaceCoordinates.x * windowWidth);
	int y = int(windowSpaceCoordinates.y * windowHeight);

	pixel = get_pixel(screen, x, y);
	SDL_GetRGBA(pixel, screen->format, &r, &g, &b, &a);

	std::cout << unsigned(r) << " - " << unsigned(g) << " - " << unsigned(b) << " - " << unsigned(a) << " - " << std::endl;
}

Uint32 SierpinskiTriangle::get_pixel(SDL_Surface* screen, int x, int y) {
	int bpp = screen->format->BytesPerPixel;

	Uint8* pixelAdress = (Uint8*)screen->pixels + y * screen->pitch + x * bpp;

	switch (bpp) {
	case 1:
		return *pixelAdress;
		break;

	case 2:
		return *(Uint16*)pixelAdress;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return pixelAdress[0] << 16 | pixelAdress[1] << 8 | pixelAdress[2];
		else
			return pixelAdress[0] | pixelAdress[1] << 8 | pixelAdress[2] << 16;
		break;

	case 4:
		return *(Uint32*)pixelAdress;
		break;

	default:
		return 0;
	}
}