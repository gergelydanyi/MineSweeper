#include "MapInfo.h"

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

