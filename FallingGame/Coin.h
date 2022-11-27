#pragma once

#include "misc.h"

class Coin
{
public:
	Coin(Game& g, float start_y);
	void logic(Game& g);
	void draw(Game& g);
	Rect r;
	float time_offset;
};

