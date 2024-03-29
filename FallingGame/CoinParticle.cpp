#include "CoinParticle.hpp"

#include "Game.hpp"

CoinParticle::CoinParticle(float x, float y, float time, float _x_vel, float _y_vel)
	: start_x(x), start_y(y), start_time(time), _x_vel(_x_vel), _y_vel(_y_vel)
{
}

void CoinParticle::draw(Game& g)
{
	g.d.draw_coin_particle(*this);
}
