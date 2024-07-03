#pragma once
#include <Windows.h>
#include <windowsx.h>

// Field value constants for individual areas on gameboard

constexpr auto FV_Mine = 0x0001;									// A mine is on the field
constexpr auto FV_Flag = 0x0002;									// The field is suggested to contain mine, a flag is drawn after right click
constexpr auto FV_Ask = 0x0004;									// Question mark is drawn after second right click (not yet ready)
constexpr auto FV_0 = 0x0008;									// There is no mine in the surrounding fields
constexpr auto FV_1 = 0x0010;									// There is 1 mine in the surrounding fields
constexpr auto FV_2 = 0x0020;									// ...
constexpr auto FV_3 = 0x0040;									// And so on ...
constexpr auto FV_4 = 0x0080;									// ...
constexpr auto FV_5 = 0x0100;									//
constexpr auto FV_6 = 0x0200;									// There's six ...
constexpr auto FV_7 = 0x0400;									// ...
constexpr auto FV_8 = 0x0800;									// All of the surrounding fields contain a mine
constexpr auto FV_Clear = 0x1000;									// Field is cleared, wheter is contains mine or not
constexpr auto FV_Change = 0x2000;									// Not used anymore
constexpr auto FV_Pushed = 0x4000;									// Indicates if left button is down on a field

// Color constants

constexpr auto clr_1 = RGB(0, 0, 255);
constexpr auto clr_2 = RGB(0, 155, 0);
constexpr auto clr_3 = RGB(255, 0, 0);
constexpr auto clr_4 = RGB(0, 0, 155);
constexpr auto clr_5 = RGB(155, 0, 0);
constexpr auto clr_6 = RGB(0, 155, 155);
constexpr auto clr_7 = RGB(0, 0, 0);;
constexpr auto clr_8 = RGB(155, 155, 155);;
///constexpr auto clr_GridLine = RGB(180, 180, 180);
constexpr auto clr_GridLine = RGB(90, 90, 90);
//constexpr auto clr_GridBack = RGB(200, 200, 200);
constexpr auto clr_GridBack = RGB(100, 100, 100);
//constexpr auto clr_BtnLightEdge = RGB(255, 255, 255);
constexpr auto clr_BtnLightEdge = RGB(130, 130, 130);
//constexpr auto clr_BtnDarkEdge = RGB(160, 160, 160);
constexpr auto clr_BtnDarkEdge = RGB(80, 80, 80);
//constexpr auto clr_BtnFace = RGB(200, 200, 200);
constexpr auto clr_BtnFace = RGB(100, 100, 100);
constexpr auto clr_Mine = RGB(0, 0, 0);
constexpr auto clr_MineGlitter = RGB(255, 255, 255);
constexpr auto clr_MineBackGO = RGB(255, 0, 0);
constexpr auto clr_Flag = RGB(255, 0, 0);
constexpr auto clr_Flagstaff = RGB(0, 0, 0);

constexpr auto tmr_GameTime = 1;

// This struct comprises information about the gameboard and its fields
struct MapInfo
{
	const int Xmax = 100;
	const int Ymax = 100;
	int sizeX = 15;														// Horizontal size of gameboard
	int sizeY = 20;														// Vertical size of gameboard
	int cMine = 10;														// Number of mines on gameboard
	short int FieldValue[100][100] = {};								// This array comprises flags for each field on the gameboard
};

// This struct is responsible for graphical sizeing of gameboard
struct GridInfo
{
	long gx = 20, gy = 40;												// upper left coordinates of the gameboard
	int cx = 15, cy = 20, w = 20, h = 20;;
};
// This struct stores mouse pointer coordinates in different states of mouse (Left, Right, Up, Down and Previous and Current coordinates)
struct MouseInfo
{
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
	GridInfo grid;
	MapInfo map;
	MouseInfo mouse;
	bool GO = false;													// Is game over?
	bool DEBUG = true;													// Debugging mode is currently ON
	bool NG = true;														// New game until first left click on game board
	long GameTime = 0;
	void OnMouseMove(LPARAM lParam);
	void OnLeftButtonDown(HWND hwnd, LPARAM lParam);
	void OnLeftButtonUp(HWND hwnd, LPARAM lParam);
	void OnRightButtonDown(HWND hwnd, LPARAM lParam);
	void OnRightButtonUp(LPARAM lParam);
	int NewGame(HWND hwnd, int sizeX, int sizeY, int cMine, int clickX, int clickY);
	int ClearMap();
	int FillMap(int a, int b, int cMine);
	int FillMapWithNumbers();
	int UnhideField(HWND hwnd, int i, int j);
	int GameOver(HWND hwnd);
};

