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
	
	while (!layer.start_frame() && !quit)
	{
		const float change = layer.dt * 3.f;
		if (layer.key_down(SDL_SCANCODE_D)) { x += change; }
		if (layer.key_down(SDL_SCANCODE_A)) { x -= change; }

		drawer.draw_rectangle(x, 0.25f + 0.5f*sin(SDL_GetTicks()*0.01f), 0.5f, 0.5f, {1.f,0.f,0.1f,1.f});
		//stuff
		layer.end_frame();
	}
	return 0;
}