#include "Game.h"

Game::Game() : l(), d(l), p()
{
}

void Game::start()
{
	bool quit = false;

	float x = 0.f;
	float timer = 0.f;

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
					p.collided(*this);
				}
			}
		}

		// game logic without place yet
		timer += l.dt;
		if (timer >= 0.7f) {
			bouncers.emplace_back((rand_01()-0.5f )*2.f*l.WIDTH, -l.HEIGHT + p.r.y);
			timer = (timer - 0.7f);
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
