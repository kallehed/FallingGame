#include "Coin.h"

#include "Game.h"

#include <cmath>

constexpr TEX::_ tex = TEX::coin_blurred;

Coin::Coin(Game& g, float start_y)
{
	constexpr float size = 0.0005f;

	r.x = rand_uni() * g.G_WIDTH;
	r.y = start_y;
	r.w = (float)g.d.tex_sizes[tex][0] * size;
	r.h = (float)g.d.tex_sizes[tex][1] * size;
	time_offset = /*g.l.timer +*/ rand_01()*5.f;
}

void Coin::logic(Game& g)
{
}

void Coin::draw(Game& g)
{
	float sin_val = std::sin((g.l.timer - time_offset) * 2.f);
	float width = r.w * (std::pow(std::abs(sin_val),0.9f)) * sign(sin_val);
	//float width = fmod(timer, w);
	g.d.draw_image(g.c, tex, r.x + r.w/2, r.y + r.h/2, width, r.h, 0.f);

	r.draw(g.d, { 1.f,0.f,0.f,0.4f });
}
