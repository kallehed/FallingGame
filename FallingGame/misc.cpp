#include "misc.h"

#include "Drawer.h"
//#include <cstdlib>
#include <cmath>

Rect::Rect(float X, float Y, float W, float H) : x(X), y(Y), w(W), h(H)
{
}

Rect::Rect()
{
}

void Rect::draw(Drawer& d, const Color& color)
{
	if constexpr (DRAW_HB) {
		d.draw_rectangle(x, y, w, h, color);
	}
}

bool Rect::intersect(Rect& e)
{
	return ((x <= e.x + e.w) && (x + w >= e.x) && (y <= e.y + e.h) && (y + h >= e.y));
}

float Rect::x_dist(HoLine& h)
{
	return (x + w / 2.f) - (h.x + h.w / 2.f);
}

HoLine::HoLine(float _x, float _y, float _w) : x(_x), y(_y), w(_w)
{
}

void HoLine::draw(Drawer& d, const Color& color)
{
	if constexpr (DRAW_HB) {
		d.draw_rectangle(x, y, w, 0.01f, color);
	}
}

float HoLine::x_dist(HoLine& h)
{
	return (x + w / 2.f) - (h.x + h.w / 2.f);
}

float rand_01()
{
	return (((float)rand()) / ((float)RAND_MAX));
}

float rand_uni()
{
	return 2.f*(rand_01() - 0.5f);
}

int rand_int(int a, int b)
{
	return a + (rand() % (b - a));
}

float sign(float val)
{
	return (val >= 0.f) ? (1.f) : (-1.f);
}

float decr_abs_val(float val, float dec)
{
	if (std::abs(val) <= dec) {
		return 0.f;
	}
	else {
		return val + (dec * (val >= 0.f ? (-1.f) : 1.f));
	}
}