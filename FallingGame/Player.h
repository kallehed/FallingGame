#pragma once

#include "misc.h"

class Game;

class Player
{
public:
	Player();
	void logic(Game& g);
	void draw(Game& g);

	void collided(Game& g);

	HoLine r{ 0.0f, 0.0f, 0.1f};
	float prev_y = 0.f;
	float y_vel = 0.f;
	float x_vel = 0.f;
private:
	constexpr static float MAX_Y_VEL = 7.f;
	constexpr static float MAX_X_VEL = 3.f;
};

