#pragma once

#include "misc.h"


#include <array>

class Layer;
class Camera;
class Player;
class CoinParticle;

class Drawer
{
public:
	Drawer(Game& g);

	void draw_rectangle(float x, float y, float w, float h, const Color& color);

	// draws middle of image at x,y. Offset by camera 
	void draw_image(Camera& c, TEX::_ tex, float x, float y, float w, float h, float rotation);

	void draw_sky(Game& g);

	void draw_sides(Player& p);

	void draw_cloud(Game& g, TEX::_ tex, float x, float y, float z, float w, float h);

	void draw_coin_particle(CoinParticle&);

	void before_draw(Game& g);

protected:
	// wrapping to GL_CLAMP_TO_BORDER will make it transparent after border(disables artifacts)
	// returns width and height of image
	std::array<int, 2> load_texture(const char* path, unsigned int* image, int wrapping_x, int wrapping_y);

private:

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
	unsigned int cloud_VAO, cloud_VBO;

	unsigned int coin_particle_program;
	unsigned int coin_particle_VAO, coin_particle_VBO;
	constexpr static int COIN_PARTICLE_FLOATS_IN_ARRAY = 5;

	/*
	* float g_death_y; // y where you die
	* float g_cam_y; // canera y
	* float g_timer; // time since level started
	* float g_w; // how much bigger the width is to the height, CONSTANT = 1.6 = g.l.WIDTH
	*/
	constexpr static int UBO_GLOBAL_SIZE = 4 * sizeof(float);
	unsigned int ubo_globals;

public:
	std::array<unsigned int, TEX::TOTAL> texs;
	std::array<std::array<int, 2>, TEX::TOTAL> tex_sizes;
};

