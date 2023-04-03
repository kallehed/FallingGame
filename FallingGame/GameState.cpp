#include "GameState.h"

#include "Game.h"

#include <cmath>
#include <algorithm>
#include <cstdio>


// true -> exit out of your loop
bool start_func(BaseState& gs, Game& g)
{
	gs.timer += g.l.dt;
	if (g.l.start_frame()) {
		g.full_exit = true;
		return true;
	}
	return false; // close
}

void end_func(Game& g)
{
	if constexpr (DEV_TOOLS) {
		g.d.draw_fps(g.l.dt);
	}
	//stuff
	g.l.end_frame();
};

void new_game_session_from_menu(Game& g)
{
	delete g.gs;
	g.gs = new GameState{};
	//++g.level_at;
	GameState::init(*(GameState*)g.gs, g, 6.f * g.level_at * g.level_at);
}



void CloudHandler::init_menu(CloudHandler& ch, Drawer& d)
{
	for (int i = 0; i < ch.clouds.size(); ++i) {
		ch.clouds[i].construct(d, (float)((int)ch.clouds.size() - i) / (float)ch.clouds.size());
		ch.clouds[i].x_vel += rand_uni();
	}
}

void CloudHandler::init_game(CloudHandler& ch, Drawer& d)
{
	for (int i = 0; i < ch.clouds.size(); ++i) {
		ch.clouds[i].construct(d, (float)((int)ch.clouds.size() - i) / (float)ch.clouds.size());
	}
}

void CloudHandler::menu_logic(CloudHandler& ch, Game& g)
{
	for (auto& e : ch.clouds)
	{
		if ((e.x + e.w) < -Layer::WIDTH || (e.x - e.w) > Layer::WIDTH || (e.y - e.h) > Layer::HEIGHT) {
			e.tex = (TEX::_)(TEX::cloud_1 + rand_int(0, 4));

			e.x_vel *= -1.f;
		}
		e.x += e.x_vel * g.l.dt * (1.f - e.z);
	}
}

void CloudHandler::game_logic(CloudHandler& ch, Game& g, Camera& c)
{
	for (auto& e : ch.clouds)
	{
		if ((e.x + e.w) < -Layer::WIDTH || (e.x - e.w) > Layer::WIDTH || (e.y - e.h) > Layer::HEIGHT) {
			e.init(g.d);
		}
		e.y += (c.y_dif) * (1.f - e.z);
		e.x += e.x_vel * g.l.dt * (1.f - e.z);
	
	}
}

void MenuState::new_menu_session(Game& g)
{
	delete g.gs;
	g.gs = new MenuState{};

	MenuState::init(*(MenuState*)g.gs, g);
}


void MenuState::init(MenuState& gs, Game& g)
{
	gs.timer = 0.f;

	gs.ch.init_menu(gs.ch, g.d);

	gs.c.init();
}

void MenuState::entry_point(Game & g)
{
	MenuState& gs = *this;
	while (true) {
		if (start_func(gs, g)) return;

		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT/* | GL_DEPTH_BUFFER_BIT */);

		g.ge.enter_game_session = g.l.key_just_down(SDL_SCANCODE_P) || g.l.m_finger_just_down;

		//logic
		if (g.ge.enter_game_session) {
			new_game_session_from_menu(g);
			return;
		}

		{
			float move = 1.1f * g.l.dt;
			//p.r.y += move;
			//gs.death_y += move;
		}

		gs.ch.menu_logic(gs.ch, g);

		g.d.before_draw(g, 5.f, 0.f, gs.timer);
		g.d.draw_sky(g, gs.c);
		g.d.draw_clouds(gs.ch);
		g.d.draw_text("Falling Game!", { 1,0,1,1 }, 0, 0, 0.002f);
		//d.draw_rectangle(-Layer::WIDTH, -Layer::HEIGHT, Layer::WIDTH*2.f, Layer::HEIGHT*2.f, {1.f,1.f,1.f,1.f});
		end_func(g);
	}
}

void GameState::init(GameState& gs, Game& g, float level_length)
{
	gs.next_bouncer_y = -1.f;
	gs.next_coin_y = -1.5f;

	gs.death_y = 3.f;
	gs.timer = 0.f;
	gs.p.init();
	gs.c.init();

	gs.ch.init_game(gs.ch, g.d);

	gs.level_length = level_length;

	gs.game_state = GameState::State::Playing;
}

void GameState::entry_point(Game& g)
{
	GameState& gs = *this;

	switch (gs.game_state)
	{
	case GameState::State::Playing:
		GameState::main_loop<GameState::State::Playing>(gs, g);
		break;
	case GameState::State::Win:
		GameState::main_loop<GameState::State::Win>(gs, g);
		break;
	case GameState::State::Lose:
		GameState::main_loop<GameState::State::Lose>(gs, g);
		break;
	}
}

template <GameState::State state>
void GameState::main_loop(GameState& gs, Game& g)
{
	while (true)
	{
		//printf("new: %f",c.y);
		if (start_func(gs, g)) return;
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT/* | GL_DEPTH_BUFFER_BIT */);

		// events
		g.ge.player_to_right = g.l.key_down(SDL_SCANCODE_D);
		g.ge.player_to_left = g.l.key_down(SDL_SCANCODE_A);
		if (g.l.m_finger_down) {
			g.ge.player_to_left = g.l.m_finger_pos.x < 0.5f;
			g.ge.player_to_right = !g.ge.player_to_left;
		}

		// logic

		if constexpr (state == GameState::State::Win || state == GameState::State::Lose) {
			g.ge.exit_current_session = g.l.key_just_down(SDL_SCANCODE_P) || g.l.m_finger_just_down || g.l.key_just_down(SDL_SCANCODE_AC_BACK);
			if (g.ge.exit_current_session) {
				MenuState::new_menu_session(g);
				return;
			}
		}

		if constexpr (state == GameState::State::Playing) {
			g.ge.exit_current_session = g.l.key_just_down(SDL_SCANCODE_P) || g.l.key_just_down(SDL_SCANCODE_AC_BACK);
			if (g.ge.exit_current_session) {
				MenuState::new_menu_session(g);
				return;
			}

			if (gs.p.r.y + 2.f * gs.p.HEIGHT < gs.c.y - g.l.HEIGHT) {
				g.level_at++;
				gs.game_state = GameState::State::Win;
				return;
			}

			if (gs.death_y + 2.f * Layer::HEIGHT + gs.c.player_screen_top_offset/2.f < gs.p.r.y) {
				gs.c.y_dif = 0.f;
				gs.game_state = GameState::State::Lose;
				return;
			}

			// increase the more far away the player is from the barrier
			//gs.death_y -= 1.4f * g.l.dt * std::max(std::log(gs.timer / 10.f), std::max(1.f, 0.05f*std::pow(gs.death_y-gs.p.r.y, 2.f) ));

			gs.death_y -= 1.4f * g.l.dt * std::max(std::min(2.f, std::log(gs.timer / 10.f)), std::max(1.f, 0.05f * std::pow(gs.death_y - gs.p.r.y, 2.f)));

			// increase speed of death_y over time
			//gs.death_y -= 1.41f * g.l.dt * std::max(1.f, std::log(gs.timer / 10.f));
			gs.p.logic(g);

			const float remove_bound = gs.c.y + 2.f * g.l.HEIGHT;

			// delete bouncers that have gone too high
			for (int i = (int)gs.bouncers.size() - 1; i >= 0; --i) {
				if (gs.bouncers[i].h.y > remove_bound) { // should be removed
					gs.bouncers.erase(gs.bouncers.begin() + i);
				}
			}

			for (int i = (int)gs.coins.size() - 1; i >= 0; --i) { // delete coins that have gone too high
				if (gs.coins[i].r.y > remove_bound) { // should be removed
					gs.coins.erase(gs.coins.begin() + i);
				}
			}
			// collision between bouncers and player
			{
				for (auto& e : gs.bouncers) {
					if (e.h.y < gs.p.prev_y && e.h.y > gs.p.r.y && gs.p.r.x < e.h.x + e.h.w && gs.p.r.x + gs.p.r.w > e.h.x) {
						if (gs.p.y_vel < 0.0f) {
							//std::cout << "bounce << " << p.y_vel << "\n";
							gs.p.bounce_x_vel = std::min(1.f, -0.5f * gs.p.y_vel) * 10.f * gs.p.r.x_dist(e.h);
							gs.p.y_vel = std::clamp(-0.5f * gs.p.y_vel, 0.75f, 1.5f);
							e.bounced_on(gs.p.bounce_x_vel);
						}
					}
				}
			}

			// collision between coins and player
			{
				for (int i = (int)gs.coins.size() - 1; i >= 0; --i) {
					auto& e = gs.coins[i];
					if (gs.p.r.intersect(e.r)) {
						//std::cout << "COIN \n";
						if (!e.picked_up) {
							// make bird shiny
							gs.p.time_since_coin = 0.f;
							++gs.p.coins;
						}
						//coins.erase(coins.begin() + i);
						e.got_picked_up(g);
					}
				}
			}

			// where it is appropriate to spawn things of screen
			float spawn_bound = gs.c.y - g.l.HEIGHT * 2.f;
			if (spawn_bound > gs.level_length) { spawn_bound = 1000000000000.f; }

			// spawn bouncer
			if (spawn_bound <= gs.next_bouncer_y) {
				gs.bouncers.emplace_back(Game::G_WIDTH, gs.next_bouncer_y);
				gs.next_bouncer_y -= (2.f * g.l.HEIGHT * (0.3f + 0.5f * rand_01()));
			}

			// spawn coin
			while (spawn_bound <= gs.next_coin_y)
			{
				gs.coins.emplace_back(g, gs.next_coin_y);
				gs.next_coin_y -= (2.f * g.l.HEIGHT * (0.5f + 0.5f * rand_01()));
			}

			// set CAMERA position
			{
				gs.c.set_in_game(gs.p, -gs.level_length);
			}
		}
		gs.ch.game_logic(gs.ch, g, gs.c);

		// Drawing
		g.d.before_draw(g, gs.death_y, gs.c.y, gs.timer);

		g.d.draw_sky(g, gs.c);

		g.d.draw_clouds(gs.ch);
		for (auto& e : gs.coins) { e.draw(g, gs.c, gs); }

		gs.p.draw(g, gs.c);
		for (auto& e : gs.bouncers) { e.draw(g, gs.c); }
		// side background
		g.d.draw_sides(gs.p);

		// draw death storm at death_y
		{
			g.d.draw_image(gs.c, TEX::storm, 0.f, gs.death_y + g.l.HEIGHT, g.l.WIDTH * 2.f, g.l.HEIGHT * 2.f, 0.f);
			g.d.draw_rectangle(-g.l.WIDTH, gs.death_y + 2.f * g.l.HEIGHT, 2.f * g.l.WIDTH, g.l.HEIGHT * 2.f, {0.f,0.f,0.f,1.f});
		}

		// draw coin counter
		{
			char buf[10];
			snprintf(buf, 10, "Coins: %d", gs.p.coins);
			g.d.draw_text(buf, { 1.f,1.f,0.f,1.f }, Game::G_WIDTH - 0.2f, Layer::HEIGHT - 0.2f, 0.001f);
		}

		if constexpr (state == GameState::State::Win) {
			g.d.draw_text("You really won the level huh!", { 1.f,1.f,1.f,1.f }, -Game::G_WIDTH, 0.f, 0.002f);
		}
		if constexpr (state == GameState::State::Lose) {
			g.d.draw_text("You LOST? wow, incredible.", { 1.f,1.f,1.f,1.f }, -Game::G_WIDTH, 0.f, 0.002f);
		}

		end_func(g);
	}
}

