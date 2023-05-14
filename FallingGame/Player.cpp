#include "Player.hpp"

#include "Game.hpp"
#include <algorithm>
#include <cmath>
#include "misc.hpp"

void Player::init()
{
	_r.init(0.0f, 0.0f, 0.08f, 0.166f);
	_prev_y = 0.f;
	_y_vel = 0.f;
	_x_vel = 0.f;
	_bounce_x_vel = 0.f;
	_rotation = 0.f;
	_time_since_coin = pow(10.f, 10.f);
	_coins = 0;
	_feathers = 0;
	_feathers_needed_for_powerup = 4;
	_max_feathers = 4;

	_powerup_active = false;
}

void Player::logic(GameEvents ge, float dt)
{
	// x movement
	{
		const float change = dt * 20.f;
		if (ge.player_to_right) { _x_vel += change; }
		if (ge.player_to_left) { _x_vel -= change; }

		float x_dec = dt * 8.f * _x_vel; // decrease in x_vel
		_x_vel = decr_abs_val(_x_vel, std::abs(x_dec));

		// bounce_x_vel
		{
			float bounce_dec = dt * 2.f;
			_bounce_x_vel = decr_abs_val(_bounce_x_vel, std::abs(bounce_dec));
		}

		_x_vel = std::clamp(_x_vel, -MAX_X_VEL, MAX_X_VEL);

		_r.x += (_x_vel + _bounce_x_vel) * dt;

		// limit x to borders of screen
		_r.x = std::clamp(_r.x, -Game::G_WIDTH, Game::G_WIDTH - _r.w);
	}

	// y movement
	{
		float y_dec = dt * ((_y_vel < 0.f) ? (1.f / (1.f + std::pow(std::abs(_y_vel), 3.f))) : 1.f);
		y_dec = std::fmaxf(y_dec, 0.001f); // limit too low increase of y vel
		float new_y_vel = _y_vel - y_dec;
		_y_vel = std::clamp(new_y_vel, -MAX_Y_VEL, MAX_Y_VEL); // probably never reached
		_prev_y = _r.y;
		_r.y += _y_vel * dt;
	}

	// rotation
	{
		// x vel rotation: HORIZONTAL MOVEMENT (SMALL)
		_rotation -= dt * (_x_vel / 1.f);

		if (_y_vel > 0.f) { // Rotation when bouncing upwards
			_rotation += (_y_vel / 0.15f) * dt;
		}
		else {
			_rotation = decr_abs_val(_rotation, std::abs(_rotation) * 5.0f * dt);
		}

		while (_rotation > pi) {
			_rotation -= tau;
		}
	}

	// animation details: time since coin,
	{
		_time_since_coin += dt;
	}

	// activate powerup
	{
		if (!_powerup_active) {
			if (ge.player_activate_special) {
				if (_feathers >= _feathers_needed_for_powerup) {
					_powerup_active = true;
					_feathers -= _feathers_needed_for_powerup;
				}
			}
		}
	}
}

void Player::draw(Game& g, Camera& c)
{
	// width "3D" rotation at bounce
	float w = WIDTH - std::max(0.0f, std::min(2.f*WIDTH, 1.f * _y_vel));
	// smallification when falling down ONLY
	TEX::_ texture = TEX::bird_closed;
	if (_y_vel >= -2.f) {
		w /= std::max(1.f, -((_y_vel - 10.f) / 10.f));
		texture = TEX::bird;
	}
	//g.d.draw_image(g.c, texture, r.x + 0.045f, r.y + 0.135f, w, WIDTH * 1.16386555f, rotation);
	_r.draw(g.d, { 1.f, 0.f, 0.1f, 0.4f });
	g.d.draw_bird(c, texture, _r.x + 0.045f, _r.y + 0.135f, _rotation, w, HEIGHT, _time_since_coin);
}

void Player::get_feather(int feathers_received)
{
	_feathers += feathers_received;
	_feathers = std::min(_feathers, _max_feathers);
}

float Player::powerup_filled() {
	return _feathers / (float)_max_feathers;
}