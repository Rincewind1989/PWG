#include <cstdlib>
#include "Functions.h"

int main(int argc, char** argv)
{		
	World world(atoi(argv[2]));

	//Construct the world
	constructWorld(world);
	
	//Write History
	writeHistory(world);
	
	//Display the world
	displayWorld(world, argv[1]);

    return 0;
}
