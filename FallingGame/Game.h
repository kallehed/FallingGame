#pragma once

#include <vector>
#include <array>

#include "Layer.h"
#include "Drawer.h"
#include "Camera.h"
#include "Player.h"
#include "Bouncer.h"
#include "Cloud.h"
#include "Coin.h"
#include "GameState.h"


class Game
{
private:
	void init();
public:
	void start();

	struct GameEvents {
		// true when you want to enter a game from a menu
		bool enter_game_session = false; // NOT USED

		// when you want to exit a game session in the middle of it to menu
		bool exit_active_game_session = false; 

		// when  you want to exit from a winning game state to menu
		bool exit_winning_game_state = false;

		// movement for bird, or Player
		bool player_to_right = false;
		bool player_to_left = false;

	};

	GameEvents ge;

	int level_at = 0;

	Layer l;
	Drawer d;

	bool full_exit = false;

	BaseState* gs;

	static constexpr float G_WIDTH = 0.8f; // zone for game entities
	static constexpr float G_HEIGHT = Layer::HEIGHT; // zone for game entities

};

