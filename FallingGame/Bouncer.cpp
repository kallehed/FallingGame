#include "Bouncer.h"

#include "Game.h"

Bouncer::Bouncer(float WIDTH, float y) : h((2.f*WIDTH - 0.4f) * rand_01() - WIDTH, y - 0.4f, 0.4f)
{
}

void Bouncer::logic(Game& g)
{
}

void Bouncer::draw(Game& g)
{
	h.draw(g.d, { 1.f,1.f,0.f,0.5f });

	if (h.x < 0.f || true) {
		g.d.draw_image(g.c, g.d.mushroom_stem_texture, h.x, h.y - 0.3f, 0.5f, 0.5f, 0.f);
	}

	g.d.draw_image(g.c, g.d.mushroom_cap_texture, h.x + h.w / 2.f, h.y - 0.05f, h.w + 0.05f, 0.15f, 0.f);
	
	
}