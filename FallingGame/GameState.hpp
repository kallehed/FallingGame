#pragma once

#include "misc.hpp"
#include "Camera.hpp"
#include "Player.hpp"
#include "Bouncer.hpp"
#include "Cloud.hpp"
#include "Coin.hpp"
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

struct BouncerHandler
{
private:
	static constexpr int _MAX_BOUNCERS = 10;
	std::array<Bouncer, _MAX_BOUNCERS> _bouncers;

	// How it works:
	// We have a `_next_bouncer_y` that we place bouncers at when the camera is sufficiently close to it.
	// We place the bouncer at `_bouncer_index`, which hopefully has already gone through the player scene, and is stranded up
	// This works as long as we don't spawn too many bouncers at once, then `_MAX_BOUNCERS` will have to be increased
	float _next_bouncer_y;
	// which bouncer to move next. When it reaches `_MAX_BOUNCERS` it resets to 0
	int _bouncer_index;

public:
	void init();

	void logic(Player& p, Camera& c, float level_end);

	void draw(Game& g, Camera& c);
};

struct Button
{
private:
	Rect _r;
	const char* _text;
	float _text_size;
	
	// A finger has pressed down on the button, but not let go yet.
	bool _almost_pressed;

	bool _just_pressed;

public:
	// midpoint for button
	void init(const char * text, float text_size, float mid_x, float mid_y, float w, float h);

	void logic(Layer& l);

	void draw(Drawer& d);

	// was the button just pressed?
	bool just_pressed();
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

	Button _btn_start;

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

	
	float next_coin_y;
	
	CloudHandler ch;
	BouncerHandler _bh;
	std::vector<Coin> coins;

	virtual void entry_point(Game& g) override;

	virtual void init(Game& g) override;

	// playing decides if win has happened or not
	template <GameState::State STATE>
	static void main_loop(GameState& gs, Game& g);
};

