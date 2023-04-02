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

	Layer l;
	Drawer d;

	bool full_exit = false;

	BaseState* gs;
	void (*loop_to_run)(BaseState&, Game& g);

	static constexpr float G_WIDTH = 0.8f; // zone for game entities
	static constexpr float G_HEIGHT = Layer::HEIGHT; // zone for game entities

};

