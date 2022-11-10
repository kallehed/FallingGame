#pragma once

#include <SDL/SDL.h>

class Layer;


class Drawer
{
public:
	Drawer(Layer& layer);

	void draw_rectangle(float x, float y, float w, float h, SDL_Color color);

private:
	unsigned int rectangle_program;
	unsigned int rectangle_VAO, rectangle_VBO, rectangle_EBO;
};

