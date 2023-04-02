#include "Camera.h"

#include "Game.h"

void Camera::set_in_game(Game& g)
{
	static constexpr float CAM_BOUND = -6.f;

	prev_y = y;
	y = g.p.r.y - g.l.HEIGHT * 0.665f; // set camera position
	y = std::max(y, CAM_BOUND);

	y_dif = prev_y - y;
	if (std::abs(y_dif) > 0.5f) y_dif = 0.f;
}
