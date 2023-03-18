#pragma once

#include "misc.h"

class Cloud
{
public:
	// constructs from depth, so to get right size
	void construct(Game& g, float depth);
	void logic(Game& g);
	// z between 0.f and 1.f
	float x, y, z;
	float w, h;
	float x_vel;
	TEX::_ tex;

protected:
	// called when cloud should reposition, change image
	void init(Game& g); 
};

