#include "Game.h"

#include <algorithm>
#include <iostream>

Game::Game() : l(), d(l), p()
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
						p.y_vel = std::clamp(-0.5f*p.y_vel, 1.1f, 3.f);
					}
				}
			}
		}

		// game logic without place yet
		if (p.h.y - l.HEIGHT*2.f <= next_bouncer_y) {
			bouncers.emplace_back(l.WIDTH, next_bouncer_y);
			next_bouncer_y -= 2.f * l.HEIGHT * (0.3f + 0.5f*rand_01()) ;
		}
		c.last_in_logic(*this);

		// Drawing
		d.before_draw(*this);
		p.draw(*this);
		for (auto& e : bouncers) {
			e.h.draw(d, {1.f,1.f,0.f,1.f});
		}
		//d.draw_image(0, 0, 0, 0);
		//stuff
		l.end_frame();
	}
}
