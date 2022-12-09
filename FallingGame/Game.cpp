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
	float next_coin_y = -1.5f;

	while (!l.start_frame() && !quit)
	{
		// logic
		death_y -= 0.9f * l.dt * std::max(1.f, 0.2f*std::pow(death_y-p.r.y, 2.f) );
		p.logic(*this);
		
		for (auto& e : bouncers) {
			e.logic(*this);
		}
		for (int i = (int)bouncers.size() - 1; i >= 0; --i) { // delete bouncers that have gone too high
			if (bouncers[i].h.y  - 1.f * l.HEIGHT > p.r.y) { // should be removed
				bouncers.erase(bouncers.begin() + i);
			}
		}
		for (auto & e : coins) { e.logic(*this); }
		for (int i = (int)coins.size() - 1; i >= 0; --i) { // delete bouncers that have gone too high
			if (coins[i].r.y - l.HEIGHT > p.r.y) { // should be removed
				coins.erase(coins.begin() + i);
			}
		}
		for (auto& e : clouds) { e.logic(*this); }

		// collision between bouncers and player
		{
			for (auto& e : bouncers) {
				if (e.h.y < p.prev_y && e.h.y > p.r.y && p.r.x < e.h.x+e.h.w && p.r.x + p.r.w > e.h.x) {
					if (p.y_vel < 0.0f) {
						std::cout << "bounce << " << p.y_vel << "\n";
						p.bounce_x_vel = std::min( 1.f,-0.5f*p.y_vel) * 10.f * p.r.x_dist(e.h);
						p.y_vel = std::clamp(-0.5f*p.y_vel, 0.75f, 1.5f);
						e.bounced_on(p.bounce_x_vel);
						
					}
				}
			}
		}
		// collision between coins and player
		{
			for (int i = (int)coins.size() - 1; i >= 0; --i) {
				auto& e = coins[i];
				if (p.r.intersect(e.r)) {
					std::cout << "COIN \n";
					//coins.erase(coins.begin() + i);
					e.got_picked_up(*this);
				}
			}
		}

		// game logic without place yet
		if (p.r.y - l.HEIGHT*2.f <= next_bouncer_y) {
			bouncers.emplace_back(G_WIDTH, next_bouncer_y);
			next_bouncer_y -= (2.f * l.HEIGHT * (0.3f + 0.5f*rand_01()));
		}

		while (p.r.y - l.HEIGHT*2.f <= next_coin_y)
		{
			coins.emplace_back(*this, next_coin_y);
			next_coin_y -= (2.f * l.HEIGHT);
		}

		c.last_in_logic(*this);

		// Drawing
		d.before_draw(*this);

		d.draw_sky(*this);

		for (auto& e : clouds) { e.draw(*this); }
		for (auto& e : coins) { e.draw(*this); }

		p.draw(*this);
		for (auto& e : bouncers) { e.draw(*this); }
		// side background
		d.draw_sides(p);
		Rect r{ -l.WIDTH, death_y, 2.f*l.WIDTH, 10.f };
		r.draw(d, { 0.f,0.f,0.f,0.5f });
		//d.draw_image(0, 0, 0, 0);
		//stuff
		l.end_frame();
	}
}
