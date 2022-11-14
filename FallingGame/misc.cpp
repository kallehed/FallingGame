#include "misc.h"

#include "Drawer.h"
#include <cstdlib>

Rect::Rect(float X, float Y, float W, float H) : x(X), y(Y), w(W), h(H)
{
}

void Rect::draw(Drawer& d, const Color& color)
{
	d.draw_rectangle(x, y, w, h, color);
}

bool Rect::intersect(Rect& e)
{
	return ((x <= e.x + e.w) && (x + w >= e.x) && (y <= e.y + e.h) && (y + h >= e.y));
}

HoLine::HoLine(float _x, float _y, float _w) : x(_x), y(_y), w(_w)
{
}

void HoLine::draw(Drawer& d, const Color& color)
{
	d.draw_rectangle(x, y, w, 0.05f, color);
}

float rand_01()
{
	return (((float)rand()) / ((float)RAND_MAX));
}


