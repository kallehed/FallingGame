#pragma once

#include "misc.hpp"

class Game;

class Player
{
public:
	void init(); // reset ALL of player
	void logic(GameEvents ge, float dt);
	void draw(Game& g, Camera& c);

	void get_feather(int feathers_received);
	
	// returns from 0.f to 1.f how powerup is filled
	float powerup_filled();

	// whether to draw the player in front or not
	bool draw_in_front();

	Rect _r; // for hitbox
	float _prev_y;
	float _y_vel;
	float _x_vel;
	float _bounce_x_vel;
	float _rotation;
	float _time_since_coin;
	int _coins;
	int _feathers;
	int _feathers_needed_for_powerup;
	int _max_feathers;

	// PUBLIC to read. Says whether the powerup is active or not.
	bool _powerup_active;
	float _powerup_timer;

	static constexpr float WIDTH = 0.23f;
	static constexpr float HEIGHT = WIDTH * 1.16386555f;
private:
	// for rendering?
	
	static constexpr float MAX_Y_VEL = 7.f;
	static constexpr float MAX_X_VEL = 3.f;
};

