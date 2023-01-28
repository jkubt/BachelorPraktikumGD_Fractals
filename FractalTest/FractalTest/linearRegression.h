#pragma once
#include "defines.h"

class LinearRegression
{
public:
    LinearRegression();
    virtual ~LinearRegression() {};

    float* calculateLinearRegression(float datapoints[18]);

    float getSlope() { return slope; };
    float getIntercept() { return intercept; };

private:
    float xValues[9];
    float yValues[9];
    float slope;
    float intercept;
    float sum_xy;
    float sum_x;
    float sum_y;
    float sum_xSquare;
    int numberOfValues;
};

