#include "glm/gtc/matrix_transform.hpp"

#include "window.h"
#include "sierpinskiTriangle.h"
#include "linearRegression.h"


SierpinskiTriangle::SierpinskiTriangle(int depth, float r, float g, float b, float a) {
	triangle[0] = Vertex{ 20.0f, 112.25f, 0.0f};
	triangle[1] = Vertex{ 800.0f, 112.25f, 0.0f};
	triangle[2] = Vertex{ 410.0f, 787.75f, 0.0f};
	this->depth = depth;
	size = 1.0f / ((float) pow(2, depth));
	redValue255 = int(r * 255);
	greenValue255 = int(g * 255);
	blueValue255 = int(b * 255);
	alphaValue255 = int(a * 255);
	zoomLevel = 1.0f;
	zoomLevel_lastDetail = 1.0f;
	zoomLevel_out = (float) pow(1.08006, 9 * 2);
	countReplaced = 0;
	last = false;
	mouseX = 0;
	mouseY = 0;
	wheelY = 0;
	shader.bind();
	shader.setColorUniform(r, g, b, a);
	model = glm::mat4(1.0f);
	projection = glm::ortho(0.0f, 1600.0f, 0.0f, 900.0f, -10.0f, 100.0f);
}

void SierpinskiTriangle::draw(int windowWidth, int windowHeight, bool zoom, float r, float g, float b) {
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
	if (zoomLevel < 1.0f) {
		zoomLevel = 1.0f;
	}
	VertexBuffer sierpinskiTriangleBuffer(triangle, 3);
	setColorFractal(r, g, b);
	shader.bind();
	shader.setColorUniform(r, g, b, 1.0f);
	sierpinskiTriangleBuffer.bind();
	drawRec(depth, glm::vec3(0.0f, 0.0f, 0.0f));
	sierpinskiTriangleBuffer.unbind();
	shader.unbind();
}

void SierpinskiTriangle::drawRec(int depth, glm::vec3 transform) {
	if (depth == 0) {
		float deltaX = triangle[0].x - (triangle[0].x * size*zoomLevel);
		float deltaY = triangle[0].y - (triangle[0].y * size*zoomLevel);
		transform += glm::vec3(deltaX, deltaY, 0.0f);
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
		if (countReplaced == 0 && zoomLevel < 2.0f) {
			mult /= 2;
		}
		if (leftButton) {
			zoomLevel *= (1+mult);
		}
		if (rightButton) {
			zoomLevel /= (1+mult);
		}
	}
}

void SierpinskiTriangle::setDepth(int depth) { 
	this->depth = depth; 
	size = 1.0f / ((float)pow(2, depth));
}

void SierpinskiTriangle::reset(bool resetZoom, int depth) {
	if (resetZoom) {
		this->depth = depth;
		size = 1.0f / ((float)pow(2, depth));
		zoomLevel = 1.0f;
		zoomLevel_lastDetail = 1.0f;
		countReplaced = 0;
		last = false;
	}
}

void SierpinskiTriangle::setColorFractal(float r, float g, float b) {
	redValue255 = int(r * 255.0f);
	greenValue255 = int(g * 255.0f);
	blueValue255 = int(b * 255.0f);
}
