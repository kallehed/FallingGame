#pragma once

#include "misc.h"

class Cloud
{
public:
	void logic(Game& g);
	void draw(Game& g);
	// z between 0.f and 1.f
	float x, y, z;
};

