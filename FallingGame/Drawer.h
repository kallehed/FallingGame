#pragma once

class Layer;

struct Color {
	float r;
	float g;
	float b;
	float a;
};

class Drawer
{
public:
	Drawer(Layer& layer);

	void draw_rectangle(float x, float y, float w, float h, const Color& color);

private:
	unsigned int rectangle_program;
	unsigned int rectangle_VAO, rectangle_VBO, rectangle_EBO;
	int m_rectangle_u_color;
};

