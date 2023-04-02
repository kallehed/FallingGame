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


	float next_bouncer_y;
	float next_coin_y;

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

	auto new_game_session = [this, &go_to_new_state, &game_state, &next_bouncer_y, &next_coin_y]()
	{
		go_to_new_state = true;
		game_state = GAME_STATE::Game;

		next_bouncer_y = -1.f;
		next_coin_y = -1.5f;
		m_bouncers.clear();
		m_coins.clear();

		

		m_death_y = 3.f;
		m_timer = 0.f;
		p.init();
		c.set_in_game(*this);

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

				d.before_draw(*this);
				d.draw_sky(*this);
				d.draw_clouds(*this);
				d.draw_text("Falling Game!", { 1,0,1,1 }, 0, 0, 0.002f);
				//d.draw_rectangle(-Layer::WIDTH, -Layer::HEIGHT, Layer::WIDTH*2.f, Layer::HEIGHT*2.f, {1.f,1.f,1.f,1.f});
				end_func();
			}
			break;
		case GAME_STATE::Game:
			while (!go_to_new_state)
			{
				//printf("new: %f",c.y);
				start_func();
				glClearColor(0.0, 0.0, 0.0, 1.0);
				glClear(GL_COLOR_BUFFER_BIT/* | GL_DEPTH_BUFFER_BIT */);

				// logic

				if (l.key_just_down(SDL_SCANCODE_P)) {
					new_menu_session();
					break;
				}

				if (p.r.y < c.y - 1.5f*l.HEIGHT) {
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

				const float remove_bound = c.y + 2.f * l.HEIGHT;

				// delete bouncers that have gone too high
				for (int i = (int)m_bouncers.size() - 1; i >= 0; --i) { 
					if (m_bouncers[i].h.y > remove_bound) { // should be removed
						m_bouncers.erase(m_bouncers.begin() + i);
					}
				}
				for (auto& e : m_coins) { e.logic(*this); }
				for (int i = (int)m_coins.size() - 1; i >= 0; --i) { // delete coins that have gone too high
					if (m_coins[i].r.y > remove_bound) { // should be removed
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

				// collision between coins and player
				{
					for (int i = (int)m_coins.size() - 1; i >= 0; --i) {
						auto& e = m_coins[i];
						if (p.r.intersect(e.r)) {
							std::cout << "COIN \n";
							if (!e.picked_up) {
								// make bird shiny
								p.time_since_coin = 0.f;
								++p.coins;
							}
							//coins.erase(coins.begin() + i);
							e.got_picked_up(*this);
						}
					}
				}

				// where it is appropriate to spawn things of screen
				const float spawn_bound = c.y - l.HEIGHT * 2.f;

				// spawn bouncer
				if (spawn_bound <= next_bouncer_y) {
					m_bouncers.emplace_back(G_WIDTH, next_bouncer_y);
					next_bouncer_y -= (2.f * l.HEIGHT * (0.3f + 0.5f * rand_01()));
				}
				
				// spawn coin
				while (spawn_bound <= next_coin_y)
				{
					m_coins.emplace_back(*this, next_coin_y);
					next_coin_y -= (2.f * l.HEIGHT * (0.5f + 0.5f*rand_01()));
				}

				// set CAMERA position
				{
					c.set_in_game(*this);
				}

				// Drawing
				d.before_draw(*this);

				d.draw_sky(*this);

				this->d.draw_clouds(*this);
				for (auto& e : m_coins) { e.draw(*this); }

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
					d.draw_text(buf, { 1.f,1.f,0.f,1.f }, Game::G_WIDTH - 0.2f, Layer::HEIGHT - 0.2f, 0.001f);
				}

				end_func();
			}
			break;
		}
	}
}
