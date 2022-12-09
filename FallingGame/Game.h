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

class Game
{
public:
	Game();
	void start();

	Layer l;
	Drawer d;
	Camera c;
	Player p;

	float death_y = 0.f;

	std::vector<Bouncer> bouncers;
	std::array<Cloud, 30> clouds;
	std::vector<Coin> coins;

	static constexpr float G_WIDTH = 0.8f; // zone for game entities
	static constexpr float G_HEIGHT = Layer::HEIGHT; // zone for game entities
private:

};

