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

float rand_01();