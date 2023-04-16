#include "Coin.hpp"

#include "Game.hpp"

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

void Coin::draw(Game& g, Camera& c, BaseState& gs)
{
	if (!picked_up) {
		float sin_val = std::sin((gs.timer - time_offset) * 2.f);
		float width = r.w * (std::pow(std::abs(sin_val), 0.9f)) * sign(sin_val);
		g.d.draw_image(tex, r.x + r.w / 2, r.y + r.h / 2, width, r.h, 0.f);

		r.draw(g.d, { 1.f,0.f,0.f,0.4f });
	} 
	else // spin away animation
	{
		float sin_val = std::sin(gs.timer * 20.f);
		float width = r.w * (std::pow(std::abs(sin_val), 0.9f)) * sign(sin_val);
		float height = r.h;
		float dec = (1.f + 50.f * time_offset * time_offset * time_offset );
		width /= dec;
		height /= dec;
		
		g.d.draw_image(tex, r.x + r.w / 2, r.y + r.h / 2, width, height, 0.f);

		r.draw(g.d, { 1.f,0.f,0.f,0.4f });

		time_offset += g.l.dt;
	}
}

void Coin::got_picked_up()
{
	if (!picked_up) {
		picked_up = true;
		time_offset = 0.f;
	}
}
