#include "Layer.h"

#include <iostream>
#include <string>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "misc.h"
#include "Drawer.h"

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	// ignore non-significant error/warning codes
	//if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;

}

void Layer::init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0); // if not using depth buffers
	//SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	//SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	//SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	//SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // cont headers... graph


	m_window = SDL_CreateWindow("Falling Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, int(WIDTH*START_LENGTH_CONST), int(HEIGHT*START_LENGTH_CONST),
		SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
	if (m_window == NULL) std::cout << "ERROR::WINDOW_COULD_NOT_BE_CREATED\n";

	m_glcontext = SDL_GL_CreateContext(m_window);
	if (!m_glcontext) { std::cout << "ERROR::GLContext_NOT_CREATED\n"; }
	SDL_GL_MakeCurrent(m_window, m_glcontext);

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		std::cin.get();
	}

	if constexpr (false) {
		int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
	}

	// 1 for vsync, which works for all platforms tested so far
	SDL_GL_SetSwapInterval(1);

	{int a = -2; std::cout << "color sizes: " << SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &a) << " = " << a << '\n'; }
	{int a = -2; std::cout << "depth sizes: " << SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &a) << " = " << a << '\n'; }
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// don't know if this is necessary? maybe on high-dpi platforms?
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

// true -> end program
bool Layer::start_frame() 
{
	m_start = SDL_GetPerformanceCounter();

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
				std::cout << "window PIXELsize: " << w << " h: " << h << '\n';
				SDL_GetWindowSize(m_window, &w, &h);
				std::cout << "window SCREENsize: " << w << " h: " << h << '\n';
				break;
			}
			break;
		case SDL_KEYDOWN:
			//if (e.key.keysym.scancode < TOTAL_KEYS) { // checks should not be needed when using all possible keys ever?
				if (!key_down(e.key.keysym.scancode)) {
					m_keys_down[e.key.keysym.scancode] = true;
					m_keys_just_down[e.key.keysym.scancode] = true;
				}	
			//}
			break;
		case SDL_KEYUP:
			//if (e.key.keysym.scancode < TOTAL_KEYS) {
				m_keys_down[e.key.keysym.scancode] = false;
			//}
			break;
		}
	}


	if (key_just_down(SDL_SCANCODE_RETURN)) { // Switch fullscreen
		bool is_fullscreen = SDL_GetWindowFlags(m_window) & SDL_WINDOW_FULLSCREEN; // should work?
		SDL_SetWindowFullscreen(m_window, is_fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
		/*if (!m_fullscreen) {
			SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		}
		else {
			SDL_SetWindowFullscreen(m_window, 0);
		}
		m_fullscreen = !m_fullscreen;
		*/
	}
	
	if constexpr (DEV_TOOLS) {

		if (key_just_down(SDL_SCANCODE_Q)) { // query
			SDL_DisplayMode mode;
			SDL_GetWindowDisplayMode(m_window, &mode);
			std::cout << "query:\nformat: " << mode.format << "\nwidth: " << mode.w
				<< "\nheight: " << mode.h << "\n driverdata: " << mode.driverdata << "\n";

			auto df = SDL_GetWindowFlags(m_window) & SDL_WINDOW_FULLSCREEN_DESKTOP;
			std::cout << "is desktop fullscreen: " << df << "\n";

			auto f = SDL_GetWindowFlags(m_window) & SDL_WINDOW_FULLSCREEN;
			std::cout << "is REAL fullscreen: " << f << "\n";

			auto all = SDL_GetWindowFlags(m_window);
			std::cout << "all flags: " << all << "\n";
		}

		if (key_just_down(SDL_SCANCODE_DELETE)) { // DELETE- escape hatch
			return true;
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
		{
			if (key_just_down(SDL_SCANCODE_3))
			{
				auto str = "\x0a\x0a\x57\x4f\x57\x20\x21\x20\x59\x6f\x75\x20\x6a\x75\x73\x74\x20\x72\x65\x63\x65\x69\x76\x65\x64\x20\x61\x6e\x20\x45\x72\x72\x6f\x72\x21\x20\x20\xf0\x9f\x98\x84\x20\xf0\x9f\x98\x81\x20\xf0\x9f\x98\x86\x20\xf0\x9f\x98\x85\x20\xf0\x9f\x98\x82\x20\xf0\x9f\xa4\xa3\x20\xf0\x9f\xa5\xb2";
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "erry", str, m_window);

			}
			if (key_just_down(SDL_SCANCODE_4)) {

			}
		}
	}
	return false;
}

void Layer::end_frame()
{
	SDL_GL_SwapWindow(m_window);
	Uint64 end = SDL_GetPerformanceCounter();

	// delay if framerate going too fast
	float milliSecElapsed = ((end - m_start) / (float)SDL_GetPerformanceFrequency()) * 1000.f;
	
	Uint32 delay = static_cast<Uint32>(std::max(0.f, 1000.f / MAX_FPS - milliSecElapsed));
	dt = (milliSecElapsed + delay) / 1000.f;
	dt = std::min(1.f / MIN_FPS, dt);

	// slow
	//SDL_SetWindowTitle(m_window, std::to_string(1000.f / (milliSecElapsed + delay)).c_str());
	SDL_Delay(delay);
}

bool Layer::key_down(SDL_Scancode key) const
{
	return m_keys_down[key];
}

bool Layer::key_just_down(SDL_Scancode key) const
{
	return m_keys_just_down[key];
}

unsigned int Layer::compile_shader_from_file(int type, const char* path, const char* error_msg)
{
	unsigned int shader = glCreateShader(type);

	{
		// load from file
		std::string shaderSource;
		{
			std::ifstream f(path);
			if (f) {
				std::getline(f, shaderSource, '\0');
			}
			else {
				std::cout << "ERROR::FILE_NOT_FOUND: " << path << "\n";
			}
		}
	
		const char* str = shaderSource.c_str();
		const char* strs[] = {Drawer::SHADER_START_TEXT, str};
		glShaderSource(shader, sizeof(strs)/sizeof(const char*), strs, NULL); // last null because null-terminated-strings
	}
	glCompileShader(shader);

	int success;
	char infoLog[512];

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << error_msg << infoLog << std::endl;
	}
	return shader;
}

unsigned int Layer::compile_shader_program(const char* vertexShaderSource, const char* fragmentShaderSource, const char* name_for_error)
{
	unsigned int vertexShader = Layer::compile_shader_from_file(GL_VERTEX_SHADER, vertexShaderSource, "ERROR::SHADER::VERTEX::COMPILATION_FAILED ");

	// fragment shader
	unsigned int fragmentShader = Layer::compile_shader_from_file(GL_FRAGMENT_SHADER, fragmentShaderSource, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED ");

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

std::array<int, 2> Layer::load_texture(const char* path, unsigned int* image, int wrapping_x, int wrapping_y)
{
	glGenTextures(1, image);
	glBindTexture(GL_TEXTURE_2D, *image); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping_x);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping_y);
	{
		float borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture: \'" << path << '\'' << std::endl;
	}
	stbi_image_free(data);
	return { width, height };
}
