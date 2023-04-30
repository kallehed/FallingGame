#pragma once

#include "misc.hpp"

class Bouncer
{
public:
	enum class Type : signed char {
		Normal,
		Moves,
	};
	void init(float spawn_width, float y, Type type, float init_time);

	void draw(Game& g, Camera& c);

	void bounced_on(float moment);

	HoLine h;
	// When was it inited
	float _init_time;
	float bounce_timer = 0.f;
	float _x_vel;
	Type _type;

};

