#include "Player.h"

#include "Game.h"
#include <algorithm>
#include <cmath>
#include "misc.h"

void Player::init()
{
	r = { 0.0f, 0.0f, 0.08f, 0.166f };
	prev_y = 0.f;
	y_vel = 0.f;
	x_vel = 0.f;
	bounce_x_vel = 0.f;
	rotation = 0.f;
	time_since_coin = pow(10.f, 10.f);
	coins = 0;
}

void Player::logic(GameEvents ge, float dt)
{
	// x movement
	{
		const float change = dt * 20.f;
		if (ge.player_to_right) { x_vel += change; }
		if (ge.player_to_left) { x_vel -= change; }

		float x_dec = dt * 8.f * x_vel; // decrease in x_vel
		x_vel = decr_abs_val(x_vel, std::abs(x_dec));

		// bounce_x_vel
		{
			float bounce_dec = dt * 2.f;
			bounce_x_vel = decr_abs_val(bounce_x_vel, std::abs(bounce_dec));
		}

		x_vel = std::clamp(x_vel, -MAX_X_VEL, MAX_X_VEL);

		r.x += (x_vel + bounce_x_vel) * dt;

		// limit x to borders of screen
		r.x = std::clamp(r.x, -Game::G_WIDTH, Game::G_WIDTH - r.w);
	}

	// y movement
	{
		float y_dec = dt * (y_vel < 0.f ? (1.f / (1.f + std::pow(std::abs(y_vel), 3.f))) : 1.f);
		float new_y_vel = y_vel - y_dec;
		y_vel = std::clamp(new_y_vel, -MAX_Y_VEL, MAX_Y_VEL);
		prev_y = r.y;
		r.y += y_vel * dt;
	}

	// rotation
	{
		// x vel rotation: HORIZONTAL MOVEMENT (SMALL)
		rotation -= dt * (x_vel / 1.f);

		if (y_vel > 0.f) { // Rotation when bouncing upwards
			rotation += (y_vel / 0.15f) * dt;
		}
		else {
			rotation = decr_abs_val(rotation, std::abs(rotation) * 5.0f * dt);
		}

		while (rotation > pi) {
			rotation -= tau;
		}
	}

	// animation details: time since coin,
	{
		time_since_coin += dt;
	}
}

void Player::draw(Game& g, Camera& c)
{
	// width "3D" rotation at bounce
	float w = WIDTH - std::max(0.0f, std::min(2.f*WIDTH, 1.f * y_vel));
	// smallification when falling down ONLY
	TEX::_ texture = TEX::bird_closed;
	if (y_vel >= -2.f) {
		w /= std::max(1.f, -((y_vel - 10.f) / 10.f));
		texture = TEX::bird;
	}
	//g.d.draw_image(g.c, texture, r.x + 0.045f, r.y + 0.135f, w, WIDTH * 1.16386555f, rotation);
	r.draw(g.d, { 1.f, 0.f, 0.1f, 0.4f });
	g.d.draw_bird(c, texture, r.x + 0.045f, r.y + 0.135f, rotation, w, HEIGHT, time_since_coin);
}
