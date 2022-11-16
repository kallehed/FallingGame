#pragma once

#include "misc.h"

class Layer;
class Camera;

class Drawer
{
public:
	Drawer(Layer& layer);

	void draw_rectangle(float x, float y, float w, float h, const Color& color);

	// draw generalized image at position(with respect to offset) with certain rotation around middle
	void draw_image(Camera& c, unsigned int texture, float x, float y, float w, float h, float rotation);

	void before_draw(Game& g);

private:
	unsigned int rectangle_program;
	unsigned int rectangle_VAO, rectangle_VBO, rectangle_EBO;
	int m_rectangle_u_color;
	int m_rectangle_u_offset; // set to camera offset

	unsigned int image_program;
	unsigned int image_VAO, image_VBO, image_EBO;
	int m_image_u_offset, m_image_u_rotation;

public:
	unsigned int bird_texture, mushroom_cap_texture;
};

