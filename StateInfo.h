#pragma once
#include <Windows.h>
#include <windowsx.h>
#include "MapInfo.h"

// Color constants

constexpr auto clr_1 = RGB(0, 0, 255);
constexpr auto clr_2 = RGB(0, 155, 0);
constexpr auto clr_3 = RGB(255, 0, 0);
constexpr auto clr_4 = RGB(0, 0, 155);
constexpr auto clr_5 = RGB(155, 0, 0);
constexpr auto clr_6 = RGB(0, 155, 155);
constexpr auto clr_7 = RGB(0, 0, 0);;
constexpr auto clr_8 = RGB(155, 155, 155);;
constexpr auto clr_GridLine = RGB(180, 180, 180);
//constexpr auto clr_GridLine = RGB(90, 90, 90);
constexpr auto clr_GridBack = RGB(200, 200, 200);
//constexpr auto clr_GridBack = RGB(100, 100, 100);
constexpr auto clr_GridBackGreen = RGB(140, 200, 140);
constexpr auto clr_GridBackRed = RGB(200, 140, 140);
constexpr auto clr_GridBackBlue = RGB(140, 140, 200);
constexpr auto clr_BtnLightEdge = RGB(255, 255, 255);
//constexpr auto clr_BtnLightEdge = RGB(130, 130, 130);
constexpr auto clr_BtnDarkEdge = RGB(160, 160, 160);
//constexpr auto clr_BtnDarkEdge = RGB(80, 80, 80);
constexpr auto clr_BtnFace = RGB(200, 200, 250);
//constexpr auto clr_BtnFace = RGB(100, 100, 100);
constexpr auto clr_BtnFaceFocus = RGB(130, 130, 130);
constexpr auto clr_BtnFaceSafe = RGB(130, 200, 130);
constexpr auto clr_BtnFaceForbidden = RGB(200, 130, 130);
constexpr auto clr_Mine = RGB(0, 0, 0);
constexpr auto clr_MineGlitter = RGB(255, 255, 255);
constexpr auto clr_MineBackGO = RGB(255, 0, 0);
constexpr auto clr_Flag = RGB(255, 0, 0);
constexpr auto clr_Flagstaff = RGB(0, 0, 0);

constexpr auto tmr_GameTime = 1;

// This class is responsible for graphical sizeing of gameboard
class GridInfo
{
public:
	GridInfo();
	long gx;
	long gy;												// upper left coordinates of the gameboard
	int cx = 15, cy = 20, w = 20, h = 20;
	RECT GetRect(Coordinate);
	RECT GetRect(int, int);
};
// This class stores mouse pointer coordinates in different states of mouse (Left, Right, Up, Down and Previous and Current coordinates)
class MouseInfo
{
public:
	long LDX = 0,
		 LDY = 0,
		 LUX = 0,
		 LUY = 0,
		 RDX = 0,
		 RDY = 0,
		 RUX = 0,
		 RUY = 0,
		 XP = 0,
		 YP = 0,
		 X = 0,
		 Y = 0;
};
// This class is used to store information about the state of the application
class StateInfo
{
public:
	StateInfo();
	void InvalidateMap();
	void MapChanged(Coordinate);
	GridInfo grid;
	MapInfo map;
	MouseInfo mouse;
	COLORREF colors[8] = {clr_1, clr_2, clr_3, clr_4, clr_5, clr_6, clr_7, clr_8};
	LPCWSTR fieldTexts[8] = {L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8"};
	bool GO = false;													// Is game over?
	bool DEBUG = true;													// Debugging mode is currently ON
	bool NG = true;														// New game until first left click on game board
	long GameTime = 0;
	void AutoFlag();
	void AutoClick();
	Field* GetActiveField();
	void OnMouseMove(LPARAM lParam);
	void OnLeftButtonDown(LPARAM lParam);
	void OnLeftButtonUp(LPARAM lParam);
	void LeftButtonUp(int, int);
	void OnRightButtonDown(LPARAM lParam);
	void OnRightButtonUp(LPARAM lParam);
	int NewGame(int sizeX, int sizeY, int cMine, int clickX, int clickY);
	int FillMap(int a, int b, int cMine);
	void GameOver();
	HWND mainWindow = 0;
private:
	Field* activeField;
};

