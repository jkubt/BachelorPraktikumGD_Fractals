#pragma once
#include "glm/glm.hpp"

#include "defines.h"
#include "shader.h"
#include "vertexBuffer.h"


class SierpinskiCarpet {
public:
	SierpinskiCarpet(int depth, float r, float g, float b, float a);
	virtual ~SierpinskiCarpet() {};

	void draw(int windowWidth, int windowHeight, bool zoom, float r, float g, float b);
	void handleZoom(float speedMultiplier, int mouseX, int mouseY, Sint32 wheelY, bool leftButton, bool rightButton);
	void reset(bool resetZoom, int depth);
	float getSize() { return size; };

	void setDepth(int depth);

private:
	void drawRec(int depth, glm::vec3 transform);
	void setColorFractal(float r, float g, float b);

	int depth;
	float size;
	int redValue255;
	int greenValue255;
	int blueValue255;
	int alphaValue255;
	float zoomLevel;
	float zoomLevel_lastDetail;
	float zoomLevel_out;
	int countReplaced;
	bool last;
	int mouseX;
	int mouseY;
	int wheelY;
	Vertex square[4];
	Shader shader = Shader("shader.vert", "shader.frac");
	glm::mat4 model;
	glm::mat4 projection;
};

