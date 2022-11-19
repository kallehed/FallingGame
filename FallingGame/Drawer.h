#pragma once

#include "misc.h"

class Layer;
class Camera;
class Player;

class Drawer
{
public:
	Drawer(Game& g);

	void draw_rectangle(float x, float y, float w, float h, const Color& color);

	// draws middle of image at x,y. Offset by camera 
	void draw_image(Camera& c, unsigned int texture, float x, float y, float w, float h, float rotation);

	void draw_sky(Player& p);

	void draw_sides(Player& p);

	void before_draw(Game& g);

protected:
	// wrapping to GL_CLAMP_TO_BORDER will make it transparent after border(disables artifacts)
	void load_texture(const char* path, unsigned int* image, int wrapping);

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
public:
	unsigned int bird_texture, bird_closed_texture, mushroom_cap_texture, mushroom_stem_texture, side_background_texture, sky_texture, sky_blurred_texture;
};

