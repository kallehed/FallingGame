#pragma once

#include "misc.h"

class Coin
{
public:
	Coin(Game& g, float start_y);
	void logic(Game& g);
	void draw(Game& g, Camera& c, BaseState& gs);
	void got_picked_up(Game& g);
	Rect r;
	float time_offset;
	bool picked_up = false;
};

