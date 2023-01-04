#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>

class Window {
public:
	Window(int width, int height, const char *title);
	virtual ~Window();

	void clear(float r, float g, float b, float a);
	void update();
	bool getClose();

protected:
private:
	SDL_Window* window;
	SDL_GLContext glContext;
	bool close;
};

