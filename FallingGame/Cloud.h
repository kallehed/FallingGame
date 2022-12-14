#pragma once

#include "misc.h"

class Cloud
{
public:
	void construct(Game& g, float depth);
	void logic(Game& g);
	void draw(Game& g);
	// z between 0.f and 1.f
	float x, y, z;
	float w, h;
	float x_vel;
	TEX::_ tex;

protected:
	void init(Game& g); // called when cloud should reposition, change image
};

