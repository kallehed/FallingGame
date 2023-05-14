#pragma once

#include "misc.hpp"

class CoinParticle
{
public:
	CoinParticle(float x, float y, float time, float _x_vel, float _y_vel);
	float start_x, start_y, start_time, _x_vel, _y_vel;
	void draw(Game& g);
};

