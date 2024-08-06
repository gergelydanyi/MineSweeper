#include "StateInfo.h"
#include "MapInfo.h"

GridInfo::GridInfo()
{
	gx = 20;
	gy = 40;
}

RECT GridInfo::GetRect(Coordinate c)
{
	long A = gx + c.X * w;
	long B = gy + c.Y * h;
	long C = A + w;
	long D = B + h;
	RECT r = { A, B, C, D };
	return r;
}

RECT GridInfo::GetRect(int x, int y)
{
	Coordinate c = { x, y };
	return GetRect(c);
}

StateInfo::StateInfo()
{
	map.Parent = this;
	map.Clear();
	activeField = &map(0, 0);
}

void StateInfo::InvalidateMap()
{
	RECT r_out = {};
	RECT r_1 = {};
	RECT r_2 = {};
	for (int i = 0; i < map.sizeX; ++i)
	{
		for (int j = 0; j < map.sizeY; ++j)
		{
			if (map(i, j).Changed())
			{
				Coordinate c = { i, j };
				r_2 = grid.GetRect(c);
				if (IsRectEmpty(&r_1))
				{
					r_1 = r_2;
				}
				UnionRect(&r_out, &r_1, &r_2);
				r_1 = r_out;
			}
		}
	}
	InvalidateRect(mainWindow, &r_out, TRUE);
}

void StateInfo::MapChanged(Coordinate c)
{
	RECT r = grid.GetRect(c);
	InvalidateRect(mainWindow, &r, TRUE);
}

void StateInfo::AutoFlag()
{
	for (int i = 0; i < map.sizeX; ++i)
	{
		for (int j = 0; j < map.sizeY; ++j)
		{
			Field* thisField = &map(i, j);
			if (!thisField->isClear() && !thisField->hasFlag())
			{
				for (Field* field : thisField->Neighbours())
				{
					if (field->SurroundingMines() == field->SurroundingHiddenFields())
					{
						thisField->setFlag();
						break;
					}
				}
			}
		}
	}
}

void StateInfo::AutoClick()
{
	for (int i = 0; i < map.sizeX; ++i)
	{
		for (int j = 0; j < map.sizeY; ++j)
		{
			if (!map(i, j).isClear() && map(i, j).SurroundingVisibleFields() > 0 && !map(i, j).hasFlag())
			{
				for (Field* field : map(i, j).Neighbours())
				{
					if (field->isClear() && field->SurroundingFlags() == field->SurroundingMines())
					{
						LeftButtonUp(i, j);
						break;
					}
				}
			}
		}
	}
}

Field* StateInfo::GetActiveField()
{
	if (mouse.X >= grid.gx && mouse.Y >= grid.gy)
	{
		int x = (mouse.X - grid.gx) / grid.w;
		int y = (mouse.Y - grid.gy) / grid.h;
		activeField = &map(x, y);
	}
	return activeField;
}

void StateInfo::OnMouseMove(LPARAM lParam)
{
	mouse.XP = mouse.X;
	mouse.YP = mouse.Y;
	mouse.X = GET_X_LPARAM(lParam);
	mouse.Y = GET_Y_LPARAM(lParam);
	if (mouse.X >= grid.gx && mouse.Y >= grid.gy)
	{
		int xp = (mouse.XP - grid.gx) / grid.w;
		int yp = (mouse.YP - grid.gy) / grid.h;
		int x = (mouse.X - grid.gx) / grid.w;
		int y = (mouse.Y - grid.gy) / grid.h;
		if (x != xp || y != yp)
		{
			RECT r = grid.GetRect(x, y);
			InvalidateRect(mainWindow, &r, TRUE);
			RECT r2 = grid.GetRect(xp, yp);
			InvalidateRect(mainWindow, &r2, TRUE);
		}
	}
}

void StateInfo::OnLeftButtonDown(LPARAM lParam)
{
	mouse.LDX = GET_X_LPARAM(lParam);
	mouse.LDY = GET_Y_LPARAM(lParam);
	if (!GO && mouse.LDX >= grid.gx && mouse.LDY >= grid.gy)
	{
		int dx = (mouse.LDX - grid.gx) / grid.w;
		int dy = (mouse.LDY - grid.gy) / grid.h;
		map(dx, dy).Push();
	}
}

void StateInfo::OnLeftButtonUp(LPARAM lParam)
{
	mouse.LUX = GET_X_LPARAM(lParam);
	mouse.LUY = GET_Y_LPARAM(lParam);
	int ux = (mouse.LUX - grid.gx) / grid.w;
	int uy = (mouse.LUY - grid.gy) / grid.h;
	int dx = (mouse.LDX - grid.gx) / grid.w;
	int dy = (mouse.LDY - grid.gy) / grid.h;
	if (mouse.LDX >= grid.gx && mouse.LDY >= grid.gy)
	{
		map(dx, dy).Release();
		map(ux, uy).Release();
		if (dx == ux && dy == uy && !map(ux, uy).hasFlag())// && !pState->GO)
		{
			LeftButtonUp(ux, uy);
		}
	}
	// EXPERIMENTAL
	 AutoFlag();
	// EXPERIMENTAL
	InvalidateMap();
}

void StateInfo::LeftButtonUp(int x, int y)
{
	if (NG)
	{
		NewGame(map.sizeX, map.sizeY, map.cMine, x + 1, y + 1);
		NG = false;
	}
	if (map(x, y).Reveal())
	{
		GameOver();
	}
	if (map(x, y).SurroundingFlags() == map(x, y).SurroundingMines())
	{
		map(x, y).RevealNeighbours();
	}
}

void StateInfo::OnRightButtonDown(LPARAM lParam)
{
	mouse.RDX = GET_X_LPARAM(lParam);
	mouse.RDY = GET_Y_LPARAM(lParam);
	if (!GO && mouse.RDX >= grid.gx && mouse.RDY >= grid.gy)
	{
		int dx = (mouse.RDX - grid.gx) / grid.w;
		int dy = (mouse.RDY - grid.gy) / grid.h;
		map(dx, dy).switchFlag();
		// EXPERIMENTAL
		// AutoFlag();
		// EXPERIMENTAL
	}
}

void StateInfo::OnRightButtonUp(LPARAM lParam)
{
	mouse.RUX = GET_X_LPARAM(lParam);
	mouse.RUY = GET_Y_LPARAM(lParam);
}

int StateInfo::NewGame(int sizeX, int sizeY, int cMine, int clickX, int clickY)
{
	GO = false;
	grid.cx = sizeX;
	grid.cy = sizeY;
	map.sizeX = sizeX;
	map.sizeY = sizeY;
	map.cMine = cMine;
	map.Clear();
	map.Init();
	int click = clickX * clickY;
	int mapsize = sizeX * sizeY;
	if (click > 1 && click < mapsize)
	{
		long f = (mapsize - 1) / (cMine - 1);
		int c1 = click / f;
		if (c1 >= cMine) { c1 = cMine - 1; }
		int c2 = cMine - c1 - 1;
		FillMap(-1, click, c1);
		FillMap(click, mapsize, c2);
	}
	else if (click == 1)
	{
		FillMap(0, mapsize, cMine);
	}
	else if (click == mapsize)
	{
		FillMap(-1, mapsize - 1, cMine);
	}
	InvalidateRect(mainWindow, NULL, TRUE);
	return 0;
}

/*

Future code for replacing the below function:

bool Probability(int a, int b)
{
	if (a == b) { return true; }
	return rand() < a * RAND_MAX / (double)b;
}

short GetMineRandomValue(short & missingMines, short & emptyFields)
{
	short r = Probability(missingMines, emptyFields--) ? 1 : 0;
	missingMines -= r;
	return r;
}

And example of use:

srand(time(0));
	short mines = 0;
	const short numberOfFields = 32767;
	const short numberOfRequiredMines = 327;
	short emptyFields;
	short missingMines;
	char fields[numberOfFields] = {};

	for (int k = 0; k < 100; ++k)
	{
		emptyFields = numberOfFields;
		missingMines = numberOfRequiredMines;
		for (int l = 0; l < numberOfFields; ++l)
		{
			GetMineRandomValue(missingMines, emptyFields);
		}
		std::cout << missingMines;
	}

*/

int StateInfo::FillMap(int a, int b, int cMine)
{
	int n = rand() % (b - a - 1) + a + 1;
	int nx = n / map.sizeY;
	int ny = n % map.sizeY;
	map(nx, ny).setMine();
	if (cMine > 1)
	{
		long f = (b - a - 2) / (cMine - 1);
		int c1 = (n - a - 1) / f;
		if (c1 >= cMine) { c1 = cMine - 1; }
		int c2 = cMine - c1 - 1;
		if (c1) { FillMap(a, n, c1); }
		if (c2) { FillMap(n, b, c2); }
	}
	return 0;
}

void StateInfo::GameOver()
{
	GO = true;
	for (int i = 0; i < map.sizeX; ++i)
	{
		for (int j = 0; j < map.sizeY; ++j)
		{
			if (map(i, j).hasMine())
			{
				map(i, j).setClear();
			}
			map(i, j).Reveal(false);
		}
	}
}