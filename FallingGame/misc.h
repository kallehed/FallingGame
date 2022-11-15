#pragma once

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

class Rect
{
public:
	float x, y, w, h;
	Rect(float, float, float, float); // x,y,w,h
	void draw(Drawer& d, const Color& color);
	bool intersect(Rect& e);
};

class HoLine // a horizontal line, containing x,y,w
{
public:
	float x, y, w;
	HoLine(float, float, float); // x, y, w
	void draw(Drawer& d, const Color& color);
};

// random float btw 0 and 1
float rand_01();

// decr val by dec in absolute terms. dec must be positive. 
// val could be any number. if dec > abs(val): return 0 
float decr_abs_val(float val, float dec);

constexpr float pi = 3.14159265359f;
constexpr float tau = 6.28318530718f;