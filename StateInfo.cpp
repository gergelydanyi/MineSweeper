#include "StateInfo.h"
#include "MapInfo.h"
//#include "Field.h"

void StateInfo::OnMouseMove(LPARAM lParam)
{
	mouse.XP = mouse.X;
	mouse.YP = mouse.Y;
	mouse.X = GET_X_LPARAM(lParam);
	mouse.Y = GET_Y_LPARAM(lParam);
}

void StateInfo::OnLeftButtonDown(HWND hwnd, LPARAM lParam)
{
	mouse.LDX = GET_X_LPARAM(lParam);
	mouse.LDY = GET_Y_LPARAM(lParam);
	if (!GO && mouse.LDX >= grid.gx && mouse.LDY >= grid.gy)
	{
		int dx = (mouse.LDX - grid.gx) / grid.w;
		int dy = (mouse.LDY - grid.gy) / grid.h;
		if (!map.getField(dx, dy).isClear() && !map.getField(dx, dy).hasFlag())
		{
			map.getField(dx, dy).setPushed();
		}
		RECT r = {
			{grid.gx + dx * grid.w},
			{grid.gy + dy * grid.h},
			{grid.gx + ++dx * grid.w},
			{grid.gy + ++dy * grid.h} };
		InvalidateRect(hwnd, &r, TRUE);
	}
}

void StateInfo::OnLeftButtonUp(HWND hwnd, LPARAM lParam)
{
	mouse.LUX = GET_X_LPARAM(lParam);
	mouse.LUY = GET_Y_LPARAM(lParam);
	int ux = (mouse.LUX - grid.gx) / grid.w;
	int uy = (mouse.LUY - grid.gy) / grid.h;
	int dx = (mouse.LDX - grid.gx) / grid.w;
	int dy = (mouse.LDY - grid.gy) / grid.h;
	if (mouse.LDX >= grid.gx && mouse.LDY >= grid.gy)
	{
		map.getField(dx, dy).deletePushed();
		map.getField(ux, uy).deletePushed();
		if (dx == ux && dy == uy && !map.getField(ux, uy).hasFlag())// && !pState->GO)
		{
			if (NG)
			{
				NewGame(hwnd, map.sizeX, map.sizeY, map.cMine, ux + 1, uy + 1);
				NG = false;
			}
			if (UnhideField(hwnd, ux, uy))
			{
				GameOver(hwnd);
			}
		}
		else
		{
			RECT r = { {grid.gx + dx * grid.w},	{grid.gy + dy * grid.h}, {grid.gx + ++dx * grid.w}, {grid.gy + ++dy * grid.h} };
			InvalidateRect(hwnd, &r, TRUE);
		}
	}
}

void StateInfo::OnRightButtonDown(HWND hwnd, LPARAM lParam)
{
	mouse.RDX = GET_X_LPARAM(lParam);
	mouse.RDY = GET_Y_LPARAM(lParam);
	if (!GO)
	{
		int dx = (mouse.RDX - grid.gx) / grid.w;
		int dy = (mouse.RDY - grid.gy) / grid.h;
		map.getField(dx, dy).switchFlag();
		RECT r = {
			{grid.gx + dx * grid.w},
			{grid.gy + dy * grid.h},
			{grid.gx + ++dx * grid.w},
			{grid.gy + ++dy * grid.h} };
		InvalidateRect(hwnd, &r, TRUE);
	}
}

void StateInfo::OnRightButtonUp(LPARAM lParam)
{
	mouse.RUX = GET_X_LPARAM(lParam);
	mouse.RUY = GET_Y_LPARAM(lParam);
}

int StateInfo::NewGame(HWND hwnd, int sizeX, int sizeY, int cMine, int clickX, int clickY)
{
	GO = false;
	grid.cx = sizeX;
	grid.cy = sizeY;
	map.sizeX = sizeX;
	map.sizeY = sizeY;
	map.cMine = cMine;
	map.Clear();
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
	FillMapWithNumbers();
	InvalidateRect(hwnd, NULL, TRUE);
	return 0;
}

int StateInfo::FillMap(int a, int b, int cMine)
{
	int n = rand() % (b - a - 1) + a + 1;
	int nx = n / map.sizeY;
	int ny = n % map.sizeY;
	map.getField(nx, ny).setMine();
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

int StateInfo::FillMapWithNumbers()
{
	int cm = 0;
	for (int i = 0; i < map.sizeX; ++i)
	{
		for (int j = 0; j < map.sizeY; ++j)
		{
			cm = 0;
			if (!map.getField(i, j).hasMine())
			{
				if (i > 0 && j > 0 && map.getField(i - 1, j - 1).hasMine()) { ++cm; }
				if (j > 0 && map.getField(i, j - 1).hasMine()) { ++cm; }
				if (i < map.sizeX - 1 && j > 0 && map.getField(i + 1, j - 1).hasMine()) { ++cm; }
				if (i < map.sizeX - 1 && map.getField(i + 1, j).hasMine()) { ++cm; }
				if (i < map.sizeX - 1 && j < map.sizeY - 1 && map.getField(i + 1, j + 1).hasMine()) { ++cm; }
				if (j < map.sizeY - 1 && map.getField(i, j + 1).hasMine()) { ++cm; }
				if (i > 0 && j < map.sizeY - 1 && map.getField(i - 1, j + 1).hasMine()) { ++cm; }
				if (i > 0 && map.getField(i - 1, j).hasMine()) { ++cm; }
				switch (cm)
				{
				case 0:map.getField(i, j).set0(); break;
				case 1:map.getField(i, j).set1(); break;
				case 2:map.getField(i, j).set2(); break;
				case 3:map.getField(i, j).set3(); break;
				case 4:map.getField(i, j).set4(); break;
				case 5:map.getField(i, j).set5(); break;
				case 6:map.getField(i, j).set6(); break;
				case 7:map.getField(i, j).set7(); break;
				case 8:map.getField(i, j).set8(); break;
				}
			}
		}
	}
	return 0;
}

int StateInfo::UnhideField(HWND hwnd, int i, int j)
{
	if (!map.getField(i, j).hasFlag() && !map.getField(i, j).isClear())
	{
		RECT r = {
			{grid.gx + i * grid.w},
			{grid.gy + j * grid.h},
			{grid.gx + (i + 1) * grid.w},
			{grid.gy + (j + 1) * grid.h} };
		InvalidateRect(hwnd, &r, TRUE);
		map.getField(i, j).setClear();
		if (map.getField(i, j).is0())
		{
			if (i > 0)
			{
				UnhideField(hwnd, i - 1, j);
				if (j > 0)
				{
					UnhideField(hwnd, i - 1, j - 1);
				}
			}
			if (j > 0)
			{
				UnhideField(hwnd, i, j - 1);
				if (i < map.sizeX)
				{
					UnhideField(hwnd, i + 1, j - 1);
				}
			}
			if (i < map.sizeX)
			{
				UnhideField(hwnd, i + 1, j);
				if (j < map.sizeY)
				{
					UnhideField(hwnd, i + 1, j + 1);
				}
			}
			if (j < map.sizeY)
			{
				UnhideField(hwnd, i, j + 1);
				if (i > 0)
				{
					UnhideField(hwnd, i - 1, j + 1);
				}
			}
		}
	}
	if (map.getField(i, j).hasMine())
	{
		return 1;
	}
	return 0;
}

int StateInfo::GameOver(HWND hwnd)
{
	GO = true;
	for (int i = 0; i < map.sizeX; ++i)
	{
		for (int j = 0; j < map.sizeY; ++j)
		{
			if (map.getField(i, j).hasMine())
			{
				map.getField(i, j).setClear();
				RECT r = { {grid.gx + i * grid.w},	{grid.gy + j * grid.h}, {grid.gx + (i + 1) * grid.w}, {grid.gy + (j + 1) * grid.h} };
				InvalidateRect(hwnd, &r, TRUE);
			}
			UnhideField(hwnd, i, j);
		}
	}
	return 0;
}