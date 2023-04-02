#pragma once

#include "misc.h"

class Camera
{
public:
	void init();
	void set_in_game(Player& p);

	float x;
	float y;
private:
	float prev_y;
public:
	float y_dif;
};