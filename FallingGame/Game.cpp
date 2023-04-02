#include "Game.h"

#include <algorithm>
#include <iostream>

#include <cmath>

void Game::init()
{
	l.init();
	d.init(*this);

	this->gs = nullptr;

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
		gs->entry_point(*this);
	}
}
