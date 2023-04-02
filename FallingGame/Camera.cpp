#include "Camera.h"

#include "Game.h"

void Camera::init()
{
	x = 0.f;
	y = 0.f;
	prev_y = 0.f;
	y_dif = 0.f;
}

void Camera::set_in_game(Player& p)
{
	static constexpr float CAM_BOUND = -6.f;

	prev_y = y;
	y = p.r.y - Layer::HEIGHT * 0.665f; // set camera position
	y = std::max(y, CAM_BOUND);

	y_dif = prev_y - y;
	if (std::abs(y_dif) > 0.5f) y_dif = 0.f;
}
