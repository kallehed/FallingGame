#include "Layer.h"
#include <SDL/SDL_rwops.h>

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

	SDL_Log("KALLE SDL big error happened BIG ERROORRRRRRRRR");
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
	SDL_Log("KALLE SDL app started");
	CHKSDL(SDL_Init(SDL_INIT_VIDEO));

#ifndef __ANDROID__
	CHKSDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE));
	CHKSDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4));
	CHKSDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3));
	
#else
	CHKSDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES));
	CHKSDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3));
	CHKSDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2));

#endif
	CHKSDL(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1));
	//SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
	CHKSDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG));
	
	CHKSDL(SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8));
	CHKSDL(SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8));
	CHKSDL(SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8));
	CHKSDL(SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8)); // cont headers... graph
	//CHKSDL(SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 64));
	CHKSDL(SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0)); // if not using depth buffers
	CHKSDL(SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0)); // not used

	//CHKSDL(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1));
	//CHKSDL(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 1));

	m_window = SDL_CreateWindow("Falling Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, int(WIDTH*START_LENGTH_CONST), int(HEIGHT*START_LENGTH_CONST),
		SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
	CHKSDL(!m_window);
	m_glcontext = SDL_GL_CreateContext(m_window);
	CHKSDL(!m_glcontext);
	CHKSDL(SDL_GL_MakeCurrent(m_window, m_glcontext));

	// I changed the name of the load function in gladES file
	if (!gladLoadGLES2Loader((GLADloadproc)SDL_GL_GetProcAddress))
		SDL_LogError(0, "Failed to initialize GLAD");

	// debug mode	
#if 1
	{
		int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		bool debug_on = flags & GL_CONTEXT_FLAG_DEBUG_BIT;
		SDL_Log("Using debug context: %d", debug_on);
		if (debug_on)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
	}
#endif

	// 1 for vsync, which works for all platforms tested so far

	CHKSDL(SDL_GL_SetSwapInterval(1));

	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	{
		int r = -2, g = -2, b = -2, a = -2, d = -2, s = -2, buf=-2, dbuf=-2;
		auto res_r = SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &r); 
		auto res_g = SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &g); 
		auto res_b = SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &b);
		auto res_a = SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &a);
		auto res_buf = SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE, &buf);
		auto res_db = SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &dbuf);

		// these two may crash on linux specifically
		auto res_d = SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &d);
		auto res_s = SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &s);

		SDL_Log("Red RES: %d, Size: %d ", res_r, r);
		SDL_Log("Green RES: %d, Size: %d ", res_g, g);
		SDL_Log("Blue RES: %d, Size: %d ", res_b, b);
		SDL_Log("Alpha RES: %d, Size: %d ", res_a, a);
		SDL_Log("Buffer RES: %d, Size: %d ", res_buf, buf);
		SDL_Log("Doublebuffer RES: %d, Value: %d", res_db, dbuf);

		SDL_Log("Depth RES: %d, Size: %d ", res_d, d);
		SDL_Log("Stencil RES: %d, Size: %d", res_s, a);
	}

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

	//SDL_Log("KALLE SDL Start of frame");

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
			{
				int w, h;
				SDL_GL_GetDrawableSize(m_window, &w, &h);
				glViewport(0, 0, w, h);
				SDL_Log("Window PIXELsize: w: %d, h: %d ", w, h);
				SDL_GetWindowSize(m_window, &w, &h);
				SDL_Log("Window SCREENsize: w: %d, h: %d ", w, h);
				break;
			}
			}
			break;
		case SDL_KEYDOWN:
			//if (e.key.keysym.scancode < TOTAL_KEYS) { // checks should not be needed when using all possible keys ever?
				if (!key_down(e.key.keysym.scancode)) {
					m_keys_down[e.key.keysym.scancode] = true;
					m_keys_just_down[e.key.keysym.scancode] = true;
				}	
				//SDL_Log("Just jmotioned!");
			//}
			break;
		case SDL_KEYUP:
			//if (e.key.keysym.scancode < TOTAL_KEYS) {
				m_keys_down[e.key.keysym.scancode] = false;
			//}
			break;
		case SDL_FINGERMOTION:
		case SDL_FINGERDOWN: // will NOT be called on a computer
			m_keys_just_down[SDL_SCANCODE_O] = true;

			m_keys_down[SDL_SCANCODE_A] = false;
			m_keys_down[SDL_SCANCODE_D] = false;

			m_keys_down[(e.tfinger.x < 0.5f) ? (SDL_SCANCODE_A) : (SDL_SCANCODE_D)] = true; 

			break;
		case SDL_FINGERUP:
			m_keys_down[SDL_SCANCODE_A] = false;
			m_keys_down[SDL_SCANCODE_D] = false;

			break;
		}
	}


	if (key_just_down(SDL_SCANCODE_RETURN)) { // Switch fullscreen
		bool is_fullscreen = SDL_GetWindowFlags(m_window) & SDL_WINDOW_FULLSCREEN_DESKTOP; // should work?
		SDL_SetWindowFullscreen(m_window, is_fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
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
	SDL_assert(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER);
	unsigned int shader = glCreateShader(type);

	{
		// load from file
		SDL_RWops* io = SDL_RWFromFile(path, "r");
		CHKSDL(!io);
		if (io == NULL) goto GOTO_AFTER_READFILE;
		auto file_size = io->size(io);
		char* str = new char[file_size + 1]; // one extra for null termination
		if (!SDL_RWread(io, str, 1, file_size)) {
			SDL_Log("ERROR in SDL_RWread at file(probably too big) %s, error: %s", path, SDL_GetError());
		}
		SDL_RWclose(io);
		str[file_size] = '\0';

		// these are specific for vertex and fragment shaders.
		const char* sh_end = ""; 
		const char* sh_start = "";

		switch (type) {
		case GL_VERTEX_SHADER:
			sh_start =
				"\n"
				"out vec2 f_screenCoord;"
				"\n";
			sh_end =
				"\n"
				"	gl_Position.x /= g_w;"
				"	gl_Position.y /= g_h;"
				"	f_screenCoord = gl_Position.xy;"
				"}\n";
			break;
		case GL_FRAGMENT_SHADER:
			sh_start =
				"\n"
				"out vec4 FragColor;"
				"in vec2 f_screenCoord;"
				"\n";
			sh_end =
				"\n{"
				"    float t = g_timer - (f_screenCoord.x + 1.0)*2.0 - (f_screenCoord.y + 1.0)*2.0;"
				"    t /= 1.0f;"
				"    float x = 0.4;"
				"    float y = 1.2 * (1.0 - x);"
				"    FragColor.b *= x + y * abs(1.0*cos(t * 0.11));"
				"    FragColor.g *= x + y * abs(1.0*cos(t * 0.12));"
				"    FragColor.r *= x + y * abs(1.0*cos(t * 0.13));"
				"}}\n";
			break;
		default:
			SDL_assert(false);
		}

		static constexpr const char* SHADER_START_FOR_ALL =
#ifndef __ANDROID__
			"#version 430 core\n"
#else
			"#version 320 es\n"
			"precision highp float;\n"
#endif
			"layout(std140, binding = 0) uniform Globals\n"
			"{"
			"	float g_death_y;"
			"	float g_cam_y;"
			"	float g_timer;"
			"	float g_w;"
			"   float g_h;"
			"};\n";
	
		const char* strs[] = { SHADER_START_FOR_ALL, sh_start, str, sh_end};
		//SDL_Log("SDL KALLE Compiling a new shader: %s. with following code: %s %s", path, strs[0], strs[1]);
		glShaderSource(shader, sizeof(strs)/sizeof(const char*), strs, NULL); // last null because null-terminated-strings
		delete[] str;
	}
	GOTO_AFTER_READFILE:
	glCompileShader(shader);

	int success;
	char infoLog[512];

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		SDL_Log("SDL KALLE ERROR compile shader in %s: %s ... INFOLOG: %s$", path, error_msg, infoLog);
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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// new
	
	SDL_RWops* io = SDL_RWFromFile(path, "rb");
	CHKSDL(!io);
	if (!io) return { 100,100 };
	Sint64 file_size = io->size(io);
	unsigned char* buffer = new unsigned char[file_size];
	
	SDL_RWread(io, buffer, 1, file_size);
	SDL_RWclose(io);

	int width, height, nrChannels;
	unsigned char* data = stbi_load_from_memory(buffer, (int)file_size, &width, &height, &nrChannels, 0);
	delete [] buffer;
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		SDL_Log("SDL KALLE Failed to load texture: \'%s\'", path);
	}
	stbi_image_free(data);
	return { width, height };
}
