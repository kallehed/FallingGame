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

	Rect r{ 0.0f, 0.6f, 0.1f, 0.2f };
	float y_vel = 0.f;
private:
	constexpr static float MAX_Y_VEL = 7.f;
};

