#pragma once

#include <vector>
#include <array>

#include "Layer.hpp"
#include "Drawer.hpp"
#include "Camera.hpp"
#include "Player.hpp"
#include "Bouncer.hpp"
#include "Cloud.hpp"
#include "Coin.hpp"
#include "GameState.hpp"



class Game
{
private:
	// if NOT 0 -> end application
	int init();
	
public:
	// called from main.cpp
	void start();

	// false -> exit
	bool to_be_looped();
	
	void set_new_state(BaseState* some_gs);

	GameEvents ge;

	SaveState _save_state;
	
	// should be set to true when a game session wants to end the whole game (Probably just the menu that wants to set this)
	bool should_quit;

	Layer l;
	Drawer d;

private:
	// pointer to a base type which has dynamic dispath to the current game loop. So only virtual calls VERY rarely basically.
	BaseState* gs;

	// should be set when changing game scene, then gs will be set to it automatically, and it will be set to null again. + old gs deleted
	BaseState* new_gs;
public:

	static constexpr float G_WIDTH = 0.8f; // zone for game entities
	static constexpr float G_HEIGHT = Layer::HEIGHT; // zone for game entities

};

