#pragma once

#include "misc.h"

#include <array>

class Layer;
class Camera;
class Player;
class CoinParticle;
struct CloudHandler;

struct Character {
	unsigned int width; // width of glyph
	unsigned int height; // height of glyph
	int bearing_x; // x start position
	int bearing_y; // y start position, kind of?
	long long advance; // x length to next glyph
};

class Drawer
{
public:
	void init(Game& g);

	void draw_text(const char* text, Color color, float x, float y, float scale);

	// draw a rectangle RELATIVE to the playeer.
	void draw_rectangle(float x, float y, float w, float h, const Color& color);

	// draws middle of image at x,y. Offset by camera 
	void draw_image(TEX::_ tex, float x, float y, float w, float h, float rotation);

	void draw_sky(Game& g, Camera& c);

	void draw_sides(Player& p);

	void draw_clouds(CloudHandler& ch);

	void draw_coin_particle(CoinParticle&);

	void draw_bird(Camera& c, TEX::_ bird_tex, float x, float y, float rotation, float width, float height, float time_since_coin);

	// updates relevant global shader values
	void before_draw(Game& g, float death_y, float cam_y, float timer);

	void draw_fps(float dt);

protected:
	// loads all programs including their source code, including hot reloads
	void load_all_programs();

private:
	// how many there are to draw
	static constexpr int CHARACTERS = 128;
	static constexpr int CHARACTERS_START_AT = 32;
	// data for all the characters; how to draw them
	std::array<Character, CHARACTERS> m_characters;
	// takes in the character and the x offset
	unsigned int m_text_program = 0;
	unsigned int m_main_font_tex;
	unsigned int m_text_VAO, m_text_VBO;
	// contains x, y and scale
	int m_text_u_offset_scale; 
	int m_text_u_color;
	// *IMPORTANT* this is the max buffer of chars allowed to be rendered IN ONE CALL
	static constexpr int TEXT_MAX_CHARS_RENDER = 256;
	// char and x offset
	static constexpr int TEXT_ATTRIBUTES = 2;
	// bytes per instance
	static constexpr int TEXT_BYTES_PER = sizeof(float) * TEXT_ATTRIBUTES;
	// one float for character, then one for x offset. Then for `above` times. Float.
	static constexpr int TEXT_ATTRIBUTE_BYTES = TEXT_BYTES_PER * TEXT_MAX_CHARS_RENDER;

	// text uniform buffer object
	static constexpr int U_TEXT_ATTRIBUTES_PER = 4;
	static constexpr int U_TEXT_FLOATS = U_TEXT_ATTRIBUTES_PER * CHARACTERS;
	static constexpr int U_TEXT_BYTES = U_TEXT_FLOATS * sizeof(float);
	// Contains `CHARACTERS` times of: 
	// Width
	// Height
	// Bearing X
	// Bearing Y

	static constexpr int UBO_TEXT_SIZE = CHARACTERS * sizeof(float);

	unsigned int standard_rect_EBO; // left bottom, left top, right top, right bottom


	// PROGRAMS: INITIALIZE ALL TO ZERO FOR HOT RELOADING, ALLOWING DELETION AT START

	unsigned int rectangle_program = 0;
	unsigned int rectangle_VAO, rectangle_VBO;
	int m_rectangle_u_color;

	unsigned int image_program = 0;
	unsigned int image_VAO, image_VBO;
	int m_image_u_offset, m_image_u_rotation;

	unsigned int sky_program = 0;
	unsigned int sky_VAO, sky_VBO;
	unsigned int sky_u_offset;
	float sky_height_per_sky;

	unsigned int sides_program = 0;
	unsigned int sides_VAO, sides_VBO;
	int sides_u_offset;
	float sides_height_per_image;

	unsigned int cloud_program = 0;
	static constexpr int FLOATS_PER_CLOUD_POS = 4;
	static constexpr int TEX_AND_Z_FLOATS = 2;
	unsigned int cloud_VAO, cloud_VBO_pos, cloud_VBO_tex_z;

	unsigned int coin_particle_program = 0;
	unsigned int coin_particle_VAO, coin_particle_VBO;
	constexpr static int COIN_PARTICLE_FLOATS_IN_ARRAY = 5;

	unsigned int bird_program = 0;
	unsigned int bird_VAO;
	int m_bird_u_offset, m_bird_u_rotation_width_height, m_bird_u_time_since_coin;

	/*
	* float g_death_y; // y where you die
	* float g_cam_y; // canera y
	* float g_timer; // time since level started
	* float g_w; // how much bigger the width is to the height, CONSTANT = 1.6 probs = g.l.WIDTH
	*/
	// shader start text and stuff is in Layer.cpp::compile_shader_from_file

	// MORE NOTES:
	// All vertex output vec2 f_screenCoord, and all fragment shaders input it.
	// f_screenCoord is automatically set at the end of all vertex shaders based on gl_Position
	// All fragment shaders output vec4 FragColor.
	// gl_Position will be modified after all vertex shaders
	// 

	private:
	static constexpr int UBO_GLOBAL_BIND = 0;
	static constexpr const char* UBO_GLOBAL_NAME = "Globals"; // never used
	static constexpr int UBO_GOBAL_FLOATS = 5;
	static constexpr int UBO_GLOBAL_SIZE = UBO_GOBAL_FLOATS * sizeof(float);
	unsigned int ubo_globals; // BINDING 0.

public:
	std::array<unsigned int, TEX::TOTAL> texs;
	std::array<std::array<int, 2>, TEX::TOTAL> tex_sizes;
};

