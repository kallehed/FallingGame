#pragma once

#include "misc.h"

class Bouncer
{
public:
	Bouncer(float spawn_width, float y);
	
	void logic(Game& g);

	void draw(Game& g, Camera& c);

	void bounced_on(float moment);

	HoLine h;
	float bounce_timer = 0.f;
};

