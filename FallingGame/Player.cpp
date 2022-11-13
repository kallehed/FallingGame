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

	float dec = g.l.dt * 8.f * x_vel;
	if (std::abs(x_vel) - std::abs(dec) > 0.f) {
		x_vel -= dec;
	}

	x_vel = std::clamp(x_vel, -MAX_X_VEL, MAX_X_VEL);

	r.x += x_vel * g.l.dt;

	float new_y_vel = y_vel - ((g.l.dt) / ( 1.f + std::pow(std::abs(y_vel), 1.5f)));
	y_vel = std::clamp( new_y_vel, -MAX_Y_VEL, MAX_Y_VEL);
	prev_y = r.y;
	r.y += y_vel * g.l.dt;

	// limit x to borders of screen
	r.x = std::clamp(r.x, -g.l.WIDTH, g.l.WIDTH - r.w);
}

void Player::draw(Game& g)
{
	r.draw(g.d, { 1.f, 0.f, 0.1f, 1.f });
}

void Player::collided(Game& g)
{
	//r.y += 2.0f * g.l.dt;
	y_vel *= -0.99f;
}
