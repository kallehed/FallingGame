#include <glad/glad.h> 
#include <SDL/SDL.h>

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
	//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	//SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	//SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	//SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	//SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_Window* window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 500, 500, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
	if (window == NULL) std::cout << "ERROR::WINDOW_COULD_NOT_BE_CREATED\n";

	SDL_GLContext glcontext = SDL_GL_CreateContext(window);
	if (!glcontext) {
		std::cout << "ERROR::GLContext_NOT_CREATED\n";
	}
	SDL_GL_MakeCurrent(window, glcontext);
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	{int a = -2; std::cout << "color sizes: " << SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &a) << " " << a << '\n';}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, 100, 100);

	bool quit = false;
	bool fullscreen = false;
	while (!quit) {
		Uint64 start = SDL_GetPerformanceCounter();
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			switch (e.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_WINDOWEVENT:
				switch (e.window.event) {
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					int w, h;
					SDL_GL_GetDrawableSize(window, &w, &h);
					
					glViewport(0, 0, w, h);
					/*std::cout << "window PIXELsize: " << w << " h: " << h << '\n';
					SDL_GetWindowSize(window, &w, &h);
					std::cout << "window SCREENsize: " << w << ' ' << h << '\n';
					*/break;
				}
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.scancode == SDL_SCANCODE_0) {
					quit = true;
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_RETURN) {
					if (!fullscreen) {
						SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
					}
					else {
						SDL_SetWindowFullscreen(window, 0);
					}
					fullscreen = !fullscreen;
					
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_1) {
					float o;
					SDL_GetWindowOpacity(window, &o);
					SDL_SetWindowOpacity(window, o - 0.1f);
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_2) {
					float o;
					SDL_GetWindowOpacity(window, &o);
					SDL_SetWindowOpacity(window, o + 0.1f);
				}
				break;
			}
		}

		glClearColor(0.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT/* | GL_DEPTH_BUFFER_BIT */);
		SDL_GL_SwapWindow(window);

		Uint64 end = SDL_GetPerformanceCounter();

		float milliSecElapsed = ((end - start) / (float)SDL_GetPerformanceFrequency()) * 1000.f;
		
		Uint32 delay = static_cast<Uint32>(std::max(0.f, 1000.f / 60.f - milliSecElapsed));
		SDL_SetWindowTitle(window, std::to_string(1000.f / (milliSecElapsed + delay)).c_str());
		SDL_Delay(delay);
	}

	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}