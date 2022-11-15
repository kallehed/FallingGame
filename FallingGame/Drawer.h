#pragma once

#include "misc.h"

class Layer;

class Drawer
{
public:
	Drawer(Layer& layer);

	void draw_rectangle(float x, float y, float w, float h, const Color& color);

	void draw_image(float x, float y, float w, float h, float rotation);

	void before_draw(Game& g);

private:
	unsigned int rectangle_program;
	unsigned int rectangle_VAO, rectangle_VBO, rectangle_EBO;
	int m_rectangle_u_color;
	int m_rectangle_u_offset; // set to camera offset

	unsigned int image_program;
	unsigned int image_VAO, image_VBO, image_EBO;
	int m_image_u_offset, m_image_u_rotation;

	unsigned int bird_texture;

	Pos global_offset = {0.f,0.f};
};

