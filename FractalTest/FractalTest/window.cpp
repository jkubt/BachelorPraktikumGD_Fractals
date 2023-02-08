#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>

#include "window.h"


Window::Window(int width, int height, const char *title) {
	SDL_Init(SDL_INIT_EVERYTHING);

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

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);
	glContext = SDL_GL_CreateContext(window);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init();

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		std::cin.get();
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
	initialDisplay = true;
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

float Window::updateBegin() {
	zoom = false;
	wheelY = 0;

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		ImGui_ImplSDL2_ProcessEvent(&event);

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
			int displayindex = SDL_GetWindowDisplayIndex(window);
			SDL_DisplayMode displaymode;
			SDL_GetCurrentDisplayMode(displayindex, &displaymode);
			float displaymodeWidth = displaymode.w;
			float displaymodeHeight = displaymode.h;
			if (displaymodeWidth / 16.0f > displaymodeHeight / 9.0f) {
				displaymodeWidth = displaymodeHeight / 9.0f * 16.0f;
			}
			if (displaymodeWidth / 16.0f < displaymodeHeight / 9.0f) {
				displaymodeHeight = displaymodeWidth / 16.0f * 9.0f;
			}
			SDL_SetWindowSize(window, displaymodeWidth*0.9f, displaymodeHeight*0.9f);
			glViewport(0, 0, displaymodeWidth*0.9f, displaymodeHeight*0.9f);
			width = displaymodeWidth*0.9f;
			height = displaymodeHeight*0.9f;
			if (initialDisplay) {
				SDL_SetWindowPosition(window, (displaymode.w-width) / 2, (displaymode.h-height) / 2);
				initialDisplay = false;
			}
		}
	}
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();
	return width / 2304.0f;
}

void Window::updateEnd() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(window);
}

void Window::destroy() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
}