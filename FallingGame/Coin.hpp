#pragma once

#include "misc.hpp"

class Coin
{
public:
	Coin(Game& g, float start_y);
	void draw(Game& g, Camera& c, BaseState& gs);
	void got_picked_up();
	Rect r;
	float time_offset;
	bool picked_up = false;
};
