#include "Game.h"

#include <algorithm>
#include <iostream>

Game::Game() : l(), d(*this), p()
{
	for (int i = 0; i < clouds.size(); ++i) {
		clouds[i].construct(*this, (float)((int)clouds.size() - i) / (float)clouds.size());
	}
}

void Game::start()
{
	bool quit = false;

	float next_bouncer_y = -1.f;

	int total_clouds = 20;
	for (int i = total_clouds; i > 0; --i) {
		//clouds.emplace_back(*this, (float)i / (float)total_clouds);
	}

	while (!l.start_frame() && !quit)
	{
		// logic
		p.logic(*this);
		
		for (auto& e : bouncers) {
			e.logic(*this);
		}
		for (auto& e : clouds) { e.logic(*this); }

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

		d.draw_sky(p);
		for (auto& e : clouds) { e.draw(*this); }

		p.draw(*this);
		for (auto& e : bouncers) {
			e.draw(*this);
		}
		// side background
		d.draw_sides(p);
		
		//d.draw_image(0, 0, 0, 0);
		//stuff
		l.end_frame();
	}
}
