#pragma once

#include "misc.h"

class Bouncer
{
public:
	Bouncer(float spawn_width, float y);
	
	void logic(Game& g);

	void draw(Game& g);

	HoLine h;
private:
};

