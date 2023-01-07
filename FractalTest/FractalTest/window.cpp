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
	SDL_GL_SetSwapInterval(1);

#ifdef _DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif 

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_MAXIMIZED | SDL_WINDOW_OPENGL| SDL_WINDOW_RESIZABLE);
	glContext = SDL_GL_CreateContext(window);

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		std::cin.get();
	}

	this->width = width;
	this->height = height;
	close = false;
	aspectRatio = (float)width / (float)height;
	zoom = false;
	mouseX = 0;
	mouseY = 0;
	wheelY = 0;
	leftMouseButtonPressed = false;
	rightMouseButtonPressed = false;
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
	zoom = false;
	wheelY = 0;
	SDL_GL_SwapWindow(window);

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			close = true;
		}
		else if (event.type == SDL_MOUSEWHEEL && event.wheel.y) {
			wheelY = event.wheel.y;
			SDL_GetMouseState(&mouseX, &mouseY);
			zoom = true;
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (event.button.button == SDL_BUTTON_LEFT) {
				leftMouseButtonPressed = true;
				mouseX = event.button.x;
				mouseY = event.button.y;
			}
			else if (event.button.button == SDL_BUTTON_RIGHT) {
				rightMouseButtonPressed = true;
				mouseX = event.button.x;
				mouseY = event.button.y;
			}
		}
		else if (event.type == SDL_MOUSEBUTTONUP) {
			if (event.button.button == SDL_BUTTON_LEFT) {
				leftMouseButtonPressed = false;
			}
			else if (event.button.button == SDL_BUTTON_RIGHT) {
				rightMouseButtonPressed = false;
			}
		}
		else if (event.type == SDL_WINDOWEVENT) {
			if (event.window.event == SDL_WINDOWEVENT_RESIZED) {	//update the Viewport, when Window is resized
				width = event.window.data1;
				height = event.window.data2;
				float windowAspectRatio = (float)width / (float)height;

				// Window is wider than the content, so add black bars on the top and bottom
				if (windowAspectRatio > aspectRatio) {
					int viewportHeight = (int) ((float) width / aspectRatio);
					int viewportY = (height - viewportHeight) / 2;
					glViewport(0, viewportY, width, viewportHeight);
				}

				// Window is taller than (or equal to) the content, so add black bars on the left and right (with width 0 if equal)
				else {
					int viewportWidth = (int)((float)height * aspectRatio);
					int viewportX = (width - viewportWidth) / 2;
					glViewport(viewportX, 0, viewportWidth, height);
				}
			}
		}
	}
}

SDL_Surface* Window::getScreen() {
	return SDL_GetWindowSurface(window);
}