#pragma once
#include "Field.h"

struct Coordinate
{
	int X;
	int Y;
};

class StateInfo;

// This class comprises information about the gameboard and its fields
class MapInfo
{
	friend class StateInfo;
public:
	static const int Xmax = 100;
	static const int Ymax = 100;
	int sizeX = 15;														// Horizontal size of gameboard
	int sizeY = 20;														// Vertical size of gameboard
	int cMine = 10;														// Number of mines on gameboard
	MapInfo();
	int Clear();
	void FieldChanged(Field*);
	Coordinate operator ()(Field*);
	Field& operator ()(short, short);
private:
	void Init();
	StateInfo* Parent;
	Field fields[Xmax][Ymax];
	FieldType GetFieldType(short, short);
	void SetFieldNeighbourhood(short, short);
};