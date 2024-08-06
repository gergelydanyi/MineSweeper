#include "MapInfo.h"
#include "StateInfo.h"

MapInfo::MapInfo()
{
	Init();
}

void MapInfo::Init()
{
	for (int i = 0; i < sizeX; ++i)
	{
		for (int j = 0; j < sizeY; ++j)
		{
			(*this)(i, j).X = i;
			(*this)(i, j).Y = j;
			(*this)(i, j).fieldType = GetFieldType(i, j);
			(*this)(i, j).Parent = this;
			// TODO: set vector empty for each field (Field._neighbours)
			SetFieldNeighbourhood(i, j);
		}
	}
}

int MapInfo::Clear()
{
	for (int i = 0; i < Xmax; ++i)
	{
		for (int j = 0; j < Ymax; ++j)
		{
			(*this)(i, j).Reset();
		}
	}
	return 0;
}

void MapInfo::FieldChanged(Field* field)
{
	//Coordinate c = (*this)(field);
	Coordinate c = { field->X, field->Y };
	Parent->MapChanged(c);
	//(*this)(field);
}

Coordinate MapInfo::operator ()(Field* field)
{
	Coordinate c = { field->X, field->Y };
	return c;
	/*for (int i = 0; i < sizeX; ++i)
	{
		for (int j = 0; j < sizeY; ++j)
		{
			if (&(*this)(i, j) == field)
			{
				Coordinate c = { i, j };
				return c;
			}
		}
	}*/
}

Field& MapInfo::operator ()(short i, short j)
{
	return fields[i][j];
}

FieldType MapInfo::GetFieldType(short x, short y)
{
	if (x == 0)	{
		if (y == 0) {
			return FieldType::FT_TOPLEFT;
		}
		else if (y == sizeY - 1) {
			return FieldType::FT_BOTTOMLEFT;
		}
		else {
			return FieldType::FT_LEFT;
		}
	}
	else if (x == sizeX - 1) {
		if (y == 0) {
			return FieldType::FT_TOPRIGHT;
		}
		else if (y == sizeY - 1) {
			return FieldType::FT_BOTTOMRIGHT;
		}
		else {
			return FieldType::FT_RIGHT;
		}
	}
	else {
		if (y == 0) {
			return FieldType::FT_TOP;
		}
		else if (y == sizeY - 1) {
			return FieldType::FT_BOTTOM;
		}
		else {
			return FieldType::FT_CENTER;
		}
	}
}

void MapInfo::SetFieldNeighbourhood(short x, short y)
{
	(*this)(x, y).SetNeighbour(ND_RIGHT,		(*this)(x + 1, y));
	(*this)(x, y).SetNeighbour(ND_BOTTOMRIGHT,	(*this)(x + 1, y + 1));
	(*this)(x, y).SetNeighbour(ND_BOTTOM,		(*this)(x	 , y + 1));
	(*this)(x, y).SetNeighbour(ND_BOTTOMLEFT,	(*this)(x - 1, y + 1));
	(*this)(x, y).SetNeighbour(ND_LEFT,			(*this)(x - 1, y));
	(*this)(x, y).SetNeighbour(ND_TOPLEFT,		(*this)(x - 1, y - 1));
	(*this)(x, y).SetNeighbour(ND_TOP,			(*this)(x	 , y - 1));
	(*this)(x, y).SetNeighbour(ND_TOPRIGHT,		(*this)(x + 1, y - 1));
}

