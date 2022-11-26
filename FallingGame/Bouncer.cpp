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
	{
		constexpr float scale = 0.0008f;
		const float width = scale * g.d.tex_sizes[TEX::mushroom_stem][0];
	    const float height = scale * g.d.tex_sizes[TEX::mushroom_stem][1];
		if (h.x < 0.f) { // stem
			g.d.draw_image(g.c, TEX::mushroom_stem, h.x - 1.2f + h.w / 2.f, h.y - 0.23f, width, height, 0.f);
		}
		else {
			g.d.draw_image(g.c, TEX::mushroom_stem, h.x + 1.2f + h.w / 2.f, h.y - 0.23f, -width, height, 0.f);
		}
	}

	// cap
	g.d.draw_image(g.c, TEX::mushroom_cap, h.x + h.w / 2.f, h.y - 0.05f, h.w + 0.05f, 0.15f, 0.f);
	
	h.draw(g.d, { 1.f,1.f,0.f,1.0f });
}