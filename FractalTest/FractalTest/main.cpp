#include <iostream>
#define SDL_MAIN_HANDLED
#include <SDL.h>

#pragma comment(lib,"SDL2.lib")
#pragma comment(lib,"opengl32.lib")

int main(int argc, char** argv){
	SDL_Window* window;
	SDL_Init(SDL_INIT_EVERYTHING);

	window = SDL_CreateWindow("FractalTest", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext glContext = SDL_GL_CreateContext(window);

	bool close = false;
	while (!close) {

		SDL_GL_SwapWindow(window);

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				close = true;
			}
		}
	}

	return 0;
}

