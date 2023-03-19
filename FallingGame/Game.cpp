#include "Game.h"

#include <algorithm>
#include <iostream>

#include <cmath>

void Game::init()
{
	l.init();
	d.init(*this);

	srand(336); //TODO add true randomness
}

namespace GAME_STATE {
	enum _ : char {
		Menu,
		Game
	};
}

void Game::start()
{
	init();
	float next_bouncer_y = -1.f;
	float next_coin_y = -1.5f;

	GAME_STATE::_ game_state;
	// REALLY exit the program
	bool full_exit = false; 
	// you want to switch GAME_STATE
	bool go_to_new_state = false; 

	auto start_func = [this, &game_state, &go_to_new_state, &full_exit]()
	{
		m_timer += l.dt;
		if (this->l.start_frame()) {
			// close
			go_to_new_state = true;
			full_exit = true;
		}
	};
	auto end_func = [this]()
	{
		if constexpr (DEV_TOOLS) {
			d.draw_fps(l.dt);
		}
		//stuff
		l.end_frame();
	};

	auto new_game_session = [this, &go_to_new_state, &game_state]()
	{
		go_to_new_state = true;
		game_state = GAME_STATE::Game;

		m_death_y = 3.f;
		m_timer = 0.f;
		p.init();

		for (int i = 0; i < m_clouds.size(); ++i) {
			m_clouds[i].construct(*this, (float)((int)m_clouds.size() - i) / (float)m_clouds.size());
		}
	};

	auto new_menu_session = [this, &go_to_new_state, &game_state]()
	{
		m_timer = 0.f;
		//m_death_y = 3.f;
		//p.init();

		for (int i = 0; i < m_clouds.size(); ++i) {
			m_clouds[i].construct(*this, (float)((int)m_clouds.size() - i) / (float)m_clouds.size());
			m_clouds[i].x_vel += rand_uni();
		}

		go_to_new_state = true;
		game_state = GAME_STATE::Menu;
	};
	new_menu_session();
	m_death_y = 5.f;

	while (!full_exit) {
		go_to_new_state = false;
		switch (game_state) {
		case GAME_STATE::Menu:
			while (!go_to_new_state) {
				start_func();
				glClearColor(1.0, 1.0, 1.0, 1.0);
				glClear(GL_COLOR_BUFFER_BIT/* | GL_DEPTH_BUFFER_BIT */);
				
				//logic
				if (l.key_just_down(SDL_SCANCODE_O)) {
					new_game_session();
					break;
				}
				{
					float move = 1.1f * l.dt;
					p.r.y += move;
					m_death_y += move;
				}

				for (auto& e : m_clouds) { e.menu_logic(*this); }

				c.last_in_logic(*this);
				d.before_draw(*this);
				d.draw_sky(*this);
				d.draw_clouds(*this);
				d.draw_text("Flying Game!", { 1,0,1,1 }, 0, 0, 0.002f);
				//d.draw_rectangle(-Layer::WIDTH, -Layer::HEIGHT, Layer::WIDTH*2.f, Layer::HEIGHT*2.f, {1.f,1.f,1.f,1.f});
				end_func();
			}
			break;
		case GAME_STATE::Game:
			while (!go_to_new_state)
			{
				start_func();
				//glClearColor(1.0, 1.0, 1.0, 1.0);
				//glClear(GL_COLOR_BUFFER_BIT/* | GL_DEPTH_BUFFER_BIT */);
				// logic
				

				if (l.key_just_down(SDL_SCANCODE_P)) {
					new_menu_session();
					break;
				}

				// increase the more far away the player is from the barrier
				//death_y -= 1.4f * l.dt * std::max(1.f, 0.05f*std::pow(death_y-p.r.y, 2.f) );

				// increase speed of death_y over time
				m_death_y -= 1.01f * l.dt * std::max(1.f, std::log(m_timer / 10.f));
				p.logic(*this);

				for (auto& e : m_bouncers) {
					e.logic(*this);
				}
				for (int i = (int)m_bouncers.size() - 1; i >= 0; --i) { // delete bouncers that have gone too high
					if (m_bouncers[i].h.y - 1.f * l.HEIGHT > p.r.y) { // should be removed
						m_bouncers.erase(m_bouncers.begin() + i);
					}
				}
				for (auto& e : m_coins) { e.logic(*this); }
				for (int i = (int)m_coins.size() - 1; i >= 0; --i) { // delete coins that have gone too high
					if (m_coins[i].r.y - l.HEIGHT > p.r.y) { // should be removed
						m_coins.erase(m_coins.begin() + i);
					}
				}
				for (auto& e : m_clouds) { e.logic(*this); }

				// collision between bouncers and player
				{
					for (auto& e : m_bouncers) {
						if (e.h.y < p.prev_y && e.h.y > p.r.y && p.r.x < e.h.x + e.h.w && p.r.x + p.r.w > e.h.x) {
							if (p.y_vel < 0.0f) {
								std::cout << "bounce << " << p.y_vel << "\n";
								p.bounce_x_vel = std::min(1.f, -0.5f * p.y_vel) * 10.f * p.r.x_dist(e.h);
								p.y_vel = std::clamp(-0.5f * p.y_vel, 0.75f, 1.5f);
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
								++p.coins;
							}
							//coins.erase(coins.begin() + i);
							e.got_picked_up(*this);
						}
					}
				}

				// game logic without place yet
				if (p.r.y - l.HEIGHT * 2.f <= next_bouncer_y) {
					m_bouncers.emplace_back(G_WIDTH, next_bouncer_y);
					next_bouncer_y -= (2.f * l.HEIGHT * (0.3f + 0.5f * rand_01()));
				}

				while (p.r.y - l.HEIGHT * 2.f <= next_coin_y)
				{
					m_coins.emplace_back(*this, next_coin_y);
					next_coin_y -= (2.f * l.HEIGHT);
				}

				c.last_in_logic(*this);

				// Drawing
				d.before_draw(*this);

				d.draw_sky(*this);

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

				// draw coin counter
				{
					char buf[10];
					snprintf(buf, 10, "Coins: %d", p.coins);
					d.draw_text(buf, { 1.f,1.f,0.f,1.f }, Game::G_WIDTH + 0.4f, Layer::HEIGHT - 0.2f, 0.001f);
				}

				end_func();
			}
			break;
		}
	}
}
