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
		p.logic(*this);
		
		for (auto& e : bouncers) {
			e.logic(*this);
		}

		// collision between bouncers and player
		{
			for (auto& e : bouncers) {
				if (p.r.intersect(e.r)) {
					if (p.y_vel < -0.9f) {
						std::cout << "bounce << " << p.y_vel << "\n";
						//p.collided(*this);
						//p.y_vel = std::max(0.5f, p.y_vel * 0.5f);
						p.y_vel = std::clamp(-0.5f*p.y_vel, 1.1f, 3.f);
						
					}
				}
			}
		}

		// game logic without place yet
		if (p.r.y - l.HEIGHT*2.f <= next_bouncer_y) {
			bouncers.emplace_back((rand_01()-0.5f )*2.f*(l.WIDTH - 0.4f), next_bouncer_y);
			next_bouncer_y -= 2.f * l.HEIGHT * (0.3f + 1.f*rand_01());
		}
		c.last_in_logic(*this);

		d.before_draw(*this);
		p.draw(*this);
		for (auto& e : bouncers) {
			Rect new_r = e.r;
			new_r.draw(d, {1.f,1.f,0.f,1.f});
		}

		//stuff
		l.end_frame();
	}
}
