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
		map(dx, dy).Push();
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
		map(dx, dy).Release();
		map(ux, uy).Release();
		if (dx == ux && dy == uy && !map(ux, uy).hasFlag())// && !pState->GO)
		{
			if (NG)
			{
				NewGame(hwnd, map.sizeX, map.sizeY, map.cMine, ux + 1, uy + 1);
				NG = false;
			}
			if (map(ux, uy).Reveal())
			{
				GameOver(hwnd);
			}
			if (map(ux, uy).SurroundingFlags() == map(ux, uy).SurroundingMines())
			{
				map(ux, uy).RevealNeighbours();
			}
		}
	}
	InvalidateMap();
}

void StateInfo::OnRightButtonDown(HWND hwnd, LPARAM lParam)
{
	mouse.RDX = GET_X_LPARAM(lParam);
	mouse.RDY = GET_Y_LPARAM(lParam);
	if (!GO && mouse.RDX >= grid.gx && mouse.RDY >= grid.gy)
	{
		int dx = (mouse.RDX - grid.gx) / grid.w;
		int dy = (mouse.RDY - grid.gy) / grid.h;
		map(dx, dy).switchFlag();
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

void StateInfo::GameOver(HWND hwnd)
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