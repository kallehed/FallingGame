#include "Player.h"

#include "Game.h"
#include <algorithm>
#include <cmath>

Player::Player() {}

void Player::logic(Game& g)
{
	const float change = g.l.dt * 3.f;
	if (g.l.key_down(SDL_SCANCODE_D)) { r.x += change; }
	if (g.l.key_down(SDL_SCANCODE_A)) { r.x -= change; }

	float new_y_vel = y_vel - ((g.l.dt) / ( 1.f + std::pow(std::abs(y_vel), 1.5f)));
	y_vel = std::clamp( new_y_vel, -MAX_Y_VEL, MAX_Y_VEL);
	r.y += y_vel * g.l.dt;
}

void Player::draw(Game& g)
{
	g.d.draw_rectangle(r.x, r.y, r.w, r.h, { 1.f,0.f,0.1f,1.f });
}

void Player::collided(Game& g)
{
	//r.y += 2.0f * g.l.dt;
	y_vel *= -0.99f;
}
