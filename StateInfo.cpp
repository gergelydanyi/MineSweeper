#include "StateInfo.h"
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
		if ((map.FieldValue[dx][dy] & FV_Clear) != FV_Clear && (map.FieldValue[dx][dy] & FV_Flag) != FV_Flag)
		{
			map.FieldValue[dx][dy] = map.FieldValue[dx][dy] | FV_Pushed;
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
		if (map.FieldValue[dx][dy] & FV_Pushed)
		{
			map.FieldValue[dx][dy] = map.FieldValue[dx][dy] ^ FV_Pushed;
		}
		if (map.FieldValue[ux][uy] & FV_Pushed)
		{
			map.FieldValue[ux][uy] = map.FieldValue[ux][uy] ^ FV_Pushed;
		}
		if (dx == ux && dy == uy && (map.FieldValue[ux][uy] & FV_Flag) != FV_Flag)// && !pState->GO)
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
		if (map.FieldValue[dx][dy] & FV_Flag)
		{
			map.FieldValue[dx][dy] = map.FieldValue[dx][dy] ^ FV_Flag;
		}
		else
		{
			map.FieldValue[dx][dy] = map.FieldValue[dx][dy] | FV_Flag;
		}
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
	ClearMap();
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

int StateInfo::ClearMap()
{
	for (int i = 0; i < map.sizeX; ++i)
	{
		for (int j = 0; j < map.sizeY; ++j)
		{
			map.FieldValue[i][j] = 0;
		}
	}
	return 0;
}

int StateInfo::FillMap(int a, int b, int cMine)
{
	int n = rand() % (b - a - 1) + a + 1;
	int nx = n / map.sizeY;
	int ny = n % map.sizeY;
	map.FieldValue[nx][ny] = map.FieldValue[nx][ny] | FV_Mine;
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
			if ((map.FieldValue[i][j] & FV_Mine) != FV_Mine)
			{
				if (i > 0 && j > 0 && (map.FieldValue[i - 1][j - 1] & FV_Mine)) { ++cm; }
				if (j > 0 && (map.FieldValue[i][j - 1] & FV_Mine)) { ++cm; }
				if (i < map.sizeX - 1 && j > 0 && (map.FieldValue[i + 1][j - 1] & FV_Mine)) { ++cm; }
				if (i < map.sizeX - 1 && (map.FieldValue[i + 1][j] & FV_Mine)) { ++cm; }
				if (i < map.sizeX - 1 && j < map.sizeY - 1 && (map.FieldValue[i + 1][j + 1] & FV_Mine)) { ++cm; }
				if (j < map.sizeY - 1 && (map.FieldValue[i][j + 1] & FV_Mine)) { ++cm; }
				if (i > 0 && j < map.sizeY - 1 && (map.FieldValue[i - 1][j + 1] & FV_Mine)) { ++cm; }
				if (i > 0 && (map.FieldValue[i - 1][j] & FV_Mine)) { ++cm; }
				switch (cm)
				{
				case 0:map.FieldValue[i][j] = map.FieldValue[i][j] | FV_0; break;
				case 1:map.FieldValue[i][j] = map.FieldValue[i][j] | FV_1; break;
				case 2:map.FieldValue[i][j] = map.FieldValue[i][j] | FV_2; break;
				case 3:map.FieldValue[i][j] = map.FieldValue[i][j] | FV_3; break;
				case 4:map.FieldValue[i][j] = map.FieldValue[i][j] | FV_4; break;
				case 5:map.FieldValue[i][j] = map.FieldValue[i][j] | FV_5; break;
				case 6:map.FieldValue[i][j] = map.FieldValue[i][j] | FV_6; break;
				case 7:map.FieldValue[i][j] = map.FieldValue[i][j] | FV_7; break;
				case 8:map.FieldValue[i][j] = map.FieldValue[i][j] | FV_8; break;
				}
			}
		}
	}
	return 0;
}

int StateInfo::UnhideField(HWND hwnd, int i, int j)
{
	if ((map.FieldValue[i][j] & FV_Flag) != FV_Flag && (map.FieldValue[i][j] & FV_Clear) != FV_Clear)
	{
		RECT r = {
			{grid.gx + i * grid.w},
			{grid.gy + j * grid.h},
			{grid.gx + (i + 1) * grid.w},
			{grid.gy + (j + 1) * grid.h} };
		InvalidateRect(hwnd, &r, TRUE);
		map.FieldValue[i][j] = map.FieldValue[i][j] | FV_Clear;
		if (map.FieldValue[i][j] & FV_0)
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
	if (map.FieldValue[i][j] & FV_Mine)
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
			if (map.FieldValue[i][j] & FV_Mine)
			{
				map.FieldValue[i][j] = map.FieldValue[i][j] | FV_Clear;
				RECT r = { {grid.gx + i * grid.w},	{grid.gy + j * grid.h}, {grid.gx + (i + 1) * grid.w}, {grid.gy + (j + 1) * grid.h} };
				InvalidateRect(hwnd, &r, TRUE);
			}
			UnhideField(hwnd, i, j);
		}
	}
	return 0;
}