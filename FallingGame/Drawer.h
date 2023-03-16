#pragma once

#include "misc.h"

#include <array>

class Layer;
class Camera;
class Player;
class CoinParticle;

struct Character {
	unsigned int width; // width of glyph
	unsigned int height; // height of glyph
	int bearing_x; // x start position
	int bearing_y; // y start position, kind of?
	int advance; // x length to next glyph
};

class Drawer
{
public:
	Drawer(Game& g);

	void draw_text(const char* text, Color color, float x, float y, float scale);

	void draw_rectangle(float x, float y, float w, float h, const Color& color);

	// draws middle of image at x,y. Offset by camera 
	void draw_image(Camera& c, TEX::_ tex, float x, float y, float w, float h, float rotation);

	void draw_sky(Game& g);

	void draw_sides(Player& p);

	void draw_clouds(Game& g);

	void draw_cloud(Game& g, TEX::_ tex, float x, float y, float z, float w, float h);

	void draw_coin_particle(CoinParticle&);

	void draw_bird(Camera& c, TEX::_ bird_tex, float x, float y, float rotation, float width, float height, float time_since_coin);

	void before_draw(Game& g);

protected:
	// wrapping to GL_CLAMP_TO_BORDER will make it transparent after border(disables artifacts)
	// returns width and height of image
	std::array<int, 2> load_texture(const char* path, unsigned int* image, int wrapping_x, int wrapping_y);

private:
	// how many there are to draw
	static constexpr int CHARACTERS = 128;
	static constexpr int CHARACTERS_START_AT = 32;
	// data for all the characters; how to draw them
	std::array<Character, CHARACTERS> m_characters;
	// takes in the character and the x offset
	unsigned int m_text_program;
	unsigned int m_main_font_tex;
	unsigned int m_text_VAO, m_text_VBO;
	int m_text_u_offset_scale;
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

	//

	static constexpr int UBO_TEXT_SIZE = CHARACTERS * sizeof(float);

	unsigned int standard_rect_EBO; // left bottom, left top, right top, right bottom

	unsigned int rectangle_program;
	unsigned int rectangle_VAO, rectangle_VBO;
	int m_rectangle_u_color;
	int m_rectangle_u_offset; // set to camera offset

	unsigned int image_program;
	unsigned int image_VAO, image_VBO;
	int m_image_u_offset, m_image_u_rotation;

	unsigned int sky_program;
	unsigned int sky_VAO, sky_VBO;
	unsigned int sky_u_offset;
	float sky_height_per_sky;

	unsigned int sides_program;
	unsigned int sides_VAO, sides_VBO;
	int sides_u_offset;
	float sides_height_per_image;

	unsigned int cloud_program;
	static constexpr int FLOATS_PER_CLOUD_POS = 4;
	static constexpr int TEX_AND_Z_FLOATS = 2;
	unsigned int cloud_VAO, cloud_VBO_pos, cloud_VBO_tex_z;

	unsigned int coin_particle_program;
	unsigned int coin_particle_VAO, coin_particle_VBO;
	constexpr static int COIN_PARTICLE_FLOATS_IN_ARRAY = 5;

	unsigned int bird_program;
	unsigned int bird_VAO;
	int m_bird_u_offset, m_bird_u_rotation_width_height, m_bird_u_time_since_coin;

	/*
	* float g_death_y; // y where you die
	* float g_cam_y; // canera y
	* float g_timer; // time since level started
	* float g_w; // how much bigger the width is to the height, CONSTANT = 1.6 = g.l.WIDTH
	*/
	constexpr static int UBO_GLOBAL_SIZE = 4 * sizeof(float);
	unsigned int ubo_globals; // BINDING 0.

public:
	std::array<unsigned int, TEX::TOTAL> texs;
	std::array<std::array<int, 2>, TEX::TOTAL> tex_sizes;
};

