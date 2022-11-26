#include "Coin.h"

#include "Game.h"

void Coin::draw(Game& g)
{
	g.d.draw_image(g.c, TEX::coin, x, y, w, h, 0.f);
}
