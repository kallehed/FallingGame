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
#include "CoinParticle.h"

class Game
{
private:
	void init();
public:
	void start();

	Layer l;
	Drawer d;
	Camera c;
	Player p;

	float m_death_y; // y coordinate of the death barrier
	float m_timer; // total time passed

	std::vector<Bouncer> m_bouncers;
	static constexpr const int NR_CLOUDS = 30;
	std::array<Cloud, NR_CLOUDS> m_clouds;
	std::vector<Coin> m_coins;

	static constexpr float G_WIDTH = 0.8f; // zone for game entities
	static constexpr float G_HEIGHT = Layer::HEIGHT; // zone for game entities
private:

};

