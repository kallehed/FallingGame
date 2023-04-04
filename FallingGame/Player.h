#pragma once

#include "misc.h"

class Game;

class Player
{
public:
	void init(); // reset ALL of player
	void logic(GameEvents ge, float dt);
	void draw(Game& g, Camera& c);

	Rect r; // for hitbox
	float prev_y;
	float y_vel;
	float x_vel;
	float bounce_x_vel;
	float rotation;
	float time_since_coin;
	int coins;

	static constexpr float WIDTH = 0.23f;
	static constexpr float HEIGHT = WIDTH * 1.16386555f;
private:
	// for rendering?
	
	static constexpr float MAX_Y_VEL = 7.f;
	static constexpr float MAX_X_VEL = 3.f;
};

