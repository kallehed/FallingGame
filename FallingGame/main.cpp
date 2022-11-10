

#include "Layer.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
	Layer layer{};

	bool quit = false;
	
	while (!layer.start_frame() && !quit) {
		//stuff
		layer.end_frame();
		
	}

	

	return 0;
}