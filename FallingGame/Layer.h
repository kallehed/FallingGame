#pragma once

#include "misc.h"

//#ifndef __ANDROID__

#include <glad/glad.h>
#include <SDL/SDL.h>

//#else

//#include <gladES/glad.h>
//#include <SDL/SDL.h>
//#include <SDL/SDL_opengles2.h>
//#include <GLES3/gl32.h>

//#endif

#include <array>



// Layer for SDL: window management, events, some opengl, some image loading ...
class Layer
{
public:
	void init();
	~Layer();

	bool start_frame(); // true -> end program
	void end_frame();

	bool key_down(SDL_Scancode key) const; // returns whether key is currently down
	bool key_just_down(SDL_Scancode key) const;
	static unsigned int compile_shader_program(const char* vertexShaderSource, const char* fragmentShaderSource, const char* name_for_error);

	// wrapping to GL_CLAMP_TO_BORDER will make it transparent after border(disables artifacts)
	// returns width and height of image
	static std::array<int, 2> load_texture(const char* path, unsigned int* image, int wrapping_x, int wrapping_y);

	float dt = 0.f; // delta time for last frame
#if defined(KALLE_GAME_VERTICAL) // Mobile
	static constexpr float WIDTH = 0.8f; // of SCREEN
	//static constexpr float HEIGHT = 1.890277777777777f; // of SCREEN
	static constexpr float HEIGHT = WIDTH * (2722.f / 1440.f); // my exact phone dimensions;
#else // Computer
	static constexpr float WIDTH = 1.6f; // of SCREEN
	static constexpr float HEIGHT = 1.0f; // of SCREEN

#endif
	

private:
	static unsigned int compile_shader_from_file(int type, const char* path, const char* error_msg);

	static constexpr float START_LENGTH_CONST = 600.f; // used for setting inital width/height

	static constexpr float MIN_FPS = 60.f;
	static constexpr float MAX_FPS = 240.f;

	SDL_Window* m_window = NULL;
	SDL_GLContext m_glcontext = NULL;
	bool m_fullscreen = false;
	Uint64 m_start = 0; // time when frame started, used for FPS calculation

	// if you lower the total keys, things might crash when pressing out of bounds keys in the array
	static constexpr int TOTAL_KEYS = SDL_NUM_SCANCODES;
	std::array<bool, TOTAL_KEYS> m_keys_down = { false };
	std::array<bool, TOTAL_KEYS> m_keys_just_down = { false };

	
};
