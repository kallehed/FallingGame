#include "Cloud.hpp"

#include "Game.hpp"

void Cloud::construct(Drawer& d, float depth)
{
	z = 0.7f * depth + 0.1f;
	init(d);

	// this places the cloud in a random screen position, while init() will place the cloud at the edges. 
	x = rand_uni() * Layer::WIDTH;
	y = rand_uni() * Layer::HEIGHT;
}

void Cloud::init(Drawer& d)
{
	//tex = (TEX::_)(TEX::cloud_1 + (int)(3.99f * rand_01()));
	tex = (TEX::_)(TEX::cloud_1 + rand_int(0, 4));

	constexpr float scale = 0.002f;
	w = scale * d.tex_sizes[TEX::cloud_1][0] * (1.f-z);
	h = scale * d.tex_sizes[TEX::cloud_1][1] * (1-z);

	_x_vel = 0.1f;//rand_uni();

	y = ((rand_01() * 4.f) - 6.f) * Layer::HEIGHT;
	if (y + h < -Layer::HEIGHT) {
		x = rand_uni() * Layer::WIDTH;
	}
	else {
		x = (rand_uni() > 0.f) ? Layer::WIDTH : -Layer::WIDTH;
	}
	
}

