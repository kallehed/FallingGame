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
	if (!picked_up) {
		float sin_val = std::sin((g.l.timer - time_offset) * 2.f);
		float width = r.w * (std::pow(std::abs(sin_val), 0.9f)) * sign(sin_val);
		//float width = fmod(timer, w);
		g.d.draw_image(g.c, tex, r.x + r.w / 2, r.y + r.h / 2, width, r.h, 0.f);

		r.draw(g.d, { 1.f,0.f,0.f,0.4f });
	} 
	else
	{
		float sin_val = std::sin(g.l.timer * 15.f);
		float width = r.w * (std::pow(std::abs(sin_val), 0.9f)) * sign(sin_val);
		float height = r.h;
		float dec = (1.f + time_offset * time_offset / 1.f);
		width /= dec;
		height /= dec;
		
		g.d.draw_image(g.c, tex, r.x + r.w / 2, r.y + r.h / 2, width, height, 10.f*time_offset);

		r.draw(g.d, { 1.f,0.f,0.f,0.4f });

		time_offset += g.l.dt;
	}
}

void Coin::got_picked_up(Game& g)
{
	if (!picked_up) {
		picked_up = true;
		time_offset = 0.f;
	}
}
