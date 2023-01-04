#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>

#include "window.h"


Window::Window(int width, int height, const char *title) {
	SDL_Init(SDL_INIT_EVERYTHING);

	// sizes for pixels and enable Double Buffering
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);
	glContext = SDL_GL_CreateContext(window);

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		std::cin.get();
	}

	close = false;
}

Window::~Window() {
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Window::clear(float r, float g, float b, float a) {
	glClearColor(r,g,b,a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Window::update() {
	SDL_GL_SwapWindow(window);

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			close = true;
		}
	}
}

bool Window::getClose() {
	return close;
}