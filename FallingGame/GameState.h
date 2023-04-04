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
	// inits the state
	virtual void init(Game& g) = 0;
};

struct MenuState final : public BaseState
{
	Camera c;
	CloudHandler ch;

	static void new_menu_session(Game& g);

	virtual void entry_point(Game& g) override;
	virtual void init(Game& g) override;

};

struct GameState final : public BaseState
{
public:
	Camera c;
	Player p;

	enum class State : unsigned char {
		Playing, Win, Lose
	};

	State game_state;

	float death_y; // y coordinate of the death barrier
	
	// which y coordinate the level ends at
	float level_end;

	float next_bouncer_y;
	float next_coin_y;

	std::vector<Bouncer> bouncers;
	
	CloudHandler ch;
	std::vector<Coin> coins;

	

	virtual void entry_point(Game& g) override;

	virtual void init(Game& g) override;

	// playing decides if win has happened or not
	template <GameState::State STATE>
	static void main_loop(GameState& gs, Game& g);
};

