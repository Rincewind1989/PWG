#include <iostream>
#include <vector>
#include "Entity.h"

struct Town
{	
	Town ();
	Town (unsigned int x, unsigned int y, unsigned int initialPopulationRNG, Races race);
	Town (unsigned int x, unsigned int y, vector<Entity> &entities, Races race);
	
	unsigned int xPosition;
	unsigned int yPosition;
	
	Races race;
	
	unsigned int population = 0;
	float military = 0;
	float culture = 0;
	float health = 0;
	
	float environmentFactorGrowthPopulation = 1.1;
	float environmentFactorGrowthMilitary = 1.1;
	float environmentFactorGrowthCultural = 1.1;
	float environmentFactorGrowthHealth = 1.1;
	
	vector<Entity> entities = vector<Entity>(0);
	
	
	void updateEnvironmentInfluence();
	void processYear();
	bool checkForFoundingNewCity();
	Town foundNewCity();
};

Town::Town()
{
	
	
}


Town::Town(unsigned int x, unsigned int y, unsigned int initialPopulationRNG, Races race)
{
	this->xPosition = x;
	this->yPosition = y;
	
	this->race = race;
	
	for (unsigned int i = 0; i < initialPopulationRNG; i++)
	{
		entities.push_back(Entity(xPosition, yPosition, race));
	}
	
	population = entities.size();
	
}


Town::Town(unsigned int x, unsigned int y, vector<Entity> &entities, Races race)
{
	this->xPosition = x;
	this->yPosition = y;
	
	this->race = race;
	
	this->entities = entities;
	
	population = entities.size();
	
}


void Town::updateEnvironmentInfluence()
{
	
	
	
}


void Town::processYear()
{
	int tmoPopulationSave = population;
	population = population*environmentFactorGrowthPopulation;
	
	for (unsigned int i = 0; i < population - tmoPopulationSave; i++)
	{
		entities.push_back(Entity(xPosition, yPosition, race, 0));
	}
	
	military = population*environmentFactorGrowthMilitary;
	culture = population*environmentFactorGrowthCultural;
	health = population*environmentFactorGrowthHealth;
}


bool Town::checkForFoundingNewCity()
{
	random_device dev;
	mt19937 rng(dev());
	uniform_real_distribution<double> chanceDist(0, 1);
	float roll = chanceDist(rng);
	
	if (roll < exp(-100.0/population)){
		cout << "Founding a new city because " << roll << " is less than " << exp(-100.0/population) << endl;
		return true;
	}
	
	return false;
}


Town Town::foundNewCity()
{
	random_device dev;
	mt19937 rng(dev());
	uniform_int_distribution<int> dist(25, 50);
	uniform_int_distribution<int> dist2(0, 1);
	uniform_int_distribution<int> dist3(this->population/10, this->population/5);
	int xNew = dist(rng);
	int yNew = dist(rng);
	
	//Check for conditions on new tile
	
	//Choose randomly who moves to new city
	int newPopulation = dist3(rng);
	vector<Entity> newCitizens;
	cout << "Old city population before newly found " << this->entities.size() << endl;
	for (unsigned int i = 0; i < newPopulation; i++)
	{
		uniform_int_distribution<int> dist4(0, this->entities.size()-1);
		int selectRandomEntity = dist4(rng);
		newCitizens.push_back(entities[selectRandomEntity]);
		this->entities.erase(this->entities.begin() + selectRandomEntity);
	}
	this->population = this->entities.size();
	cout << "After new City " << this->entities.size() << endl;
	
	Town town(this->xPosition + xNew, this->yPosition + yNew, newCitizens, this->race);
	
	cout << "New city found with population " << newCitizens.size() << endl;
	
	return town;
}
