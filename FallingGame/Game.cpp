#include "Game.hpp"

#include <algorithm>
#include <iostream>

#include <cmath>

#include <nlohmann/json.hpp>
using json = nlohmann::json;


#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SaveState::LevelInfo, state)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SaveState, level_info)

static void save_game (SaveState& s) 
{
	json j = s;
	auto d = j.dump(2);

	std::cout << d.c_str() << "\n";

	SDL_RWops* w = SDL_RWFromFile("save.json", "w");

	w->write(w, d.c_str(), 1, d.size());

	w->close(w);
}

static void load_game(Game& g)
{
	// will initialize everything as if never played game before
	auto no_save_file = [&g]() {
		int i = 0;
		for (auto& e : g._save_state.level_info) {
			if (i == 0) {
				e.state = LevelState::Unlocked;
			}
			else {
				e.state = LevelState::Locked;
			}
			++i;
		}
	};

	SDL_RWops* r = SDL_RWFromFile("save.json", "r");
	if (r == NULL) {
		// no file found
		no_save_file();
		
	}
	else {
		auto size = r->size(r);
		auto buffer = new unsigned char[size + 1];
		r->read(r, buffer, 1, size);
		r->close(r);

		buffer[size] = '\0';

		try {

			json data = json::parse(buffer);

			auto levels = data["level_info"].get<std::vector<SaveState::LevelInfo>>();
			int i = 0;
			for (auto& e : levels) {
				if (i >= g._save_state.level_info.size()) break;
				g._save_state.level_info[i] = e;
				++i;
			}
			//g._save_state = save_state;
		}
		catch (const std::exception& e) {
			SDL_LogError(0, "Exception at deserializing save data: %s\n", e.what());
			no_save_file();
		}

		delete[] buffer;
	}
}

static int event_filterer(void* userdata, SDL_Event* e)
{
    if (e->type == SDL_APP_WILLENTERBACKGROUND) {
		Game& g = *((Game*)userdata);
		save_game(g._save_state);
		SDL_Log("KALLE SDL APP AHHHHHHHHHHHHHHHHHHH WE WERE SO BACKGROUND/TERMINATE RIGHT NOW IT IS SO HORRIBLEEEEEEEEEEEEEEEEEEEEEEEEE\nAAAAAAAAAAA\nAAAAAAAAAA\nAAAAAA");
		return 0;
	}
	return 1;	
}

int Game::init()
{
	if (l.init()) return -1;
	d.init(*this);

	this->gs = nullptr;
	this->new_gs = nullptr;

	load_game(*this);

	this->should_quit = false;


	// set up filter to check for quit events, specific for android, as it will close your application with little notice
	{
		SDL_SetEventFilter(event_filterer,(void*)this);
	}

	MenuState::new_menu_session(*this);

	srand(336); //TODO add true randomness
	return 0;
}

// will change state of game if user wants to quit
static bool start_func(BaseState& gs, Game& g)
{
	gs.timer += g.l.dt;
	if (g.l.start_frame() || g.should_quit) {
		return true;
	}
	
	// set most basic events
	g.ge.exit_current_session = g.l.key_just_down(SDL_SCANCODE_P) || g.l.key_just_down(SDL_SCANCODE_AC_BACK);
	g.ge.enter_game_session = g.l.key_just_down(SDL_SCANCODE_O);

	return false;
}

static void end_func(Game& g)
{
	if constexpr (DEV_TOOLS) {
		g.d.draw_fps(g.l.dt);
	}
	//stuff
	g.l.end_frame();
};

#ifdef __EMSCRIPTEN__
static void emscripten_main_loop_callback(void* arg)
{
	Game* g = (Game*)arg;
	g->to_be_looped();

}
#endif

bool Game::to_be_looped() {
	if (this->new_gs != nullptr) {
		delete this->gs; // should be safe, even for nullptr

		// swap game states to new one
		this->gs = this->new_gs;
		this->new_gs = nullptr;

		// init it the first time
		this->gs->init(*this);
	}

	// increases timer, gets events, possibly exits
	if (start_func(*gs, *this)) return false;

	// this does NOT loop
	gs->entry_point(*this);

	// delays, swaps frame buffers
	end_func(*this);
	return true;
}

void Game::start()
{
	if (init()) return;;

	// this loop calls a virtual function EACH frame. This is the only loop
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(emscripten_main_loop_callback, (void*)this, 0, true);
#else
	while (to_be_looped());
#endif
	// TODO: possibly handle saving and stuff?
	save_game(_save_state);
}

void Game::set_new_state(BaseState* some_gs)
{
	if (this->new_gs != nullptr) {
		SDL_LogError(0, "Seems that we set the game state twice in one frame?!?!?!?!");
		//exit(1); // crash to notice, EXCEPT maybe don't crash if user tries to exit at the same time as a session change
	}
	else {
		this->new_gs = some_gs;
	}
}
