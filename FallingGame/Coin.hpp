#pragma once

#include "misc.hpp"

class Coin
{
public:
	Coin(Game& g, float start_y);
	void draw(Game& g, Camera& c, float timer);
	void got_picked_up();
	Rect _r;
	float time_offset;
	bool picked_up = false;
};

