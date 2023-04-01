#pragma once

// set this if on computer and wanting to develop for mobile
#if defined __ANDROID__  || true
// make game for vertical playing
#define KALLE_GAME_VERTICAL 
#endif

constexpr inline bool DRAW_HB = false; // draw hitboxes?s
// R for hot reload.
constexpr inline bool DEV_TOOLS = true; // allow development stuff

class Game;
class Drawer;

struct Color {
	float r;
	float g;
	float b;
	float a;
};

struct Pos { // position: x, y. floats
	float x;
	float y;
	Pos(float X, float Y) : x(X), y(Y) {};
};

class HoLine;

class Rect // for hitboxes
{
public:
	float x, y, w, h;
	Rect(float, float, float, float); // x,y,w,h
	Rect();
	void draw(Drawer& d, const Color& color);
	bool intersect(Rect& e);
	float x_dist(HoLine&);
};

class HoLine // a horizontal line, containing x,y,w. A hitbox
{
public:
	float x, y, w;
	HoLine(float, float, float); // x, y, w
	void draw(Drawer& d, const Color& color);
	float x_dist(HoLine& h); // distance in x to other holine
};

// random float btw 0 and 1
float rand_01();

// random float btw -1 and 1
float rand_uni();

// random int from a to b, not including b
int rand_int(int a, int b);

float sign(float val); // returns 1.f if >= 0, else -1

// decr val by dec in absolute terms. dec must be positive. 
// val could be any number. if dec > abs(val): return 0 
float decr_abs_val(float val, float dec);

constexpr inline float pi = 3.14159265359f;
constexpr inline float tau = 6.28318530718f;

namespace TEX
{
	enum _ : char {
		bird,
		bird_closed,
		mushroom_cap,
		mushroom_stem,
		side_background,
		vines,
		sky,
		sky_blurred,
		sky2,
		sky3,
		storm,
		cloud_1,
		cloud_2,
		cloud_3,
		cloud_4,
		coin,
		coin_blurred,

		TOTAL
	};

}