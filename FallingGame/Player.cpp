#include "Player.h"

#include "Game.h"
#include <algorithm>
#include <cmath>

Player::Player() {}

void Player::logic(Game& g)
{
	// x movement
	{
		const float change = g.l.dt * 20.f;
		if (g.l.key_down(SDL_SCANCODE_D)) { x_vel += change; }
		if (g.l.key_down(SDL_SCANCODE_A)) { x_vel -= change; }

		float x_dec = g.l.dt * 8.f * x_vel; // decrease in x_vel
		x_vel = decr_abs_val(x_vel, std::abs(x_dec));

		// bounce_x_vel
		{
			float bounce_dec = g.l.dt * 2.f;
			bounce_x_vel = decr_abs_val(bounce_x_vel, std::abs(bounce_dec));
		}

		x_vel = std::clamp(x_vel, -MAX_X_VEL, MAX_X_VEL);

		h.x += (x_vel + bounce_x_vel) * g.l.dt;

		// limit x to borders of screen
		h.x = std::clamp(h.x, -g.G_WIDTH, g.G_WIDTH - h.w);
	}

	// y movement
	{
		float y_dec = g.l.dt * (y_vel < 0.f ? (1.f / (1.f + std::pow(std::abs(y_vel), 3.f))) : 1.f);
		float new_y_vel = y_vel - y_dec;
		y_vel = std::clamp(new_y_vel, -MAX_Y_VEL, MAX_Y_VEL);
		prev_y = h.y;
		h.y += y_vel * g.l.dt;
	}

	// rotation
	{
		// x vel rotation: HORIZONTAL MOVEMENT (SMALL)
		rotation -= g.l.dt * (x_vel / 1.f);

		if (y_vel > 0.f) { // Rotation when bouncing upwards
			rotation += (y_vel / 0.15f) * g.l.dt;
		}
		else {
			rotation = decr_abs_val(rotation, std::abs(rotation) * 5.0f * g.l.dt);
		}

		while (rotation > pi) {
			rotation -= tau;
		}
	}
}

void Player::draw(Game& g)
{
	// width "3D" rotation at bounce
	float w = WIDTH - std::max(0.0f, std::min(2.f*WIDTH, 1.f * y_vel));
	// smallification when falling down ONLY
	unsigned int texture = g.d.bird_closed_texture;
	if (y_vel >= -2.f) {
		w /= std::max(1.f, -((y_vel - 10.f) / 10.f));
		texture = g.d.bird_texture;
	}
	g.d.draw_image(g.c, texture, h.x + 0.06f, h.y + 0.2f, w, WIDTH* 1.16386555, rotation);
	h.draw(g.d, { 1.f, 0.f, 0.1f, 1.0f });
}
