#include <iostream>

using namespace std;

struct Tile{
	int x;
	int y;
	double height = 0.0;
	double wind = 0.0;
	double heat = 0.0;
	double temperature = 0.0;
	double moisture = 0.0;
	double rain = 0.0;
	bool isUnderWater = false;
	bool hasSnow = false;
	bool isDesert = false;
	bool isForest = false;
	bool isHighland = false;
	bool isMountain = false;
	bool isOcean = false;
	bool isGlacier = false;
	int opacity = 200;
	
	void checksum();
};

void Tile::checksum()
{
	int tmp = isDesert + isForest + isHighland + isMountain;
	if (tmp > 1) {
		cout << "Checksum > 1, something went wrong. A landscape is more than one type!\n";
		cout << "Forest :" << isForest << ", Desert: " << isDesert << ", Highland: " << isHighland << ", Moutain: " << isMountain << endl;
	}
}
