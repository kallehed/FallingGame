#include "Cloud.h"

#include "Game.h"

void Cloud::logic(Game& g)
{
	x += 0.001f * g.l.dt;
}

void Cloud::draw(Game& g)
{
	//g.d.draw_image(g.c, g.d.cloud_texture, x, y, 0.1f, 0.1f, 0.f);
}