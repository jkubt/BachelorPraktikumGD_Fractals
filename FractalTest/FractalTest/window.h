#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

class Window {
public:
	Window(int width, int height, const char *title);
	virtual ~Window();

	void clear(float r, float g, float b, float a);
	void updateBegin();
	void updateEnd();
	void destroy();

	bool getClose() { return close; };
	SDL_Window* getWindow() { return window; };
	int getWidth() { return width; };
	int getHeight() { return height; };
	bool getZoom() { return zoom; };
	int getMouseX() { return mouseX; };
	int getMouseY() { return mouseY; };
	Sint32 getWheelY() { return wheelY; };
	bool getLeftMouseButtonPressed() { return leftMouseButtonPressed; };
	bool getRightMouseButtonPressed() { return rightMouseButtonPressed; };

protected:
private:
	SDL_Window* window;
	int width;
	int height;
	SDL_GLContext glContext;
	bool close;
	float aspectRatio;
	bool zoom;
	int mouseX;
	int mouseY;
	Sint32 wheelY;
	bool leftMouseButtonPressed;
	bool rightMouseButtonPressed;
};