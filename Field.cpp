#include "Field.h"
#include "MapInfo.h"

Field::Field()
{
	//_neighbours = new std::vector<Field*>();
	value = 0;
}

Field::Field(int value)
{
	(*this).value = value;
}

Field::Field(FieldType fieldType)
{
	(*this).fieldType = fieldType;
	value = 0;
}

void Field::setChanged()
{
	value |= FV_Change;
	Parent->FieldChanged(this);
}

bool Field::hasMine()
{
	return value & FV_Mine;
}

void Field::setMine()
{
	if (!hasMine())
	{
		value |= FV_Mine;
		setChanged();
	}
}

bool Field::hasFlag()
{
	return value & FV_Flag;
}

void Field::switchFlag()
{
	value ^= FV_Flag;
	setChanged();
}

void Field::setFlag()
{
	value |= FV_Flag;
	setChanged();
	for (Field* field : Neighbours())
	{
		field->setChanged();
	}
}

bool Field::isClear()
{
	return value & FV_Unhidden;
}

void Field::setClear()
{
	if (!isClear())
	{
		value |= FV_Unhidden;
		setChanged();
	}
}

bool Field::isPushed()
{
	return value & FV_Pushed;
}

void Field::setPushed()
{
	if (!isPushed())
	{
		value |= FV_Pushed;
		setChanged();
	}
}

void Field::deletePushed()
{
	if (isPushed())
	{
		value &= ~FV_Pushed;
		setChanged();
	}
}

void Field::Reset()
{
	if (value)
	{
		fieldType = FT_EMPTY;
		for (int i = 0; i < 8; ++i)
		{
			neighbours[i] = 0;
		}
		_neighbours.clear();
		value = 0;

		setChanged();
	}
}

bool Field::Changed()
{
	if (value & FV_Change)
	{
		value ^= FV_Change;
		return true;
	}
	return false;
}

void Field::Push()
{
	if (!isClear() && !hasFlag())
	{
		setPushed();
	}
}

void Field::Release()
{
	deletePushed();
}

Field* Field::Neighbour(NeighbourDirection direction)
{
	return neighbours[direction];
}

std::vector<Field*> Field::Neighbours()
{
	if (_neighbours.empty())
	{
		for (int i = 0; i < 8; ++i)
		{
			if (neighbours[i] != 0)
			{
				_neighbours.push_back(neighbours[i]);
			}
		}
	}
	return _neighbours;
}

bool Field::Reveal(bool cascade)
{
	if (!isClear() && !hasFlag())
	{
		setClear();
		if (cascade && (SurroundingMines() == 0))
		{
			RevealNeighbours();
		}
	}
	return hasMine();
}

void Field::RevealNeighbours()
{
	for (Field* neighbour : Neighbours())
	{
		{
			neighbour->Reveal(true);
		}
	}
}

short Field::SurroundingMines()
{
	short i = 0;
	//for (Field* neighbour : neighbours)
	for (Field* neighbour : Neighbours())
	{
		if (/*neighbour != 0 && */neighbour->hasMine())
		{
			++i;
		}
	}
	return i;
}

short Field::SurroundingFlags()
{
	short flags = 0;
	for (Field* f : Neighbours())
	{
		if (f->hasFlag())
		{
			++flags;
		}
	}
	return flags;
}

short Field::SurroundingHiddenFields()
{
	short fields = 0;
	for (Field* f : Neighbours())
	{
		if (!f->isClear())
		{
			++fields;
		}
	}
	return fields;

}

short Field::SurroundingVisibleFields()
{
	short fields = 0;
	for (Field* f : Neighbours())
	{
		if (f->isClear())
		{
			++fields;
		}
	}
	return fields;

}

void Field::SetNeighbour(NeighbourDirection direction, Field& field)
{
	if ((neighbours[direction] == 0) && (GetNeighbourhoodType(direction) & fieldType))
	{
		neighbours[direction] = &field;
	}
}

NeighbourhoodType Field::GetNeighbourhoodType(NeighbourDirection direction)
{
	switch (direction)
	{
	case ND_RIGHT:
		return NT_RIGHT;
		break;
	case ND_BOTTOMRIGHT:
		return NT_BOTTOMRIGHT;
		break;
	case ND_BOTTOM:
		return NT_BOTTOM;
		break;
	case ND_BOTTOMLEFT:
		return NT_BOTTOMLEFT;
		break;
	case ND_LEFT:
		return NT_LEFT;
		break;
	case ND_TOPLEFT:
		return NT_TOPLEFT;
		break;
	case ND_TOP:
		return NT_TOP;
		break;
	case ND_TOPRIGHT:
		return NT_TOPRIGHT;
		break;
	}
}
