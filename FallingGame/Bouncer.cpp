#include "Bouncer.h"

#include "Game.h"

Bouncer::Bouncer(float WIDTH, float y) : h((2.f*WIDTH - 0.4f) * rand_01() - WIDTH, y - 0.4f, 0.4f)
{
}

void Bouncer::logic(Game& g)
{
}