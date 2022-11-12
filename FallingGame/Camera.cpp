#include "Camera.h"

#include "Game.h"

void Camera::last_in_logic(Game& g)
{
	y = g.p.r.y + 0*g.l.HEIGHT;
}

Pos Camera::offset()
{
	return Pos(-x, -y);
}
