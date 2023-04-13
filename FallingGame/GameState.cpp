#include "GameState.h"

#include "Game.h"

#include <cmath>
#include <algorithm>
#include <cstdio>




static void new_game_session_from_menu(Game& g)
{
	g.set_new_state(new GameState{});
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
	g.set_new_state(new MenuState{});
}


void MenuState::init(Game& g)
{
	MenuState& gs = *this;
	gs.timer = 0.f;

	gs.ch.init_menu(gs.ch, g.d);

	gs.c.init();
}

void MenuState::entry_point(Game & g)
{
	MenuState& gs = *this;

	g.ge.enter_game_session = g.l.key_just_down(SDL_SCANCODE_P) || g.l.m_finger_just_down;

	//logic
	if (g.ge.enter_game_session) {
		new_game_session_from_menu(g);
		//return;
	}

	{
		float move = 1.1f * g.l.dt;
		//p.r.y += move;
		//gs.death_y += move;
	}

	gs.ch.menu_logic(gs.ch, g);

	g.d.before_draw(g, 5.f, 0.f, gs.timer, {1.f,1.f,1.f,1.f});
	g.d.draw_sky(g, gs.c);
	g.d.draw_clouds(gs.ch);
	g.d.draw_text("Falling Game!", { 1,0,1,1 }, 0, 0, 0.002f);
	//d.draw_rectangle(-Layer::WIDTH, -Layer::HEIGHT, Layer::WIDTH*2.f, Layer::HEIGHT*2.f, {1.f,1.f,1.f,1.f});
}

void GameState::init(Game& g)
{
	GameState& gs = *this;
	gs.next_bouncer_y = -1.f;
	gs.next_coin_y = -1.5f;

	gs.death_y = 3.f;
	gs.timer = 0.f;
	gs.p.init();
	gs.c.init();

	gs.ch.init_game(gs.ch, g.d);

	gs.level_end = -6.f * g.level_at * g.level_at;

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

static void set_movement_events(Game& g)
{
	g.ge.player_to_right = g.l.key_down(SDL_SCANCODE_D);
	g.ge.player_to_left = g.l.key_down(SDL_SCANCODE_A);
	if (g.l.m_finger_down) {
		g.ge.player_to_left = g.l.m_finger_pos.x < 0.5f;
		g.ge.player_to_right = !g.ge.player_to_left;
	}
}

static void set_exit_events(Game& g)
{
	g.ge.exit_current_session = g.l.key_just_down(SDL_SCANCODE_P) || g.l.key_just_down(SDL_SCANCODE_AC_BACK);
}

static void handle_collisions_player_bouncers(std::vector<Bouncer>& bouncers, Player& p)
{
	for (auto& e : bouncers) {
		if (e.h.y < p.prev_y && e.h.y > p.r.y && p.r.x < e.h.x + e.h.w && p.r.x + p.r.w > e.h.x) {
			if (p.y_vel < 0.0f) {
				//std::cout << "bounce << " << p.y_vel << "\n";
				p.bounce_x_vel = std::min(1.f, -0.5f * p.y_vel) * 10.f * p.r.x_dist(e.h);
				p.y_vel = std::clamp(-0.5f * p.y_vel, 0.75f, 1.5f);
				e.bounced_on(p.bounce_x_vel);
			}
		}
	}
}

static void handle_collisions_player_coins(std::vector<Coin>& coins, Player& p)
{
	for (int i = (int)coins.size() - 1; i >= 0; --i) {
		auto& e = coins[i];
		if (p.r.intersect(e.r)) {
			//std::cout << "COIN \n";
			if (!e.picked_up) {
				// make bird shiny
				p.time_since_coin = 0.f;
				++p.coins;
			}
			//coins.erase(coins.begin() + i);
			e.got_picked_up();
		}
	}
}

template <GameState::State STATE>
void GameState::main_loop(GameState& gs, Game& g)
{
	// events
	set_movement_events(g);
	set_exit_events(g);

	// logic

	if constexpr (STATE == GameState::State::Win || STATE == GameState::State::Lose)
	{
		g.ge.exit_current_session |= g.l.m_finger_just_down;

		gs.c.y_dif = 0.f;
	}

	// Exit
	if (g.ge.exit_current_session) {
		MenuState::new_menu_session(g);
	}

	if constexpr (STATE == GameState::State::Playing)
	{
		// Win
		if (gs.p.r.y + 2.f * gs.p.HEIGHT < gs.c.y - g.l.HEIGHT) {
			g.level_at++;
			gs.game_state = GameState::State::Win;
		}

		// Lose
		if (gs.death_y + 2.f * Layer::HEIGHT + gs.c.player_screen_top_offset/2.f < gs.p.r.y) {
			gs.game_state = GameState::State::Lose;
		}

		// increase the more far away the player is from the barrier
		//gs.death_y -= 1.4f * g.l.dt * std::max(std::log(gs.timer / 10.f), std::max(1.f, 0.05f*std::pow(gs.death_y-gs.p.r.y, 2.f) ));

		gs.death_y -= 1.4f * g.l.dt * std::max(std::min(1.8f, std::log(gs.timer / 10.f)), std::max(1.f, 0.05f * std::pow(gs.death_y - gs.p.r.y, 2.f)));

		// increase speed of death_y over time
		//gs.death_y -= 1.41f * g.l.dt * std::max(1.f, std::log(gs.timer / 10.f));
		gs.p.logic(g.ge, g.l.dt);

		const float remove_bound = gs.c.y + 2.f * g.l.HEIGHT;

		// delete bouncers that have gone too high
		for (int i = (int)gs.bouncers.size() - 1; i >= 0; --i) {
			if (gs.bouncers[i].h.y > remove_bound) { // should be removed
				gs.bouncers.erase(gs.bouncers.begin() + i);
			}
		}

		// delete coins that have gone too high
		for (int i = (int)gs.coins.size() - 1; i >= 0; --i) { 
			if (gs.coins[i].r.y > remove_bound) { // should be removed
				gs.coins.erase(gs.coins.begin() + i);
			}
		}
		handle_collisions_player_bouncers(gs.bouncers, gs.p);
			
		handle_collisions_player_coins(gs.coins, gs.p);

		// where it is appropriate to spawn things of screen
		float spawn_bound = gs.c.y - g.l.HEIGHT * 2.f;
		if (spawn_bound < gs.level_end - g.l.HEIGHT * 2.f) { spawn_bound = 1000000000.f; }

		// spawn bouncer
		while (spawn_bound <= gs.next_bouncer_y) {
			gs.bouncers.emplace_back(Game::G_WIDTH, gs.next_bouncer_y);
			gs.next_bouncer_y -= (2.8f * (0.3f + 0.5f * rand_01()));
		}

		// spawn coin
		while (spawn_bound <= gs.next_coin_y)
		{
			gs.coins.emplace_back(g, gs.next_coin_y);
			gs.next_coin_y -= (2.f * g.l.HEIGHT * (0.5f + 0.5f * rand_01()));
		}

		// set CAMERA position
		{
			gs.c.set_in_game(gs.p, gs.level_end);
		}
	}
	gs.ch.game_logic(gs.ch, g, gs.c);

	// Drawing
	g.d.before_draw(g, gs.death_y, gs.c.y, gs.timer, {0.f,0.f,0.f,1.f});

	g.d.draw_sky(g, gs.c);

	g.d.draw_clouds(gs.ch);
	for (auto& e : gs.coins) { e.draw(g, gs.c, gs); }

	gs.p.draw(g, gs.c);
	for (auto& e : gs.bouncers) { e.draw(g, gs.c); }
	// side background
	g.d.draw_sides(gs.p);

	// draw death storm at death_y
	{
		g.d.draw_image(TEX::storm, 0.f, gs.death_y + g.l.HEIGHT, g.l.WIDTH * 2.f, g.l.HEIGHT * 2.f, 0.f);
		g.d.draw_rectangle(-g.l.WIDTH, gs.death_y + 2.f * g.l.HEIGHT, 2.f * g.l.WIDTH, g.l.HEIGHT * 2.f, {0.f,0.f,0.f,1.f});
	}

	// draw coin counter
	{
		char buf[10];
		snprintf(buf, 10, "Coins: %d", gs.p.coins);
		g.d.draw_text(buf, { 1.f,1.f,0.f,1.f }, Game::G_WIDTH - 0.2f, Layer::HEIGHT - 0.2f, 0.001f);
	}

	if constexpr (STATE == GameState::State::Win) {
		g.d.draw_text("You really won the level huh!", { 1.f,1.f,1.f,1.f }, -Game::G_WIDTH, 0.f, 0.002f);
	}
	if constexpr (STATE == GameState::State::Lose) {
		g.d.draw_text("You LOST? wow, incredible.", { 1.f,1.f,1.f,1.f }, -Game::G_WIDTH, 0.f, 0.002f);
	}
}

