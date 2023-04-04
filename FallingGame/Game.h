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

	void set_new_state(BaseState* some_gs);

	GameEvents ge;

	int level_at;

	Layer l;
	Drawer d;

	bool full_exit;

	

private:
	// pointer to a base type which has dynamic dispath to the current game loop. So only virtual calls VERY rarely basically.
	BaseState* gs;

	// should be set when changing game scene, then gs will be set to it automatically, and it will be set to null again. + old gs deleted
	BaseState* new_gs;
public:

	static constexpr float G_WIDTH = 0.8f; // zone for game entities
	static constexpr float G_HEIGHT = Layer::HEIGHT; // zone for game entities

};

