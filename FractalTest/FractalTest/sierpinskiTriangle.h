#pragma once
#include <string>
#include "glm/glm.hpp"

#include "defines.h"
#include "shader.h"
#include "vertexBuffer.h"


class SierpinskiTriangle{
public:
	SierpinskiTriangle(int depth, float r, float g, float b, float a);
	virtual ~SierpinskiTriangle() {};

	void draw(int windowWidth, int windowHeight, bool zoom, float r, float g, float b);
	void drawVerLines(int boxSizeFactor, float r, float g, float b, float a);
	void drawHorLines(int boxSizeFactor, float r, float g, float b, float a);
	void drawCoordinateSystem();
	int contentBoxes(int boxSizeFactor, int windowWidth, int windowHeight, float alphaValue);
	int calculateDimensionGraph();
	std::string drawPredictedFunction();
	void handleZoom(float speedMultiplier, int mouseX, int mouseY, Sint32 wheelY, bool leftButton, bool rightButton);
	void reset(bool resetZoom, int depth);
	float getCalculatedDimensionWithGraph() { return calculatedDimensionWithGraph; };
	Uint32 getTimeLastIterationGraphDimension() { return timeLastIterationGraphDimension; };
	float getSize() { return size; };

	void setDepth(int depth);
	void setBoxSizeFactorCurrentIterationGraphDimension(int boxSizeFactor) { boxSizeFactorCurrentIterationGraphDimension = boxSizeFactor; };
	void setTimeLastIterateionGraphDimension(Uint32 time) { timeLastIterationGraphDimension = time; };

	void resetPositionsDatapoints();

private:
	void drawRec(int depth, glm::vec3 transform);
	void drawBox(float xShift, float yShift, int boxSizeFactor);
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
	int boxesTotal;
	int boxesWithContent;
	int boxSizeFactorCurrentIterationGraphDimension;
	bool writeToPositionsDatapoints;
	float calculatedDimensionWithGraph;
	float positionsDatapoints[18];
	Uint32 timeLastIterationGraphDimension;
	Vertex triangle[3];
	Vertex lineVer[2];
	Vertex lineHor[2];
	Vertex box[4];
	Vertex CoSy_x_y[3];
	Vertex datapoint[4];
	Vertex datapointConnections[9];
	Shader shader = Shader("shader.vert", "shader.frac");
	glm::mat4 model;
	glm::mat4 projection;
};
