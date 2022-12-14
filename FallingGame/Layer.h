#pragma once

#include <glad/glad.h> 
#include <SDL/SDL.h>
#include <array>

// Layer for SDL: window management, events, ...
class Layer
{
public:
	Layer();
	~Layer();

	bool start_frame(); // true -> end program
	void end_frame();

	bool key_down(SDL_Scancode key); // returns whether key is currently down
	bool key_just_down(SDL_Scancode key);

	unsigned int compile_shader_program(const char* vertexShaderSource, const char* fragmentShaderSource, const char* name_for_error);

	float dt = 0.f; // delta time for last frame
	constexpr static float WIDTH = 1.6f; // of SCREEN
	constexpr static float HEIGHT = 1.0f; // of SCREEN
private:
	unsigned int compile_shader_from_file(int type, const char* path, const char* error_msg);

	constexpr static float START_LENGTH_CONST = 500.f; // used for setting inital width/height

	constexpr static float MIN_FPS = 60.f;
	constexpr static float MAX_FPS = 240.f;

	SDL_Window* m_window = NULL;
	SDL_GLContext m_glcontext = NULL;
	bool m_fullscreen = false;
	Uint64 m_start = 0; // time when frame started

	constexpr static int TOTAL_KEYS = 256;
	std::array<bool, TOTAL_KEYS> m_keys_down = { false };
	std::array<bool, TOTAL_KEYS> m_keys_just_down = { false };

	
};