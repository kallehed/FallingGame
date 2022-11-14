#include "Player.h"

#include "Game.h"
#include <algorithm>
#include <cmath>

Player::Player() {}

void Player::logic(Game& g)
{
	const float change = g.l.dt * 20.f;
	if (g.l.key_down(SDL_SCANCODE_D)) { x_vel += change; }
	if (g.l.key_down(SDL_SCANCODE_A)) { x_vel -= change; }

	{
		float dec = g.l.dt * 8.f * x_vel;
		if (std::abs(x_vel) - std::abs(dec) > 0.f) {
			x_vel -= dec;
		}

		x_vel = std::clamp(x_vel, -MAX_X_VEL, MAX_X_VEL);

		h.x += x_vel * g.l.dt;
	}

	{
		float y_dec = g.l.dt * (y_vel < 0.f ? (1.f / (1.f + std::pow(std::abs(y_vel), 1.5f))) : 1.f);
		float new_y_vel = y_vel - y_dec;
		y_vel = std::clamp(new_y_vel, -MAX_Y_VEL, MAX_Y_VEL);
		prev_y = h.y;
		h.y += y_vel * g.l.dt;
	}

	// limit x to borders of screen
	h.x = std::clamp(h.x, -g.l.WIDTH, g.l.WIDTH - h.w);
}

void Player::draw(Game& g)
{
	g.d.draw_image(h.x - 0.14f, h.y, 0.4f, 0.4f);
	h.draw(g.d, { 1.f, 0.f, 0.1f, 0.5f });
}

void Player::collided(Game& g)
{
	//r.y += 2.0f * g.l.dt;
	y_vel *= -0.99f;
}
