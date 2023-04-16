#pragma once

#include "misc.hpp"

class Cloud
{
public:
	// constructs from depth, so to get right size
	void construct(Drawer& g, float depth);
	// z between 0.f and 1.f
	float x, y, z;
	float w, h;
	float x_vel;
	TEX::_ tex;

	// called when cloud should reposition, change image
	void init(Drawer& g); 
};

