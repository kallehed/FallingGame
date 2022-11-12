#include "Player.h"

#include "Game.h"

Player::Player() {}

void Player::logic(Game& g)
{
	const float change = g.l.dt * 3.f;
	if (g.l.key_down(SDL_SCANCODE_D)) { r.x += change; }
	if (g.l.key_down(SDL_SCANCODE_A)) { r.x -= change; }

	y_vel -= 1.0f * g.l.dt;
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
