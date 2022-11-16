#pragma once

#include "misc.h"

class Bouncer
{
public:
	Bouncer(float WIDTH, float y);
	
	void logic(Game& g);

	void draw(Game& g);

	HoLine h;
private:
};

