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



struct Button
{
	Rect _r;
private:
	const char* _text;
	float _text_size;
	
	// A finger has pressed down on the button, but not let go yet.
	bool _almost_pressed;

	bool _just_pressed;

	// Whether to draw the button pressed
	bool _draw_pressed;

public:
	// midpoint for button
	void init(const char * text, float text_size, float mid_x, float mid_y, float w, float h);

	void logic(Layer& l, float cam_y, float timer, bool move);

	// for the start button
	void draw_start(Drawer& d, float cam_y);

	// for level buttons
	void draw_level(Drawer& d, float cam_y, LevelState ls);
	 
	// was the button just pressed?
	bool just_pressed();
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
private:
	Camera c;
	CloudHandler ch;

	Button _btn_start;
public:
	MenuState(Game& g);
	virtual void entry_point(Game& g) override;
	
};

struct LevelSelectorState final : public BaseState
{
private:

	std::array<Button, SaveState::TOTAL_LEVELS> _btn_levels;
	float _scroll_y;

public:
	LevelSelectorState(Game& g);
	virtual void entry_point(Game& g) override;
};

BaseState* create_new_session(Game& g, SessionToChangeTo asd, int level);


namespace LEVEL {
	enum _ : signed char {
		Tutorial,
		W1_0,
		W1_1,
		W1_2,
		W1_3,
		W1_4,
		W1_5,
		W1_6,
		W1_7,
		W1_8,
		TOTAL,
	};
}