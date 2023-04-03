#include "Camera.h"

#include "Game.h"

void Camera::init()
{
	x = 0.f;
	y = 0.f;
	prev_y = 0.f;
	y_dif = 0.f;

	player_screen_top_offset = -Layer::HEIGHT * 0.665f;
}

void Camera::set_in_game(Player& p, const float cam_bound)
{

	prev_y = y;
	y = p.r.y + player_screen_top_offset; // set camera position
	y = std::max(y, cam_bound);

	// prevent jump if camera travels too far in frame (happens at startup of game from menu)
	y_dif = prev_y - y;
	if (std::abs(y_dif) > 0.5f) y_dif = 0.f;
}
