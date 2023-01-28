#pragma once
#include <string>
#include "glm/glm.hpp"

#include "defines.h"
#include "shader.h"
#include "vertexBuffer.h"

class FractalDimension
{
public:
	FractalDimension(float r, float g, float b, float a);
	virtual ~FractalDimension() {};

	void drawVerLines(int boxSizeFactor, float r, float g, float b, float a);
	void drawHorLines(int boxSizeFactor, float r, float g, float b, float a);
	int contentBoxes(int boxSizeFactor, int windowWidth, int windowHeight, float alphaValue);
	void drawCoordinateSystem();
	int calculateDimensionGraph();
	std::string drawPredictedFunction();
	 
	float getCalculatedDimensionWithGraph() { return calculatedDimensionWithGraph; };
	Uint32 getTimeLastIterationGraphDimension() { return timeLastIterationGraphDimension; };

	void setBoxSizeFactorCurrentIterationGraphDimension(int boxSizeFactor) { boxSizeFactorCurrentIterationGraphDimension = boxSizeFactor; };
	void setTimeLastIterateionGraphDimension(Uint32 time) { timeLastIterationGraphDimension = time; };
	void setColorOfFractal(float r, float g, float b);

	void resetPositionsDatapoints();

private:
	void drawBox(float xShift, float yShift, int boxSizeFactor);

	int redValue255;
	int greenValue255;
	int blueValue255;
	int alphaValue255;
	int boxesTotal;
	int boxesWithContent;
	int boxSizeFactorCurrentIterationGraphDimension;
	bool writeToPositionsDatapoints;
	float calculatedDimensionWithGraph;
	float positionsDatapoints[18];
	Uint32 timeLastIterationGraphDimension;
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

