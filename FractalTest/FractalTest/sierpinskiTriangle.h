#pragma once
#include "glm/glm.hpp"

#include "defines.h"
#include "shader.h"
#include "vertexBuffer.h"


class SierpinskiTriangle{
public:
	SierpinskiTriangle(int depth, float r, float g, float b, float a);
	virtual ~SierpinskiTriangle() {};
		
	void drawIterations(Window window);
	void draw(int windowWidth, int windowHeight, bool zoom);
	void drawVerLines(int boxSizeFactor, float r, float g, float b, float a);
	void drawHorLines(int boxSizeFactor, float r, float g, float b, float a);
	void contentBoxes(int boxSizeFactor, SDL_Surface* screen, int windowWidth, int windowHeight);
	void handleZoom(float speedMultiplier, int mouseX, int mouseY, Sint32 wheelY, bool leftButton, bool rightButton);
	
	Uint32 get_pixel(SDL_Surface* screen, int x, int y);

	Vertex* getVertices() { return triangle; };
	Vertex* getLineVer() { return lineVer; };
	Vertex* getLineHor() { return lineHor; };
	float getSize() { return size; };

private:
	void drawRec(int depth, glm::vec3 transform);

	int depth;
	float size;
	float zoomLevel;
	float zoomLevel_lastDetail;
	float zoomLevel_out;
	int countReplaced;
	bool last;
	int mouseX;
	int mouseY;
	int wheelY;
	int boxesWithContent;
	int boxesWithoutContent;
	Vertex triangle[3];
	Vertex lineVer[2];
	Vertex lineHor[2];
	Shader shader = Shader("shader.vert", "shader.frac");
	glm::mat4 model;
	glm::mat4 projection;
};
