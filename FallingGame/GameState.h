#pragma once

#include "misc.h"
#include "Camera.h"
#include "Player.h"
#include "Bouncer.h"
#include "Cloud.h"
#include "Coin.h"
#include <vector>
#include <array>


struct CloudHandler
{
	static constexpr const int NR_CLOUDS = 30;
	std::array<Cloud, NR_CLOUDS> clouds;

	static void init_menu(CloudHandler& ch, Drawer& d);
	static void init_game(CloudHandler& ch, Drawer& d);

	static void menu_logic(CloudHandler& ch, Game& g);
	static void game_logic(CloudHandler& ch, Game& g, Camera& c);
};



struct BaseState
{
	float timer; // total time passed
	virtual ~BaseState() = default;
	// where game calls in for the state
	virtual void entry_point(Game& g) = 0;
};

struct MenuState final : public BaseState
{
	Camera c;
	CloudHandler ch;

	static void new_menu_session(Game& g);

	static void init(MenuState& s, Game& g);
	virtual void entry_point(Game& g) override;

};

struct GameState final : public BaseState
{
public:
	Camera c;
	Player p;

	enum class State : unsigned char {
		Playing, Win, Lose

	};

	State state;

	float death_y; // y coordinate of the death barrier
	
	float level_length;

	float next_bouncer_y;
	float next_coin_y;

	std::vector<Bouncer> bouncers;
	
	CloudHandler ch;
	std::vector<Coin> coins;

	static void init(GameState& gs, Game& g, float level_length);

	virtual void entry_point(Game& g) override;

	// playing decides if win has happened or not
	template <GameState::State state>
	static void main_loop(GameState& gs, Game& g);
};

