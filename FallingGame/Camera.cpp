#include "Camera.h"

#include "Game.h"

void Camera::last_in_logic(Game& g)
{
	y = g.p.r.y - g.l.HEIGHT*0.45f;
}

Pos Camera::offset()
{
	return Pos(-x, -y);
}
