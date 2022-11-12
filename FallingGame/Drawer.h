#pragma once

#include "misc.h"

class Layer;

class Drawer
{
public:
	Drawer(Layer& layer);

	void draw_rectangle(float x, float y, float w, float h, const Color& color);

	void before_draw(Game& g);

private:
	unsigned int rectangle_program;
	unsigned int rectangle_VAO, rectangle_VBO, rectangle_EBO;
	int m_rectangle_u_color;
	int m_rectangle_u_offset;
};

