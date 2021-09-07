#include <iostream>
#include <ctime>
#include <random>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include "World.h"
#include "FastNoise.h"

using namespace std;

//------------------------------------Create Basic World------------------------------//

//Simulate a filter for the edge of the world
void addFilterForEdge(World &world)
{
	cout << "Applying a filter to construct an edge of the world...\n";
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			if (x < world.X*world.edgeOfWorld)
				world.tiles[x][y].height -= (world.X*world.edgeOfWorld - x)/(world.X*world.edgeOfWorld)*world.maxHeightValue;
			if (x > world.X*(1.0 - world.edgeOfWorld))
				world.tiles[x][y].height -= (x - world.X*(1.0 - world.edgeOfWorld))/(world.X*world.edgeOfWorld)*world.maxHeightValue;
			if (y > world.Y*(1.0 - world.edgeOfWorld))
				world.tiles[x][y].height -= (y - world.Y*(1.0 - world.edgeOfWorld))/(world.Y*world.edgeOfWorld)*world.maxHeightValue;
			if (y < world.Y*world.edgeOfWorld)
				world.tiles[x][y].height -= (world.Y*world.edgeOfWorld - y)/(world.Y*world.edgeOfWorld)*world.maxHeightValue;
			if (world.tiles[x][y].height < world.minHeightValue)
				world.minHeightValue = world.tiles[x][y].height;
		}
	}
}

//Calculate the height map via a fractal noise
void getHeightMap(World &world)
{
	cout << "Calculating the height map via a fractal noise...\n";
	
	FastNoise myNoise; // Create a FastNoise object
	myNoise.SetFrequency(0.01);
	myNoise.SetFractalOctaves(6);
	myNoise.SetSeed(world.seed);
	myNoise.SetNoiseType(FastNoise::PerlinFractal); // Set the desired noise type

	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			world.tiles[x][y].height = myNoise.GetNoise(x,y);
			if (world.tiles[x][y].height > world.maxHeightValue)
				world.maxHeightValue = world.tiles[x][y].height;
			if (world.tiles[x][y].height < world.minHeightValue)
				world.minHeightValue = world.tiles[x][y].height;
		}
	}
	
	addFilterForEdge(world);
	
	//Normalize data
	world.maxHeightValue -= world.minHeightValue;
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			world.tiles[x][y].height -= world.minHeightValue;
			world.tiles[x][y].height /= world.maxHeightValue;
		}
	}
	world.minHeightValue = 0.0;
	world.maxHeightValue = 1.0;
}


//Calculate the opacity map via a fractal noise
void getOpacityMap(World &world)
{
	cout << "Calculating the opacity map for the visualisation...\n";
	
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			if (world.tiles[x][y].height > world.maxHeightValue*world.waterLevelPercentage)
				world.tiles[x][y].opacity = 255*(1.0 - 0.65*(world.tiles[x][y].height - world.maxHeightValue*world.waterLevelPercentage)/(world.maxHeightValue - world.maxHeightValue*world.waterLevelPercentage));
			else
				world.tiles[x][y].opacity = 255*(1.0 - 0.85*(world.maxHeightValue*world.waterLevelPercentage - world.tiles[x][y].height)/(world.maxHeightValue*world.waterLevelPercentage - world.minHeightValue));	
		}
	}
}


//Calculate the temperature map via a fractal noise
void getWindMap(World &world)
{
	cout << "Calculating the wind map via a perturbed fractal noise and consider influence of height...\n";
	
	FastNoise myNoise; // Create a FastNoise object
	myNoise.SetFrequency(0.015);
	myNoise.SetFractalOctaves(2);
	myNoise.SetSeed(world.seed);
	myNoise.SetNoiseType(FastNoise::Value); // Set the desired noise type
	myNoise.SetGradientPerturbAmp(45.0);

	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			float newX = (float)x;
			float newY = (float)y;
			myNoise.GradientPerturbFractal(newX, newY);
			world.tiles[x][y].wind = myNoise.GetNoise(newX,newY);
			if (world.tiles[x][y].wind > world.maxWindValue)
				world.maxWindValue = world.tiles[x][y].wind;
			if (world.tiles[x][y].wind < world.minWindValue)
				world.minWindValue = world.tiles[x][y].wind;
		}
	}

	//Normalize data
	world.maxWindValue -= world.minWindValue;
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			world.tiles[x][y].wind -= world.minWindValue;
			world.tiles[x][y].wind /= world.maxWindValue;
		}
	}
	world.minHeightValue = 0.0;
	world.maxWindValue = 1.0;
}



//Simulate a filter for the edge of the world
void addTemperatureFilter(World &world)
{
	
	FastNoise myNoise; // Create a FastNoise object
	myNoise.SetFrequency(0.004);
	myNoise.SetFractalOctaves(3);
	myNoise.SetSeed(world.seed);
	myNoise.SetNoiseType(FastNoise::Value); // Set the desired noise type
	myNoise.SetGradientPerturbAmp(35.0);
	
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			float newX = (float)x;
			float newY = (float)y;
			myNoise.GradientPerturbFractal(newX, newY);
			double tmp = myNoise.GetNoise(newX,newY) + 1.0 - 0.35*(world.Y - y)/(world.Y*world.edgeTemperatureMap);
			//if (y > world.Y*(1.0 - world.edgeTemperatureMap))
			if (tmp > 0)
			world.tiles[x][y].temperature -= tmp;
				//+ (y - world.Y*(1.0 - world.edgeTemperatureMap))/(world.Y*world.edgeTemperatureMap)*world.maxHeatValue;
			//if (y < world.Y*world.edgeOfWorld)
			//	world.tiles[x][y].heat -= (world.Y*world.edgeTemperatureMap - y)/(world.Y*world.edgeTemperatureMap)*world.maxHeatValue;
			if (world.tiles[x][y].temperature < world.minTemperatureValue)
				world.minTemperatureValue = world.tiles[x][y].temperature;
		}
	}
}


//Calculate the temperature map via a fractal noise
void getHeatMap(World &world)
{
	FastNoise myNoise; // Create a FastNoise object
	myNoise.SetFrequency(0.004);
	myNoise.SetFractalOctaves(3);
	myNoise.SetSeed(world.seed);
	myNoise.SetNoiseType(FastNoise::Value); // Set the desired noise type
	myNoise.SetGradientPerturbAmp(35.0);

	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			float newX = (float)x;
			float newY = (float)y;
			myNoise.GradientPerturbFractal(newX, newY);
			world.tiles[x][y].heat = myNoise.GetNoise(newX,newY);
			if (world.tiles[x][y].heat > world.maxHeatValue)
				world.maxHeatValue = world.tiles[x][y].heat;
			if (world.tiles[x][y].heat < world.minHeatValue)
				world.minHeatValue = world.tiles[x][y].heat;
		}
	}
	
	//Normalize data
	world.maxHeatValue -= world.minHeatValue;
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			world.tiles[x][y].heat -= world.minHeatValue;
			world.tiles[x][y].heat /= world.maxHeatValue;
		}
	}
	world.minHeatValue = 0.0;
	world.maxHeatValue = 1.0;
}


//Calculate the temperature map via a fractal noise
void getRainMap(World &world)
{
	
	cout << "Let it rain...\n";
	
	FastNoise myNoise; // Create a FastNoise object
	myNoise.SetFrequency(0.03);
	myNoise.SetFractalOctaves(2);
	myNoise.SetSeed(world.seed);
	myNoise.SetNoiseType(FastNoise::Value); // Set the desired noise type
	myNoise.SetGradientPerturbAmp(20.0);

	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			float newX = (float)x;
			float newY = (float)y;
			myNoise.GradientPerturbFractal(newX, newY);
			world.tiles[x][y].rain = myNoise.GetNoise(newX,newY);
			if (world.tiles[x][y].rain > world.maxRainValue)
				world.maxRainValue = world.tiles[x][y].rain;
			if (world.tiles[x][y].rain < world.minRainValue)
				world.minRainValue = world.tiles[x][y].rain;
		}
	}
	
	//Normalize data
	world.maxRainValue -= world.minRainValue;
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			world.tiles[x][y].rain -= world.minRainValue;
			world.tiles[x][y].rain /= world.maxRainValue;
		}
	}
	world.minRainValue = 0.0;
	world.maxRainValue = 1.0;
}


//Calculate the temperature map via a fractal noise
void getTemperatureMap(World &world)
{
	cout << "Calculating the temperature map via a perturbed value noise with wind and heat influence...\n";
	
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			world.tiles[x][y].temperature = world.tiles[x][y].heat - fabs(world.tiles[x][y].wind)*0.15 - fabs(world.tiles[x][y].height)*0.75 - fabs(world.tiles[x][y].rain)*0.2;
			if (world.tiles[x][y].isOcean)
				world.tiles[x][y].temperature += -0.5;
			if (world.tiles[x][y].temperature > world.maxTemperatureValue)
				world.maxTemperatureValue = world.tiles[x][y].temperature;
			if (world.tiles[x][y].temperature < world.minTemperatureValue)
				world.minTemperatureValue = world.tiles[x][y].temperature;
		}
	}
	
	addTemperatureFilter(world);
	
	//Normalize data
	world.maxTemperatureValue -= world.minTemperatureValue;
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			world.tiles[x][y].temperature -= world.minTemperatureValue;
			world.tiles[x][y].temperature /= world.maxTemperatureValue;
		}
	}
	world.minTemperatureValue = 0.0;
	world.maxTemperatureValue = 1.0;
}


//Calculate the temperature map via a fractal noise
void getMoistureMap(World &world)
{
	cout << "Calculating the moisture map...\n";
	
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			world.tiles[x][y].moisture = -world.tiles[x][y].temperature - world.tiles[x][y].height + 2.0*world.tiles[x][y].rain;

			if (world.tiles[x][y].moisture > world.maxMoistureValue)
				world.maxMoistureValue = world.tiles[x][y].moisture;
			if (world.tiles[x][y].moisture < world.minMoistureValue)
				world.minMoistureValue = world.tiles[x][y].moisture;
			if (world.tiles[x][y].moisture < world.minMoistureLandValue && !world.tiles[x][y].isOcean)
				world.minMoistureLandValue = world.tiles[x][y].moisture;
		}
	}
	
	//Normalize data
	world.maxMoistureValue -= world.minMoistureValue;
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			world.tiles[x][y].moisture -= world.minMoistureValue;
			world.tiles[x][y].moisture /= world.maxMoistureValue;
		}
	}
	world.minMoistureValue = 0.0;
	world.maxMoistureValue = 1.0;
	
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			//Add influence of rivers/sweet water
			if (world.tiles[x][y].isUnderWater && !world.tiles[x][y].isOcean)
			{
				for (int tmpX = -15; tmpX < 16; tmpX++)
				{
					for (int tmpY = -15; tmpY < 16; tmpY++)
					{
						if (x + tmpX < 0 || x + tmpX > world.X || y + tmpY < 0 || y + tmpY > world.Y)
							break;
							
						world.tiles[x + tmpX][y + tmpY].moisture += 0.001;
						if (world.tiles[x + tmpX][y + tmpY].moisture > world.maxMoistureValue)
							world.maxMoistureValue = world.tiles[x + tmpX][y + tmpY].moisture;
						
					}
				}
			}
			if (world.tiles[x][y].moisture > world.maxMoistureValue)
				world.maxMoistureValue = world.tiles[x][y].moisture;
			if (world.tiles[x][y].moisture < world.minMoistureValue)
				world.minMoistureValue = world.tiles[x][y].moisture;
			if (world.tiles[x][y].moisture < world.minMoistureLandValue && !world.tiles[x][y].isOcean)
				world.minMoistureLandValue = world.tiles[x][y].moisture;
		}
	}
	
	//Normalize data
	world.maxMoistureValue -= world.minMoistureValue;
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			world.tiles[x][y].moisture -= world.minMoistureValue;
			world.tiles[x][y].moisture /= world.maxMoistureValue;
		}
	}
	world.minMoistureValue = 0.0;
	world.maxMoistureValue = 1.0;
	
	
	
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			if (world.tiles[x][y].isOcean)
				world.tiles[x][y].moisture = world.minMoistureValue;
		}
	}
	
}


//Uses cellular noise to simulate plates
void applyTectonics(World &world)
{
	cout << "Simulating tectonic movement to build mountains...\n";
	
	random_device dev;
    mt19937 rng(dev());
    uniform_real_distribution<double> dist(0.0,1.0);
    uniform_int_distribution<int> distInt(10,25);
    uniform_int_distribution<int> distInt2(-15,15);
	
	FastNoise myNoise; // Create a FastNoise object
	myNoise.SetFrequency(0.005);
	myNoise.SetFractalOctaves(3);
	myNoise.SetSeed(world.seed);
	myNoise.SetNoiseType(FastNoise::Cellular); // Set the desired noise type

	vector<vector<double>> plates = vector<vector<double>>(world.X, vector<double>(world.Y));
	vector<vector<double>> changes = vector<vector<double>>(world.X, vector<double>(world.Y, 0.0));
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			plates[x][y] = myNoise.GetNoise(x,y);
		}
	}
	
	//Simulate a few millions years. Right now do it in a very simple way
	for (int x = 1; x < world.X - 1; x++)
	{
		for (int y = 1; y < world.Y - 1; y++)
		{
			for (int i = -1; i < 2; i++)
			{
				for (int j = -1; j < 2; j++)
				{
					if (j == 0 && i == 0)
						continue;
					changes[x][y] += (plates[x][y] - plates[x + i][y + j]);
				}
			}
		}
	}
	int iEnd = distInt(rng);
	int jEnd = distInt(rng);
	int xShift = distInt2(rng);
	int yShift = distInt2(rng);
	for (int x = 50; x < world.X - 50; x++)
	{
		for (int y = 50; y < world.Y - 50; y++)
		{
			if (changes[x][y] == 0)
				continue;
			if (dist(rng) < 0.05)
			{
				xShift = distInt2(rng);
				yShift = distInt2(rng);
				iEnd = distInt(rng);
				jEnd = distInt(rng);
				for (int i = -distInt(rng); i < iEnd; i++)
				{
					for (int j = -distInt(rng); j < jEnd; j++)
					{
						if (j == 0 && i == 0)
							continue;
						world.tiles[x + xShift + i][y + yShift + j].height += changes[x][y]*plates[x][y]*0.1*(1.0/(double(i)*double(i) + double(j)*double(j)));
						//cout << "i=" << i << ", j=" << j << ", change=" << changes[x][y]*plates[x][y]*0.005*(1.0/(double(i)*double(i) + double(j)*double(j))) << endl;
						if (world.tiles[x + xShift + i][y + yShift + j].height > world.maxHeightValue)
							world.maxHeightValue = world.tiles[x + xShift + i][y + yShift + j].height;
						if (world.tiles[x + xShift + i][y + yShift + j].height < world.minHeightValue)
							world.minHeightValue = world.tiles[x + xShift + i][y + yShift + j].height;
					}
				}
			}
		}
	}
}


//Filling world with oceans, seas and rivers
void fillInWater(World &world)
{
	cout << "Filling world with water...\n";
	
	//Calculte water level for the oceans
	world.waterLevel = (world.maxHeightValue - world.minHeightValue)*world.waterLevelPercentage + world.minHeightValue;
	
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			if (world.tiles[x][y].height < world.waterLevel)
			{
				world.tiles[x][y].isUnderWater = true;
				world.tiles[x][y].isOcean = true;
				world.seaMass += 1;
				world.tiles[x][y].checksum();
			}
		}
	}
	
	world.seaMassInPercentage = double(world.seaMass)/(double(world.X) * double(world.Y));
	world.landMassInPercentage = double(world.landMass)/(double(world.X) * double(world.Y));
	
	//------Calculte some rivers--------
	
	//Find positions for rivers
	random_device dev;
    mt19937 rng(dev());
    uniform_real_distribution<double> dist(0.0,1.0);
    uniform_int_distribution<int> distInt(int(double(world.mountains.size())/100.0*0.4) + 2, int(double(world.mountains.size())/100.0*0.6) + 5);
    
    int numberRivers = distInt(rng);
    for (int i = 0; i < numberRivers; i++)
    {
		uniform_int_distribution<int> distInt(0, world.mountains.size() - 1);
		int roll = distInt(rng);
		bool riverAlreadyInVector = false;
		
		for (int j = 0; j < world.rivers.size(); j++)
		{
			if ((abs(world.rivers[j].x - world.mountains[roll].x)*abs(world.rivers[j].x - world.mountains[roll].x) + abs(world.rivers[j].y - world.mountains[roll].y)*abs(world.rivers[j].y - world.mountains[roll].y)) < world.distanceBetweenRivers)
				riverAlreadyInVector = true;
		}
		
		if (riverAlreadyInVector){
			i--;
			continue;
		}
		else
		{
			world.rivers.push_back(world.mountains[roll]);
		}
	}
	
	
	//Follow a gradient path for the river
	for (int i = 0; i < world.rivers.size(); i++)
	{
		world.tiles[world.rivers[i].x][world.rivers[i].y].isUnderWater = true;
		bool oceanFound = false;
		int tempX = world.rivers[i].x;
		int tempY = world.rivers[i].y;
		while (!oceanFound)
		{
			double smallestHeight = MAXFLOAT;
			int foundX = 0;
			int foundY = 0;
			for (int x = -1; x < 2; x++)
			{
				for (int y = -1; y < 2; y++)
				{
					if (x == 0 && y == 0)
						continue;

                    if (tempX+x > world.X || tempY+y > world.Y || tempX+x < 0 || tempY+y < 0)
                        break;
						
					if (world.tiles[tempX + x][tempY + y].isOcean) {
						oceanFound = true;
						break;
					}
					
					if (world.tiles[tempX + x][tempY + y].height < smallestHeight && !world.tiles[tempX + x][tempY + y].isUnderWater && world.tiles[tempX + x][tempY + y].height < world.rivers[i].height)
					{
						foundX = x;
						foundY = y;
						smallestHeight = world.tiles[tempX + x][tempY + y].height;
					}
				}
				if (oceanFound)
					break;
			}
			if (foundX == 0 && foundY == 0)
				break;
			tempX = tempX + foundX;
			tempY = tempY + foundY;
			world.tiles[tempX][tempY].isUnderWater = true;
		}
	}
	
	cout << "Created " << world.rivers.size() << " rivers in the world\n";
	
    
}



//Filling world with oceans, seas and rivers
void detectMountains(World &world)
{
	cout << "Raising mountains...\n";
	
	//Calculte water level for the oceans
	world.mountainLevel = (world.maxHeightValue - world.minHeightValue)*world.mountainLevelPercentage + world.minHeightValue;
	
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			if (world.tiles[x][y].height > world.mountainLevel)
			{
				world.tiles[x][y].isMountain = true;
				world.mountains.push_back(world.tiles[x][y]);
				world.tiles[x][y].checksum();
			}
		}
	}
	
	world.seaMassInPercentage = double(world.seaMass)/(double(world.X) * double(world.Y));
	world.landMassInPercentage = double(world.landMass)/(double(world.X) * double(world.Y));
}


//Filling world with oceans, seas and rivers
void growForests(World &world)
{
	cout << "Planting forests...\n";
	
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			if (!world.tiles[x][y].isMountain && !world.tiles[x][y].isUnderWater)
			{
				//cout << "Moisture of tile:" << world.tiles[x][y].moisture << ", MaxMoisture:" << world.maxMoistureValue << ", MinMoisture:" << world.minMoistureValue << ", MinMoistureLandValue:" << world.minMoistureLandValue << endl;
				if (world.tiles[x][y].moisture > (world.maxMoistureValue - world.minMoistureLandValue)*world.forestPercentage)
				{
					world.tiles[x][y].isForest = true;
					world.tiles[x][y].isHighland = false;
				}
				else
				{
					if (!world.tiles[x][y].isForest)
					{
						world.tiles[x][y].isHighland = true;
						world.highlands.push_back(world.tiles[x][y]);
						world.tiles[x][y].checksum();
					}
				}
			}
		}
	}
}


//Filling world with oceans, seas and rivers
void checkForDeserts(World &world)
{
	cout << "Checking for deserts...\n";
	
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			if (!world.tiles[x][y].isMountain && !world.tiles[x][y].isUnderWater && !world.tiles[x][y].isForest)
			{
				if (world.tiles[x][y].moisture < world.isDesertMoisture*world.maxMoistureValue && world.tiles[x][y].temperature > world.isDesertTemperature*world.maxTemperatureValue)
				{
					world.tiles[x][y].isDesert = true;
					world.tiles[x][y].isHighland = false;
				}
			}
		}
	}
}



//Filling world with oceans, seas and rivers
void checkForSnow(World &world)
{
	cout << "Checking for cold places...\n";
	
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			if (!world.tiles[x][y].isUnderWater)
			{
				if (world.tiles[x][y].temperature < world.isSnowTemperature*world.maxTemperatureValue)
				{
					world.tiles[x][y].hasSnow = true;
				}
			}
		}
	}
}


//Filling world with oceans, seas and rivers
void checkForGlacier(World &world)
{
	cout << "Checking for glaciers...\n";
	
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			if (world.tiles[x][y].temperature < world.isGlacierTemperature*world.maxTemperatureValue)
			{
				world.tiles[x][y].isGlacier = true;
			}
		}
	}
}


//Calculate the temperature map via a fractal noise
void calcTemperatureMap(World &world)
{	
	getWindMap(world);
	getHeatMap(world);
	getRainMap(world);
	getTemperatureMap(world);
}


//Construct world map
void constructWorld(World &world)
{
	getHeightMap(world);
	detectMountains(world);
	fillInWater(world);
	getOpacityMap(world);
	calcTemperatureMap(world);
	getMoistureMap(world);
	growForests(world);
	checkForDeserts(world);
	checkForSnow(world);
	checkForGlacier(world);
}



//------------------------------------Create Civilizations------------------------------//


enum Beasts {Golem, Dragon, Giant, Cyclops, Elemental};

void spawnCivilizations(World &world)
{
	cout << "Spawning Civilizations!\n";
	
	unsigned int numberCivilizations = (world.X*world.Y)/world.density;
	random_device dev;
	mt19937 rng(dev());
	uniform_real_distribution<double> dist(0.0,1.0);
		
	for (unsigned int i = 0; i < numberCivilizations; i++)
	{			
		uniform_int_distribution<int> distInt(0, world.tiles.size()*world.tiles[0].size() - 1);
		int roll = distInt(rng);
		bool civilizationAlreadyInVector = false;
		
		int xTmp = int(roll/world.tiles[0].size());
		int yTmp = roll % world.tiles.size();
		
		if (world.tiles[xTmp][yTmp].isUnderWater){
			i--;
			continue;
		}
	
		for (int j = 0; j < world.civilizations.size(); j++)
		{
			if (((world.civilizations[j].xStart - world.tiles[xTmp][yTmp].x)*(world.civilizations[j].xStart - world.tiles[xTmp][yTmp].x) + (world.civilizations[j].yStart - world.tiles[xTmp][yTmp].y)*(world.civilizations[j].yStart- world.tiles[xTmp][yTmp].y)) < world.distanceBetweenCivilizations*world.distanceBetweenCivilizations)
				civilizationAlreadyInVector = true;
		}
		
		if (civilizationAlreadyInVector){
			i--;
			continue;
		}
		else
		{
			world.civilizations.push_back(Civilization(world.tiles[xTmp][yTmp], world.initialPopulation,i));
		}
	}
}


void processCivilizations(World &world)
{
	cout << "Processing a year...\n";
	for (unsigned int i = 0; i < world.historyYears; i++)
	{
		for (unsigned int j = 0; j < world.civilizations.size(); j++)
		{
			world.civilizations[j].processYear();
		}
	}
	
}



void writeHistory(World &world)
{
	spawnCivilizations(world);
	
	
}

//---------------------------------------------Graphic Interface-------------------------------------//


void displayWorld(World &world, sf::RenderWindow &window)
{
	sf::RectangleShape rectangle;
	rectangle.setSize(sf::Vector2f(1, 1));
	double tmp;
	for (int x = 0; x < world.X; x++)
	{
		for (int y = 0; y < world.Y; y++)
		{
			
			rectangle.setPosition(x, y);
			
			//Set map point default to pink to detect errors
			rectangle.setFillColor(sf::Color(217, 124, 114));
			
			if (world.tiles[x][y].isDesert)
				rectangle.setFillColor(sf::Color(255, 255, 0, world.tiles[x][y].opacity));
			if (world.tiles[x][y].isForest)
				rectangle.setFillColor(sf::Color(0, 75, 0, world.tiles[x][y].opacity));
			if (world.tiles[x][y].isMountain)
				rectangle.setFillColor(sf::Color(100, 75, 0, world.tiles[x][y].opacity));			
			if (world.tiles[x][y].isHighland)
				rectangle.setFillColor(sf::Color(0, 255, 0, world.tiles[x][y].opacity));					
			if (world.tiles[x][y].hasSnow)
				rectangle.setFillColor(sf::Color(188, 188, 188, world.tiles[x][y].opacity));
			if (world.tiles[x][y].isUnderWater)
				rectangle.setFillColor(sf::Color(0,0,255, world.tiles[x][y].opacity));
			if (world.tiles[x][y].isGlacier)
				rectangle.setFillColor(sf::Color(255, 255, 255, world.tiles[x][y].opacity));
					

			window.draw(rectangle);

		}
	}
	
	
	sf::CircleShape shape(5);
	shape.setOutlineThickness(4);
	sf::CircleShape square(4, 4);
	square.setOutlineThickness(3);
	
	for (unsigned int i = 0; i < world.civilizations.size(); i++)
	{
		shape.setPosition(world.civilizations[i].xStart, world.civilizations[i].yStart);
		shape.setFillColor(sf::Color(world.civilizations[i].colorsRFirst, world.civilizations[i].colorsGFirst, world.civilizations[i].colorsBFirst));
		shape.setOutlineColor(sf::Color(world.civilizations[i].colorsRSecond, world.civilizations[i].colorsGSecond, world.civilizations[i].colorsBSecond));
		//window.draw(shape);
		for (unsigned int j = 0; j < world.civilizations[i].towns.size(); j++)
		{
			square.setPosition(world.civilizations[i].towns[j].xPosition, world.civilizations[i].towns[j].yPosition);
			square.setFillColor(sf::Color(world.civilizations[i].colorsRFirst, world.civilizations[i].colorsGFirst, world.civilizations[i].colorsBFirst));
			square.setOutlineColor(sf::Color(world.civilizations[i].colorsRSecond, world.civilizations[i].colorsGSecond, world.civilizations[i].colorsBSecond));
			window.draw(square);
		}
	}
	
}




void displayWorld(World &world, string type)
{
	//Render the window
	cout << "Rendering the visualisation...\n";
	sf::RenderWindow window(sf::VideoMode(world.X, world.Y), type);
    while (window.isOpen())
    {
        
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
                
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				processCivilizations(world);
			}	
        }

        window.clear();
        sf::RectangleShape rectangle;
        rectangle.setSize(sf::Vector2f(1, 1));
        double tmp;
        
        if (type == "height")
        {
			for (int x = 0; x < world.X; x++)
			{
				for (int y = 0; y < world.Y; y++)
				{
					tmp = (world.tiles[x][y].height - world.minHeightValue*1.00001)/((world.maxHeightValue - world.minHeightValue)*1.0001)*255.0;
					if (tmp < 0.0 or tmp > 255.0)
						cout << tmp << " " << world.tiles[x][y].height << " " << world.minHeightValue << " " << world.maxHeightValue << endl;
					rectangle.setPosition(x, y);
					rectangle.setFillColor(sf::Color(255,255,255, tmp));
					window.draw(rectangle);

				}
			}
		}
		
		if (type == "wind")
        {
			for (int x = 0; x < world.X; x++)
			{
				for (int y = 0; y < world.Y; y++)
				{
					tmp = (world.tiles[x][y].wind - world.minWindValue*1.00001)/((world.maxWindValue - world.minWindValue)*1.0001)*255.0;
					if (tmp < 0.0 or tmp > 255.0)
						cout << tmp << " " << world.tiles[x][y].wind << " " << world.minHeightValue << " " << world.maxWindValue << endl;
					rectangle.setPosition(x, y);
					rectangle.setFillColor(sf::Color(255,255,255, tmp));
					window.draw(rectangle);

				}
			}
		}
		
		if (type == "heat")
        {
			for (int x = 0; x < world.X; x++)
			{
				for (int y = 0; y < world.Y; y++)
				{
					tmp = (world.tiles[x][y].heat - world.minHeatValue*1.00001)/((world.maxHeatValue - world.minHeatValue)*1.0001)*255.0;
					if (tmp < 0.0 or tmp > 255.0)
						cout << tmp << " " << world.tiles[x][y].heat << " " << world.minHeatValue << " " << world.maxHeatValue << endl;
					rectangle.setPosition(x, y);
					rectangle.setFillColor(sf::Color(255,255,255, tmp));
					window.draw(rectangle);

				}
			}
		}
		
		if (type == "temperature")
        {
			for (int x = 0; x < world.X; x++)
			{
				for (int y = 0; y < world.Y; y++)
				{
					tmp = (world.tiles[x][y].temperature - world.minTemperatureValue*1.00001)/((world.maxTemperatureValue - world.minTemperatureValue)*1.0001)*255.0;
					if (tmp < 0.0 or tmp > 255.0)
						cout << tmp << " " << world.tiles[x][y].temperature << " " << world.minTemperatureValue << " " << world.maxTemperatureValue << endl;
					rectangle.setPosition(x, y);
					rectangle.setFillColor(sf::Color(255,255,255, tmp));
					window.draw(rectangle);

				}
			}
		}
		
		if (type == "rain")
        {
			for (int x = 0; x < world.X; x++)
			{
				for (int y = 0; y < world.Y; y++)
				{
					tmp = (world.tiles[x][y].rain - world.minRainValue*1.00001)/((world.maxRainValue - world.minRainValue)*1.0001)*255.0;
					if (tmp < 0.0 or tmp > 255.0)
						cout << tmp << " " << world.tiles[x][y].rain << " " << world.minRainValue << " " << world.maxRainValue << endl;
					rectangle.setPosition(x, y);
					rectangle.setFillColor(sf::Color(255,255,255, tmp));
					window.draw(rectangle);

				}
			}
		}
		
		if (type == "moisture")
        {
			for (int x = 0; x < world.X; x++)
			{
				for (int y = 0; y < world.Y; y++)
				{
					tmp = (world.tiles[x][y].moisture - world.minMoistureValue*1.00001)/((world.maxMoistureValue - world.minMoistureValue)*1.0001)*255.0;
					if (tmp < 0.0 or tmp > 255.0)
						cout << tmp << " " << world.tiles[x][y].moisture << " " << world.minMoistureValue << " " << world.maxMoistureValue << endl;
					rectangle.setPosition(x, y);
					rectangle.setFillColor(sf::Color(255,255,255, tmp));
					window.draw(rectangle);

				}
			}
		}
		
		if (type == "full")
			displayWorld(world, window);
		
        window.display();
    }
}
