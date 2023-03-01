#pragma once

#include "misc.h"
#include <cmath>

class Game;

class Player
{
public:
	Player();
	void logic(Game& g);
	void draw(Game& g);

	Rect r{ 0.0f, 0.0f, 0.08f, 0.166f };
	float prev_y = 0.f;
	float y_vel = 0.f;
	float x_vel = 0.f;
	float bounce_x_vel = 0.f;
	float rotation = 0.f;
	float time_since_coin = pow(10.f,10.f);
private:
	constexpr static float WIDTH = 0.23f;
	constexpr static float MAX_Y_VEL = 7.f;
	constexpr static float MAX_X_VEL = 3.f;
};

