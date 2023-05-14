#pragma once

#include "misc.hpp"

class Camera
{
public:
	void init();
	void set_in_game(Player& p, const float cam_bound);

	float x;
	float y;
	// where the player(bird) usually is offset from the top of the screen
	float player_screen_top_offset;
private:
	float _prev_y;
public:
	float y_dif;
};