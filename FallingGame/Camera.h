#pragma once

#include "misc.h"

class Camera
{
public:
	void set_in_game(Game& g);

	float x = 0.f;
	float y = 0.f;
private:
	float prev_y = 0.f;
public:
	float y_dif = 0.f;
};