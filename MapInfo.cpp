#include "MapInfo.h"

MapInfo::MapInfo()
{
	int i = 0;
	for (auto field : fields)
	{
		field->v = ++i;
	}
}

int MapInfo::Clear()
{
	for (int i = 0; i < sizeX; ++i)
	{
		for (int j = 0; j < sizeY; ++j)
		{
			getField(i, j).reset();
		}
	}
	return 0;
}

