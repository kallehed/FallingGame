#include "Bouncer.h"

#include "Game.h"

#include <cmath>

void Bouncer::init(float spawn_width, float y)
{
	h.w = 0.3f + 0.1f*rand_uni();
	h.x = (2.f * spawn_width + h.w) * rand_01() - (spawn_width + h.w);
	h.y = y;
}


void Bouncer::draw(Game& g, Camera& c)
{
	{
		constexpr float scale = 0.0008f;
		float width = scale * g.d.tex_sizes[TEX::mushroom_stem][0];
	    const float height = scale * g.d.tex_sizes[TEX::mushroom_stem][1];
		float x, rot = std::abs(bounce_timer)/20.f;
		if (h.x < 0.f) { // stem
			x = h.x - 1.2f + h.w / 2.f;
		}
		else {
			x = h.x + 1.2f + h.w / 2.f;
			width *= -1.f;
			rot *= -1.f;
		}
		
		g.d.draw_image(TEX::mushroom_stem, x, h.y - 0.23f, width, height, rot);
	}

	// cap
	g.d.draw_image(TEX::mushroom_cap, h.x + h.w / 2.f, h.y - 0.05f, h.w + 0.05f, 0.15f, bounce_timer);
	
	bounce_timer = decr_abs_val(bounce_timer, 0.5f*g.l.dt * (1.f + 5.f*std::abs(bounce_timer)));

	h.draw(g.d, { 1.f,1.f,0.f,1.0f });
}

void Bouncer::bounced_on(float moment)
{
	bounce_timer = moment * 0.75f;
}
