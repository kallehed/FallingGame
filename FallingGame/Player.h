#pragma once

#include "misc.h"

class Game;

class Player
{
public:
	void init(); // reset ALL of player
	void logic(Game& g);
	void draw(Game& g, Camera& c);

	Rect r; // for hitbox
	float prev_y;
	float y_vel;
	float x_vel;
	float bounce_x_vel;
	float rotation;
	float time_since_coin;
	int coins;

	constexpr static float WIDTH = 0.23f;
	constexpr static float HEIGHT = WIDTH * 1.16386555f;
private:
	// for rendering?
	
	constexpr static float MAX_Y_VEL = 7.f;
	constexpr static float MAX_X_VEL = 3.f;
};

