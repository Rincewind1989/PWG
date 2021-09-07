#include <vector>
#include <time.h>
#include "Tile.h"
#include "Civilization.h"

using namespace std;

struct World{
	World();
	World(int seed_);
	
	//Set parameters for world
	double edgeOfWorld = 0.2;
	double waterLevelPercentage = 0.6;	//Under this Percentage the world is filled with oceans
	double mountainLevelPercentage = 0.80; //Above this percentage, a height is considered a mountain
	double forestPercentage = 0.12;	//Percentage of the land minus the mountains, that is filled with forests
	double edgeTemperatureMap = 0.25;
	double isDesertMoisture = 0.15;
	double isDesertTemperature = 0.2;
	double isSnowTemperature = 0.25;
	double isGlacierTemperature = 0.10;
	int distanceBetweenRivers = 16;
	int distanceBetweenMountains = 16;
	int X = 1024;
	int Y = 780;
	int seed = time(0);
	unsigned int historyYears = 1;
	
	//Set parameters for civilizations
	unsigned int density = 25000;
	int distanceBetweenCivilizations = 35;
	unsigned int initialPopulation = 20;
	
	//Max and min height values
	double maxHeightValue = -99999.0;
	double minHeightValue = 99999.0;
	
	//Max and min wind values
	double maxWindValue = -99999.0;
	double minWindValue = 99999.0;
	
	//Max and min heat values
	double maxHeatValue = -99999.0;
	double minHeatValue = 99999.0;
	
	//Max and min temperature values
	double maxRainValue = -99999.0;
	double minRainValue = 99999.0;
	
	//Max and min temperature values
	double maxTemperatureValue = -99999.0;
	double minTemperatureValue = 99999.0;
	
	//Max and min temperature values
	double maxMoistureValue = -99999.0;
	double minMoistureValue = 99999.0;
	double minMoistureLandValue = 99999.0;
	
	//Map properties
	int landMass = 0;
	int seaMass = 0;
	double landMassInPercentage = 1.0;
	double seaMassInPercentage = 0.0;
	double waterLevel = 0.0;
	double mountainLevel = 0.0;
	



	vector<vector<Tile>> tiles = vector<vector<Tile>>(X, vector<Tile>(Y));
	vector<Tile> mountains = vector<Tile>(0);
	vector<Tile> rivers = vector<Tile>(0);
	vector<Tile> forests = vector<Tile>(0);
	vector<Tile> highlands = vector<Tile>(0);
	
	vector<Civilization> civilizations = vector<Civilization>(0);
};


World::World()
{
	for (int i = 0; i < tiles.size(); i++)
	{
		for (int j = 0; j < tiles[i].size(); j++)
		{
			tiles[i][j].x = i;
			tiles[i][j].y = j;
		}
	}
}


World::World(int seed_)
{
	seed = seed_;
	for (int i = 0; i < tiles.size(); i++)
	{
		for (int j = 0; j < tiles[i].size(); j++)
		{
			tiles[i][j].x = i;
			tiles[i][j].y = j;
		}
	}
}
