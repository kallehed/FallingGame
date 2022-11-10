#include "Layer.h"

#include <iostream>
#include <string>
#include <fstream>

Layer::Layer()
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


	m_window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 500, 500, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
	if (m_window == NULL) std::cout << "ERROR::WINDOW_COULD_NOT_BE_CREATED\n";

	m_glcontext = SDL_GL_CreateContext(m_window);

	if (!m_glcontext) {
		std::cout << "ERROR::GLContext_NOT_CREATED\n";
	}
	SDL_GL_MakeCurrent(m_window, m_glcontext);

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		std::cin.get();
	}
	SDL_GL_SetSwapInterval(1);

	{int a = -2; std::cout << "color sizes: " << SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &a) << " " << a << '\n'; }
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	{
		int w, h;
		SDL_GL_GetDrawableSize(m_window, &w, &h);
		glViewport(0, 0, w, h);
	}
}

Layer::~Layer()
{
	SDL_GL_DeleteContext(m_glcontext);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

bool Layer::start_frame()
{
	m_start = SDL_GetPerformanceCounter();

	glClearColor(0.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT/* | GL_DEPTH_BUFFER_BIT */);

	m_keys_just_down = { false }; // reset before getting events
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		switch (e.type) {
		case SDL_QUIT:
			return true;
			break;
		case SDL_WINDOWEVENT:
			switch (e.window.event) {
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				int w, h;
				SDL_GL_GetDrawableSize(m_window, &w, &h);

				glViewport(0, 0, w, h);
				/*std::cout << "window PIXELsize: " << w << " h: " << h << '\n';
				SDL_GetWindowSize(window, &w, &h);
				std::cout << "window SCREENsize: " << w << ' ' << h << '\n';
				*/break;
			}
			break;
		case SDL_KEYDOWN:
			if (e.key.keysym.scancode < TOTAL_KEYS) {
				if (!key_down(e.key.keysym.scancode)) {
					m_keys_down[e.key.keysym.scancode] = true;
					m_keys_just_down[e.key.keysym.scancode] = true;
				}	
			}
			break;
		case SDL_KEYUP:
			if (e.key.keysym.scancode < TOTAL_KEYS) {
				m_keys_down[e.key.keysym.scancode] = false;
			}
			break;
		}
	}

	if (key_just_down(SDL_SCANCODE_DELETE)) {
		return true;
	}
	if (key_just_down(SDL_SCANCODE_RETURN)) {
		if (!m_fullscreen) {
			SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		}
		else {
			SDL_SetWindowFullscreen(m_window, 0);
		}
		m_fullscreen = !m_fullscreen;
	}
	{
		constexpr float op_change = 0.04f;
		if (key_down(SDL_SCANCODE_1)) {
			float o;
			SDL_GetWindowOpacity(m_window, &o);
			SDL_SetWindowOpacity(m_window, o - op_change);
		}
		if (key_down(SDL_SCANCODE_2)) {
			float o;
			SDL_GetWindowOpacity(m_window, &o);
			SDL_SetWindowOpacity(m_window, o + op_change);
		}
	}
	return false;
}

void Layer::end_frame()
{
	SDL_GL_SwapWindow(m_window);

	Uint64 end = SDL_GetPerformanceCounter();

	float milliSecElapsed = ((end - m_start) / (float)SDL_GetPerformanceFrequency()) * 1000.f;

	Uint32 delay = static_cast<Uint32>(std::max(0.f, 1000.f / 60.f - milliSecElapsed));
	SDL_SetWindowTitle(m_window, std::to_string(1000.f / (milliSecElapsed + delay)).c_str());
	SDL_Delay(delay);
}

bool Layer::key_down(SDL_Scancode key)
{
	return m_keys_down[key];
}

bool Layer::key_just_down(SDL_Scancode key)
{
	return m_keys_just_down[key];
}

unsigned int Layer::compile_shader_from_file(int type, const char* path, const char* error_msg)
{
	unsigned int shader = glCreateShader(type);

	// load from file
	char* shaderSource = new char[100000];
	{
		std::ifstream f(path);
		if (f) {
			f.getline(shaderSource, 100000, '\0');
		}
		else {
			std::cout << "ERROR::FILE_NOT_FOUND: " << path << "\n";
		}
	}

	glShaderSource(shader, 1, (&shaderSource), NULL);
	glCompileShader(shader);

	int success;
	char infoLog[512];

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << error_msg << infoLog << std::endl;
	}
	delete[] shaderSource;
	return shader;
}

unsigned int Layer::compile_shader_program(const char* vertexShaderSource, const char* fragmentShaderSource, const char* name_for_error)
{
	unsigned int vertexShader = compile_shader_from_file(GL_VERTEX_SHADER, vertexShaderSource, "ERROR::SHADER::VERTEX::COMPILATION_FAILED ");

	// fragment shader
	unsigned int fragmentShader = compile_shader_from_file(GL_FRAGMENT_SHADER, fragmentShaderSource, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED ");

	// shader program
	unsigned int shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	int  success;
	char infoLog[512];

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::PROGRAM::SHADERPROGRAM::COMPILATION_FAILED, NAME: " << name_for_error << "\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}
