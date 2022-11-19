#include "Game.h"

#include <algorithm>
#include <iostream>

Game::Game() : l(), d(*this), p()
{
}

void Game::start()
{
	bool quit = false;

	float next_bouncer_y = -1.f;

	while (!l.start_frame() && !quit)
	{
		// logic
		p.logic(*this);
		
		for (auto& e : bouncers) {
			e.logic(*this);
		}

		// collision between bouncers and player
		{
			for (auto& e : bouncers) {
				if (e.h.y < p.prev_y && e.h.y > p.h.y && p.h.x < e.h.x+e.h.w && p.h.x + p.h.w > e.h.x) {
					if (p.y_vel < 0.0f) {
						std::cout << "bounce << " << p.y_vel << "\n";
						p.bounce_x_vel = std::min( 1.f,-0.5f*p.y_vel) * 10.f * p.h.x_dist(e.h);
						p.y_vel = std::clamp(-0.5f*p.y_vel, 0.75f, 1.5f);
						
					}
				}
			}
		}

		// game logic without place yet
		if (p.h.y - l.HEIGHT*2.f <= next_bouncer_y) {
			bouncers.emplace_back(G_WIDTH, next_bouncer_y);
			next_bouncer_y -= 2.f * l.HEIGHT * (0.3f + 0.5f*rand_01()) ;
		}
		c.last_in_logic(*this);

		// Drawing
		d.before_draw(*this);

		// sky
		{
			d.draw_sky(p);
			//d.draw_image(c, d.sky_texture, c.x, c.y, (G_WIDTH*1.5f) * 2.f, G_HEIGHT * 2.f, 0.f);
		}

		p.draw(*this);
		for (auto& e : bouncers) {
			e.draw(*this);
		}
		// side background
		{
			/*float w = l.WIDTH - G_WIDTH;
			float h = l.HEIGHT * 2.f;
			// left side
			d.draw_image(c, d.side_background_texture, c.x - G_WIDTH - w/2.f, c.y, w, h, 0.f);
			// right side
			d.draw_image(c, d.side_background_texture, c.x + G_WIDTH + w / 2.f, c.y, -w, h, 0.f);
			*/
			d.draw_sides(p);
		}
		
		//d.draw_image(0, 0, 0, 0);
		//stuff
		l.end_frame();
	}
}
