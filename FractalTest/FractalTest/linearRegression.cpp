#include "linearRegression.h"

#include <iostream>

LinearRegression::LinearRegression() {
    slope = -1.0f;
    intercept = -1.0f;
    sum_y = 0.0f;
    sum_x = 0.0f;
    sum_xSquare = 0.0f;
    sum_xy = 0.0f;
    numberOfValues = 9;
    for (int i = 0; i < numberOfValues; i++) {
        xValues[i] = -1.0f;
    }
    for (int i = 0; i < numberOfValues; i++) {
        yValues[i] = -1.0f;
    }
}

float* LinearRegression::calculateLinearRegression(float datapoints[18]) {
    for (int i = 0; i < numberOfValues; i++) {
        xValues[i] = datapoints[i*2];
        yValues[i] = datapoints[(i*2) + 1];
    }

    for (int i = 0; i < numberOfValues; i++) {
        sum_x += xValues[i];
        sum_y += yValues[i];
        sum_xSquare += (xValues[i] * xValues[i]);
        sum_xy += (xValues[i] * yValues[i]);
    }

    slope = (numberOfValues * sum_xy - sum_x * sum_y) / (numberOfValues * sum_xSquare - sum_x * sum_x);
    intercept = (sum_y - slope * sum_x) / numberOfValues;
    float endpointpredictedGraphX = 550.0f;
    float endpointpredictedGraphY = slope * 550.0f + intercept*100.0f;
    if (endpointpredictedGraphY > 550.0f) {
        endpointpredictedGraphX = (550.0f - intercept*100.0f) / slope;
        endpointpredictedGraphY = 550.0f;
    }
    float startpointpredictedGraphX = 0.0f;
    float startpointpredictedGraphY = intercept * 100.0f;
    if (startpointpredictedGraphY < 0.0f) {
        startpointpredictedGraphX = ( - intercept * 100) / slope;
        startpointpredictedGraphY = 0.0f;
    }
    float predictedGraph[4] = { startpointpredictedGraphX, startpointpredictedGraphY, endpointpredictedGraphX, endpointpredictedGraphY};

    return predictedGraph;
}