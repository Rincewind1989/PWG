#include <iostream>
#include <string>
#include <math.h>

using namespace std;

enum Races {Humans, Orcs, Elves, Dwarfs, Halflings};

struct Entity
{
	Entity();
	Entity(unsigned int x, unsigned int y, Races race);
	Entity(unsigned int x, unsigned int y, Races race, unsigned int age);
	
	unsigned int xPosition = 0;
	unsigned int yPosition = 0;
	
	string name = "Default";
	
	unsigned int age = 0;
	
	Races race;
};

Entity::Entity()
{

}


Entity::Entity(unsigned int x, unsigned int y, Races race)
{
	random_device dev;
	mt19937 rng(dev());
	uniform_int_distribution<int> ageDist(5, 50);
	age = ageDist(rng);
	
	this->xPosition = x;
	this->yPosition = y;
	
	this->race = race;
}


Entity::Entity(unsigned int x, unsigned int y, Races race, unsigned int age)
{
	random_device dev;
	mt19937 rng(dev());
	uniform_int_distribution<int> ageDist(5, 50);
	this->age = age;
	
	this->xPosition = x;
	this->yPosition = y;
	
	this->race = race;
}

