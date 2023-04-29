#include "GameState.hpp"

#include "Game.hpp"

#include <cmath>
#include <algorithm>
#include <cstdio>


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

void BouncerHandler::init(float percent_move) {
	_next_bouncer_y = -0.5f;
	_bouncer_index = 0;
	_percent_move = percent_move;

	for (auto& e : _bouncers) {
		e.init(0.f, 2.f*Layer::HEIGHT, Bouncer::Type::Normal);
	}
}

void BouncerHandler::logic(Player& p, Camera& c, float level_end, float timer)
{
	// Bounce Logic
	const bool can_bounce_player = p.y_vel < 0.0f;
	
	for (auto& e : _bouncers) {
		if (can_bounce_player) {
			// TODO: Maybe remove the branches from this if?
			if (e.h.y < p.prev_y && e.h.y > p.r.y && p.r.x < e.h.x + e.h.w && p.r.x + p.r.w > e.h.x) {
				//std::cout << "bounce << " << p.y_vel << "\n";
				p.bounce_x_vel = std::min(1.f, -0.5f * p.y_vel) * 10.f * p.r.x_dist(e.h);
				p.y_vel = std::clamp(-0.5f * p.y_vel, 0.75f, 1.5f);
				e.bounced_on(p.bounce_x_vel);
			}
		}

		switch (e._type) {
		case Bouncer::Type::Moves:
			e.h.x += 0.01f * sinf(timer);
			break;
		default:
			break;
		}
	}

	// Spawn bouncers
	float spawn_bound = c.y - Layer::HEIGHT * 1.25f;
	while (spawn_bound <= _next_bouncer_y && _next_bouncer_y > level_end - Layer::HEIGHT)
	{
		auto type = (rand_01() > _percent_move) ? Bouncer::Type::Normal : Bouncer::Type::Moves;
		_bouncers[_bouncer_index++].init( Game::G_WIDTH, _next_bouncer_y, type);
		if (_bouncer_index >= _MAX_BOUNCERS) { _bouncer_index = 0; }

		_next_bouncer_y -= (2.8f * (0.3f + 0.5f * rand_01()));
		//_next_bouncer_y -= 0.05f;
	}
}

void BouncerHandler::draw(Game& g, Camera& c)
{
	for (auto& e : _bouncers) { e.draw(g, c); }
}


void Button::init(const char* text, float text_size, float mid_x, float mid_y, float w, float h)
{
	_r.init(mid_x - w/2.f, mid_y - h/2.f, w, h);
	_text = text;
	_text_size = text_size;
	_almost_pressed = false;
	_just_pressed = false;
	_draw_pressed = false;
}

void Button::logic(Layer& l, float cam_y, float timer, bool move)
{
	_just_pressed = false;
	_draw_pressed = false;

	if (move)
		_r.x += 0.0010f * cosf(timer);

	Pos finger = l.m_finger_pos;
	finger.y += cam_y;
	if (!_almost_pressed)
	{
		if (l.m_finger_just_down && _r.intersect_point(finger))
		{
			_almost_pressed = true;
			_draw_pressed = true;
		}
	}
	else
	{
		static constexpr float BOUND = 0.01f;
		if (fabs(l.m_finger_move.y) > BOUND || fabs(l.m_finger_move.x) > BOUND) { _almost_pressed = false; return; }
		bool intersects = _r.intersect_point(finger);
		_draw_pressed = intersects;
		if (l._finger_just_up)
		{
			if (intersects)
			{
				_just_pressed = true;
			}
			_almost_pressed = false;
		}
	}
}

void Button::draw_start(Drawer& d, float cam_y)
{
	Color c = (_draw_pressed) ? (Color{ 0.5f,0.5f,0.f,1.f }) : (Color { 1.f,1.f,0.f,1.f });
	d.draw_rectangle(_r.x, _r.y, _r.w, _r.h, c);
	d.draw_text<true>(_text, { 0.f,0.f,0.f,1.f }, _r.x + _r.w/2.f, _r.y + _r.h/2.f - cam_y, _text_size);
}

void Button::draw_level(Drawer& d, float cam_y, LevelState ls)
{
	Color c;
	switch (ls) {
	case LevelState::Locked:
		c = Color{ 0.5f,0.5f,0.f,0.5f };
		break;
	case LevelState::Unlocked:
		c = (_draw_pressed) ? (Color{ 0.5f,0.5f,0.f,1.f }) : (Color{ 1.f,1.f,0.f,1.f });
		break;
	case LevelState::Done:
		c = (_draw_pressed) ? (Color{ 0.5f,0.5f,0.f,1.f }) : (Color{ 0.35f,0.95f,0.1f,1.f });
		break;
	}
	d.draw_rectangle(_r.x, _r.y, _r.w, _r.h, c);
	d.draw_text<true>(_text, { 0.f,0.f,0.f,1.f }, _r.x + _r.w / 2.f, _r.y + _r.h / 2.f - cam_y, _text_size);
}

bool Button::just_pressed() { return _just_pressed; }


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

	gs._btn_start.init("Start", 0.0025f, 0.f, 0.f, 0.5f, 0.25f);
}

void MenuState::entry_point(Game & g)
{
	MenuState& gs = *this;

	if (g.ge.exit_current_session) {
		g.should_quit = true;
	}

	gs._btn_start.logic(g.l, 0.f, 0.f, false);

	g.ge.enter_game_session |= gs._btn_start.just_pressed();

	// logic
	if (g.ge.enter_game_session) {
		g.set_new_state(new LevelSelectorState{});
	}

	{
		float move = 1.1f * g.l.dt;
		gs.c.y += move;
	}

	gs.ch.menu_logic(gs.ch, g);

	g.d.before_draw(g, 5000000.f, 0.f, gs.timer, {1.f,1.f,1.f,1.f});
	g.d.draw_sky(g, gs.c.y);
	g.d.draw_clouds(gs.ch);
	gs._btn_start.draw_start(g.d, 0.f);
	g.d.draw_text<true>("Down-Fall", { 0.5f,0.f,0.5f,1.f }, 0.00f, Layer::HEIGHT*0.5f, 0.004f);
	//d.draw_rectangle(-Layer::WIDTH, -Layer::HEIGHT, Layer::WIDTH*2.f, Layer::HEIGHT*2.f, {1.f,1.f,1.f,1.f});
}


void LevelSelectorState::init(Game& g)
{
	int i = 0;

	 static constexpr std::array<const char *, 9> names = {
			"Level 1",
			"Level 2",
			"Level 3",
			"Level 4",
			"Level 5",
			"Level 6",
			"Level 7",
			"Level 8",
			"Level 9",
	};

	for (auto& e : _btn_levels)
	{
		e.init(names[i], 0.00175f, 0.4f * sinf(float(i) / 1.75f), -i * 0.3f, 0.5f, 0.2f);
		++i;
	}
}

void LevelSelectorState::entry_point(Game& g)
{
	LevelSelectorState& gs = *this;
	//logic

	if (g.ge.exit_current_session) {
		g.set_new_state(new MenuState{});
	}

	/*if (g.l.m_finger_down)*/ {
		_scroll_y += g.l._finger_scroll.y;
		_scroll_y = fmin(0.f, _scroll_y);
		_scroll_y = fmax(-3.f, _scroll_y);
	}
	const float cam_y = _scroll_y;

	{
		int i = 0;
		for (auto& e : _btn_levels) {
			e.logic(g.l, cam_y, 2.f* gs.timer + 0.7f * float(i), true);
			if (e.just_pressed() && g._save_state.level_info[i].state != LevelState::Locked) {
				g.set_new_state(new GameState{i});
			}
			++i;
		}
	}

	//printf("finger rel y: %f\n", g.l.m_finger_move.y);

	//draw
	
	g.d.before_draw(g, 5000000.f, cam_y, gs.timer, { 1.f,1.f,1.f,1.f });
	g.d.draw_sky(g, gs.timer);
	int i = 0;
	for (auto& e : _btn_levels) { e.draw_level(g.d, cam_y, g._save_state.level_info[i].state); ++i; }

	g.d.draw_text<true>("Level Selector", { 0.5f,0.f,0.5f,1.f }, 0.00f, Layer::HEIGHT * 0.5f - cam_y, 0.004f);
}

GameState::GameState(int level)
{
	this->_level = level;
}

void GameState::init(Game& g)
{
	GameState& gs = *this;
	gs._bh.init(0.f);
	
	gs.next_coin_y = -1.5f;

	gs.death_y = 3.f;
	gs.timer = 0.f;
	gs.p.init();
	gs.c.init();
	gs.c.set_in_game(gs.p, -1000.f);

	this->level_end = -6.f * _level * _level;

	gs.ch.init_game(gs.ch, g.d);

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
		g.ge.player_to_left = g.l.m_finger_pos.x < 0.0f; // middle of screen
		g.ge.player_to_right = !g.ge.player_to_left;
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
			gs.game_state = GameState::State::Win;
			g._save_state.level_info[gs._level].state = LevelState::Done;
			if (gs._level + 1 < SaveState::TOTAL_LEVELS) {
				if (g._save_state.level_info[gs._level + 1].state != LevelState::Done) {
					g._save_state.level_info[gs._level + 1].state = LevelState::Unlocked;
				}
			}
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

		// set CAMERA position
		gs.c.set_in_game(gs.p, gs.level_end);

		const float remove_bound = gs.c.y + 2.f * g.l.HEIGHT;

		// delete coins that have gone too high
		for (int i = (int)gs.coins.size() - 1; i >= 0; --i) { 
			if (gs.coins[i].r.y > remove_bound) { // should be removed
				gs.coins.erase(gs.coins.begin() + i);
			}
		}
		//printf("Camera y: %f", gs.c.y);
		gs._bh.logic(gs.p, gs.c, gs.level_end, gs.timer);
			
		handle_collisions_player_coins(gs.coins, gs.p);

		// where it is appropriate to spawn things of screen
		float spawn_bound = gs.c.y - g.l.HEIGHT * 2.f;
		if (spawn_bound < gs.level_end - g.l.HEIGHT * 2.f) { spawn_bound = 1000000000.f; }

		// spawn coin
		while (spawn_bound <= gs.next_coin_y)
		{
			gs.coins.emplace_back(g, gs.next_coin_y);
			gs.next_coin_y -= (2.f * g.l.HEIGHT * (0.5f + 0.5f * rand_01()));
		}
	}
	gs.ch.game_logic(gs.ch, g, gs.c);

	// Drawing
	g.d.before_draw(g, gs.death_y, gs.c.y, gs.timer, {0.f,0.f,0.f,1.f});

	g.d.draw_sky(g, gs.c.y);

	g.d.draw_clouds(gs.ch);
	for (auto& e : gs.coins) { e.draw(g, gs.c, gs); }

	gs.p.draw(g, gs.c);
	gs._bh.draw(g, gs.c);

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

