#pragma once

#include "misc.hpp"

class Bouncer
{
public:
	void init(float spawn_width, float y);

	void draw(Game& g, Camera& c);

	void bounced_on(float moment);

	HoLine h;
	float bounce_timer = 0.f;
};

