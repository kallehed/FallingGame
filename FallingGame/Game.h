#pragma once

#include <vector>

#include "Layer.h"
#include "Drawer.h"
#include "Camera.h"
#include "Player.h"
#include "Bouncer.h"
#include "Cloud.h"

class Game
{
public:
	Game();
	void start();

	Layer l;
	Drawer d;
	Camera c;
	Player p;

	std::vector<Bouncer> bouncers;
	std::vector<Cloud> clouds;

	static constexpr float G_WIDTH = 0.8f; // zone for game entities
	static constexpr float G_HEIGHT = Layer::HEIGHT; // zone for game entities
private:

};

