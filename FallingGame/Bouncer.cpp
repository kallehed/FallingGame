#include "Bouncer.h"

#include "Game.h"

Bouncer::Bouncer(float spawn_width, float y) : h(0.f,0.f,0.f)
{
	h.w = 0.3f + 0.1f*rand_uni();
	h.x = (2.f * spawn_width + h.w) * rand_01() - (spawn_width + h.w);
	h.y = y;
}

void Bouncer::logic(Game& g)
{
}

void Bouncer::draw(Game& g)
{
	if (h.x < 0.f || true) { // stem
		g.d.draw_image(g.c, g.d.mushroom_stem_texture, h.x - 0.15f + h.w/2.f, h.y - 0.23f, 0.4f, 0.4f, 0.f);
	}

	// cap
	g.d.draw_image(g.c, g.d.mushroom_cap_texture, h.x + h.w / 2.f, h.y - 0.05f, h.w + 0.05f, 0.15f, 0.f);
	
	h.draw(g.d, { 1.f,1.f,0.f,1.0f });
}