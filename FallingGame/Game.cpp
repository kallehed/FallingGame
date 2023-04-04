#include "Game.h"

#include <algorithm>
#include <iostream>

#include <cmath>

void Game::init()
{
	l.init();
	d.init(*this);

	this->gs = nullptr;
	this->new_gs = nullptr;

	this->level_at = 0;
	this->full_exit = false;

	MenuState::new_menu_session(*this);

	srand(336); //TODO add true randomness
}

void Game::start()
{
	init();

	// template state:
	/*
	case GAME_STATE::Game_End_Win_Screen:
	{
		while (!go_to_new_state)
		{
			start_func();

			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT)// | GL_DEPTH_BUFFER_BIT );

			d.before_draw(*this);

			end_func();

		}
		break;
	}
	*/

	while (!full_exit) {
		if (this->new_gs != nullptr) {
			delete this->gs; // should be safe, even for nullptr
			
			// swap game states to new one
			this->gs = this->new_gs;
			this->new_gs = nullptr;

			// init it the first time
			this->gs->init(*this);
		}
		gs->entry_point(*this);
	}
}

void Game::set_new_state(BaseState* some_gs)
{
	if (this->new_gs != nullptr) {
		SDL_LogError(0, "Seems that we set the game state twice in one frame?!?!?!?!");
	}
	else {
		this->new_gs = some_gs;
	}
}
