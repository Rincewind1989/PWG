#include <iostream>
#include "Town.h"

using namespace std;

struct Civilization{
	int yStart;
	int xStart;
	
	Races race;
	
	vector<Town> towns = vector<Town>(0);
	
	unsigned int population = 0;
	
	unsigned int colorsRFirst,colorsGFirst,colorsBFirst;
	unsigned int colorsRSecond,colorsGSecond,colorsBSecond;
	
	Civilization();
	Civilization(Tile &tile, unsigned int initialPopulation, unsigned int indexNumber);
	
	
	void processYear();
};


Civilization::Civilization()
{
	
	
	
}



Civilization::Civilization(Tile &tile, unsigned int initialPopulation, unsigned int indexNumber)
{
	this->xStart = tile.x;
	this->yStart = tile.y;
	
	this->race = Races(indexNumber%5);
	
	towns.push_back(Town(xStart, yStart, initialPopulation, race));
	
	random_device dev;
	mt19937 rng(dev());
	uniform_int_distribution<int> distInt(0, 255);
	colorsRFirst = distInt(rng);
	colorsGFirst = distInt(rng);
	colorsBFirst = distInt(rng);
	
	colorsRSecond = distInt(rng);
	colorsGSecond = distInt(rng);
	colorsBSecond = distInt(rng);
}

//Processes a full year of a civilization, like deciding to build a city e.g.
void Civilization::processYear()
{
	vector<Town> newTowns; 
	for (unsigned int i = 0; i < towns.size(); i++)
	{
		//cout << towns.size() << endl;
		towns[i].processYear();
		if (towns[i].checkForFoundingNewCity())
			newTowns.push_back(towns[i].foundNewCity());
	}
	
	towns.insert( towns.end(), newTowns.begin(), newTowns.end() );
}
