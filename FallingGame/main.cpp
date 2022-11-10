#include "Layer.h"
#include "Drawer.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
	Layer layer{};

	bool quit = false;

	Drawer drawer(layer);

	float x = 0.f;
	
	while (!layer.start_frame() && !quit) {

		if (layer.key_down(SDL_SCANCODE_D)) { x += 0.01f; }
		if (layer.key_down(SDL_SCANCODE_A)) { x -= 0.01f; }

		drawer.draw_rectangle(x + std::sin(SDL_GetTicks() / 300.f), 0.25f, 0.5f, 0.5f, {125,0,34,255});
		//stuff
		layer.end_frame();
	}
	return 0;
}