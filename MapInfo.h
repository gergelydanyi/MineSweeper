#pragma once
#include "Field.h"

// This class comprises information about the gameboard and its fields
class MapInfo
{
public:
	int Clear();
	Field& getField(short int x, short int y) { return field[x][y]; }
	const int Xmax = 100;
	const int Ymax = 100;
	int sizeX = 15;														// Horizontal size of gameboard
	int sizeY = 20;														// Vertical size of gameboard
	int cMine = 10;														// Number of mines on gameboard
private:
	Field field[100][100];
};

class MapInfo_new
{
};

