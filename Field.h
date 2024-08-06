#pragma once
#include <vector>
// Field value constants for individual areas on gameboard

// TODO: Consider removing these constants

constexpr auto FV_Mine = 0x0001;									// A mine is on the field
constexpr auto FV_Flag = 0x0002;									// The field is suggested to contain mine, a flag is drawn after right click
constexpr auto FV_Ask = 0x0004;									// Question mark is drawn after second right click (not yet ready)
constexpr auto FV_Unhidden = 0x1000;									// Field is cleared, wheter it contains mine or not
constexpr auto FV_Change = 0x2000;									// Used to indicate that the properties of the field was changed since last observation
constexpr auto FV_Pushed = 0x4000;									// Indicates if left button is down on a field

enum NeighbourDirection
{
	ND_RIGHT = 0,
	ND_BOTTOMRIGHT = 1,
	ND_BOTTOM = 2,
	ND_BOTTOMLEFT = 3,
	ND_LEFT = 4,
	ND_TOPLEFT = 5,
	ND_TOP = 6,
	ND_TOPRIGHT = 7
};

enum FieldType
{
	FT_EMPTY = 0,
	FT_TOPLEFT = 1,
	FT_TOP = 2,
	FT_TOPRIGHT = 4,
	FT_RIGHT = 8,
	FT_BOTTOMRIGHT = 16,
	FT_BOTTOM = 32,
	FT_BOTTOMLEFT = 64,
	FT_LEFT = 128,
	FT_CENTER = 256
};

enum NeighbourhoodType
{
	NT_RIGHT = ~(FieldType::FT_TOPRIGHT | FieldType::FT_RIGHT | FieldType::FT_BOTTOMRIGHT),
	NT_BOTTOM = ~(FieldType::FT_BOTTOMRIGHT | FieldType::FT_BOTTOM | FieldType::FT_BOTTOMLEFT),
	NT_LEFT = ~(FieldType::FT_TOPLEFT | FieldType::FT_LEFT | FieldType::FT_BOTTOMLEFT),
	NT_TOP = ~(FieldType::FT_TOPLEFT | FieldType::FT_TOP | FieldType::FT_TOPRIGHT),
	NT_BOTTOMRIGHT = NT_BOTTOM & NT_RIGHT,
	NT_BOTTOMLEFT = NT_BOTTOM & NT_LEFT,
	NT_TOPLEFT = NT_TOP & NT_LEFT,
	NT_TOPRIGHT = NT_TOP & NT_RIGHT
};

class MapInfo;

class Field
{
	friend class MapInfo;
public:
	int X;
	int Y;
	FieldType fieldType;

	Field();
	Field(int value);
	Field(FieldType fieldType);

	void setChanged();
	bool hasMine();
	void setMine();
	bool hasFlag();
	void switchFlag();
	void setFlag();
	bool isClear();
	void setClear();
	bool isPushed();
	void setPushed();
	void deletePushed();
	void Reset();
	// TODO: introduce Property class to use getting and setting properties in the same field
	bool Changed();
	void Push();
	void Release();
	Field* Neighbour(NeighbourDirection direction);
	std::vector<Field*> Neighbours();
	bool Reveal(bool cascade = true);
	void RevealNeighbours();
	short SurroundingMines();
	short SurroundingFlags();
	short SurroundingHiddenFields();
	short SurroundingVisibleFields();
private:
	MapInfo* Parent;
	Field* neighbours[8] = { 0 };
	std::vector<Field*> _neighbours;
	short value;

	void SetNeighbour(NeighbourDirection, Field&);
	NeighbourhoodType GetNeighbourhoodType(NeighbourDirection);

};

