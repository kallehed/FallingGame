#include "Camera.h"

#include "Game.h"

void Camera::last_in_logic(Game& g)
{
	y = g.p.h.y - g.l.HEIGHT*0.55f;
}

Pos Camera::offset()
{
	return Pos(-x, -y);
}
