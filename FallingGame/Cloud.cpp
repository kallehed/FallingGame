#include "Cloud.h"

#include "Game.h"

void Cloud::construct(Game& g, float depth)
{
	z = 0.7f * depth + 0.1f;
	init(g);

	// this places the cloud in a random screen position, while init() will place the cloud at the edges. 
	x = rand_uni() * Layer::WIDTH;
	y = rand_uni() * Layer::HEIGHT;
}

void Cloud::logic(Game& g)
{
	if ((x + w) < -Layer::WIDTH || (x - w) > Layer::WIDTH  || (y - h) > Layer::HEIGHT) {
		init(g);
	}
	y += (g.p.prev_y - g.p.r.y) * (1.f - z);
	x += x_vel * g.l.dt * (1.f - z);
}

void Cloud::menu_logic(Game& g)
{
	if ((x + w) < -Layer::WIDTH || (x - w) > Layer::WIDTH || (y - h) > Layer::HEIGHT) {
		tex = (TEX::_)(TEX::cloud_1 + rand_int(0, 4));

		constexpr float scale = 0.002f;
		//w = scale * g.d.tex_sizes[TEX::cloud_1][0] * (1.f - z);
		//h = scale * g.d.tex_sizes[TEX::cloud_1][1] * (1 - z);

		//x_vel = 0.1f;//rand_uni();

		//y = ((rand_01() * 4.f) - 6.f) * Layer::HEIGHT;
		//if (y + h < -Layer::HEIGHT) {
		//	x = rand_uni() * Layer::WIDTH;
		//}
		//else {
		//	x = (rand_uni() > 0.f) ? Layer::WIDTH : -Layer::WIDTH;
		//}
		x_vel *= -1.f;
	}
	//y += (g.p.prev_y - g.p.r.y) * (1.f - z);
	x += x_vel * g.l.dt * (1.f - z);
}

void Cloud::init(Game& g)
{
	//tex = (TEX::_)(TEX::cloud_1 + (int)(3.99f * rand_01()));
	tex = (TEX::_)(TEX::cloud_1 + rand_int(0, 4));

	constexpr float scale = 0.002f;
	w = scale * g.d.tex_sizes[TEX::cloud_1][0] * (1.f-z);
	h = scale * g.d.tex_sizes[TEX::cloud_1][1] * (1-z);

	x_vel = 0.1f;//rand_uni();

	y = ((rand_01() * 4.f) - 6.f) * Layer::HEIGHT;
	if (y + h < -Layer::HEIGHT) {
		x = rand_uni() * Layer::WIDTH;
	}
	else {
		x = (rand_uni() > 0.f) ? Layer::WIDTH : -Layer::WIDTH;
	}
	
}

