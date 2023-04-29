#pragma once

#include "misc.hpp"

class Bouncer
{
public:
	enum class Type : signed char {
		Normal,
		Moves,
	};
	void init(float spawn_width, float y, Type type);

	void draw(Game& g, Camera& c);

	void bounced_on(float moment);

	HoLine h;
	float bounce_timer = 0.f;
	Type _type;

};

