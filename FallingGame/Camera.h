#pragma once

#include "misc.h"

class Camera
{
public:
	void last_in_logic(Game& g); // set offset for this frame
	Pos offset() const; // get offset to set uniform to
	float x = 0.f, y = 0.f;
};

