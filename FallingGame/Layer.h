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

private:
	unsigned int compile_shader_from_file(int type, const char* path, const char* error_msg);

	SDL_Window* m_window = NULL;
	SDL_GLContext m_glcontext = NULL;
	bool m_fullscreen = false;
	Uint64 m_start = 0; // time when frame started

	constexpr static int TOTAL_KEYS = 256;
	std::array<bool, TOTAL_KEYS> m_keys_down = { false };
	std::array<bool, TOTAL_KEYS> m_keys_just_down = { false };
};