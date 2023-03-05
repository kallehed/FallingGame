#include "Game.h"

#include <algorithm>
#include <iostream>

Game::Game() : l(), d(*this), p()
{
	srand(336);
	for (int i = 0; i < m_clouds.size(); ++i) {
		m_clouds[i].construct(*this, (float)((int)m_clouds.size() - i) / (float)m_clouds.size());
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
		m_timer += l.dt;

		// increase the more far away the player is from the barrier
		//death_y -= 1.4f * l.dt * std::max(1.f, 0.05f*std::pow(death_y-p.r.y, 2.f) );

		// increase speed of death_y over time
		m_death_y -= 1.4f * l.dt * std::max(1.f, std::log(m_timer/10.f));
		p.logic(*this);
		
		for (auto& e : m_bouncers) {
			e.logic(*this);
		}
		for (int i = (int)m_bouncers.size() - 1; i >= 0; --i) { // delete bouncers that have gone too high
			if (m_bouncers[i].h.y  - 1.f * l.HEIGHT > p.r.y) { // should be removed
				m_bouncers.erase(m_bouncers.begin() + i);
			}
		}
		for (auto & e : m_coins) { e.logic(*this); }
		for (int i = (int)m_coins.size() - 1; i >= 0; --i) { // delete coins that have gone too high
			if (m_coins[i].r.y - l.HEIGHT > p.r.y) { // should be removed
				m_coins.erase(m_coins.begin() + i);
			}
		}
		for (auto& e : m_clouds) { e.logic(*this); }

		// collision between bouncers and player
		{
			for (auto& e : m_bouncers) {
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

		// delete coin particles that are bad
		for (int i = (int)m_coin_particles.size() - 1; i >= 0; --i) {
			auto& e = m_coin_particles[i];
			if (m_timer - e.start_time > 4.f) {
				m_coin_particles.erase(m_coin_particles.begin() + i);
			}
		}

		// collision between coins and player
		{
			for (int i = (int)m_coins.size() - 1; i >= 0; --i) {
				auto& e = m_coins[i];
				if (p.r.intersect(e.r)) {
					std::cout << "COIN \n";
					if (!e.picked_up) {
						// don't spawn the particle
						//coin_particles.emplace_back(e.r.x + e.r.w/2.f, e.r.y + e.r.h/2.f, timer, p.x_vel * 1.2f, p.y_vel * 1.2f);
						// make bird shiny
						p.time_since_coin = 0.f;
					}
					//coins.erase(coins.begin() + i);
					e.got_picked_up(*this);
				}
			}
		}

		// game logic without place yet
		if (p.r.y - l.HEIGHT*2.f <= next_bouncer_y) {
			m_bouncers.emplace_back(G_WIDTH, next_bouncer_y);
			next_bouncer_y -= (2.f * l.HEIGHT * (0.3f + 0.5f*rand_01()));
		}

		while (p.r.y - l.HEIGHT*2.f <= next_coin_y)
		{
			m_coins.emplace_back(*this, next_coin_y);
			next_coin_y -= (2.f * l.HEIGHT);
		}

		c.last_in_logic(*this);

		// Drawing
		d.before_draw(*this);

		d.draw_sky(*this);

		//for (auto& e : m_clouds) { e.draw(*this); }
		this->d.draw_clouds(*this);
		for (auto& e : m_coins) { e.draw(*this); }
		for (auto& e : m_coin_particles) { e.draw(*this); }

		p.draw(*this);
		for (auto& e : m_bouncers) { e.draw(*this); }
		// side background
		d.draw_sides(p);

		// draw death storm at death_y
		{
			d.draw_image(c, TEX::storm, 0.f, m_death_y + l.HEIGHT, l.WIDTH * 2.f, l.HEIGHT * 2.f, 0.f);
		}
		//d.draw_image(0, 0, 0, 0);
		//stuff
		l.end_frame();
	}
}
