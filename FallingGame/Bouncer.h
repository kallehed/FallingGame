#pragma once

#include "misc.h"

class Bouncer
{
public:
	Bouncer(float x, float y);
	
	void logic(Game& g);

	Rect r;
private:
};
