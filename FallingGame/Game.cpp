#include "Game.h"

#include <algorithm>
#include <iostream>

#include <cmath>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

int Game::init()
{
	if (l.init()) return -1;
	d.init(*this);

	this->gs = nullptr;
	this->new_gs = nullptr;

	this->level_at = 0;

	MenuState::new_menu_session(*this);

	srand(336); //TODO add true randomness
	return 0;
}

// will change state of game if user wants to quit
static bool start_func(BaseState& gs, Game& g)
{
	gs.timer += g.l.dt;
	if (g.l.start_frame()) {
		return true;
	}
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

bool Game::should_change_state()
{
	return this->new_gs != nullptr;
}
