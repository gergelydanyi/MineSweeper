#ifndef UNICODE
#define UNICODE
#endif

#include <Windows.h>
#include <windowsx.h>
#include "resource.h"
#include <string>

// Field value constants for individual areas on gameboard

constexpr auto FV_Mine		= 0x0001;									// A mine is on the field
constexpr auto FV_Flag		= 0x0002;									// The field is suggested to contain mine, a flag is drawn after right click
constexpr auto FV_Ask		= 0x0004;									// Question mark is drawn after second right click (not yet ready)
constexpr auto FV_0			= 0x0008;									// There is no mine in the surrounding fields
constexpr auto FV_1			= 0x0010;									// There is 1 mine in the surrounding fields
constexpr auto FV_2			= 0x0020;									// ...
constexpr auto FV_3			= 0x0040;									// And so on ...
constexpr auto FV_4			= 0x0080;									// ...
constexpr auto FV_5			= 0x0100;									//
constexpr auto FV_6			= 0x0200;									// There's six ...
constexpr auto FV_7			= 0x0400;									// ...
constexpr auto FV_8			= 0x0800;									// All of the surrounding fields contain a mine
constexpr auto FV_Clear		= 0x1000;									// Field is cleared, wheter is contains mine or not
constexpr auto FV_Change	= 0x2000;									// Not used anymore
constexpr auto FV_Pushed	= 0x4000;									// Indicates if left button is down on a field

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
constexpr auto clr_GridBack = RGB(200, 200, 200);
constexpr auto clr_BtnLightEdge = RGB(255, 255, 255);
constexpr auto clr_BtnDarkEdge = RGB(160, 160, 160);
constexpr auto clr_BtnFace = RGB(200, 200, 200);
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
	int cx = 15, cy = 20, w = 20, h = 20;
};
// This struct stores mouse pointer coordinates in different states of mouse (Left, Right, Up, Down and Previous and Current coordinates)
struct MouseInfo
{
	long LDX = 0, LDY = 0, LUX = 0, LUY = 0, RDX = 0, RDY = 0, RUX = 0, RUY = 0, XP = 0, YP = 0, X = 0, Y = 0;
};
// This struct is used to store information about the state of the application
struct StateInfo
{
	GridInfo grid;
	MapInfo map;
	MouseInfo mouse;
	bool GO = false;													// Is game over?
	bool DEBUG = true;													// Debugging mode is currently ON
	bool NG = true;														// New game until first left click on game board
	long GameTime = 0;
};

LRESULT CALLBACK HandleMessages(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int NewGame(HWND hwnd, StateInfo* pState, int sizeX, int sizeY, int cMine, int clickX, int clickY);
int ClearMap(StateInfo* pState);
int DrawField(HDC hdc, StateInfo* pState, int i, int j);
int DrawFlag(HDC hdc, StateInfo* pState, int i, int j);
int DrawMine(HDC hdc, StateInfo* pState, int i, int j);
int DrawMap(HDC hdc, StateInfo* pState, HRGN hrgn);
int FillMap(StateInfo* pState, int a, int b, int cMine);
int FillMapWithNumbers(StateInfo* pState);
int UnhideField(HWND hwnd,StateInfo* pState, int i, int j);
int GameOver(HWND hwnd, StateInfo* pState);
int debug(HWND hwnd, StateInfo* pState);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR cmdLine, int nCmdShow)
{
	StateInfo* pState = new StateInfo;
	ClearMap(pState);

	if(pState == NULL)
	{
		return 0;
	}

	const wchar_t CLASS_NAME[] = L"MineSweeperWindowClass";

	WNDCLASS wc = {};
	wc.lpfnWndProc = HandleMessages;
	wc.lpszClassName = CLASS_NAME;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
	wc.lpszMenuName = MAKEINTRESOURCEW(IDR_MENU1);
	//wc.hCursor = (HCURSOR)IDC_ARROW;									// this caused an unknown error for some reason

	RegisterClassW(&wc);

	HWND hWnd = CreateWindowW(
		CLASS_NAME,
		L"MineSweeper",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, pState
	);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	SetTimer(hWnd, tmr_GameTime, 1000, NULL);

	MSG msg;
	while (GetMessageW(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK HandleMessages(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	StateInfo* pState;
	if (uMsg == WM_CREATE)
	{
		CREATESTRUCTW* pcs = reinterpret_cast<CREATESTRUCTW*>(lParam);
		pState = reinterpret_cast<StateInfo*>(pcs->lpCreateParams);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)pState);
		NewGame(hwnd, pState, 9, 9, 10, 0, 0);
	}
	else
	{
		LONG_PTR lp = GetWindowLongPtrW(hwnd, GWLP_USERDATA);
		pState = reinterpret_cast<StateInfo*>(lp);
	}
	switch (uMsg)
	{
		case WM_COMMAND:
		{
			if (HIWORD(wParam) == 0)
			{
				switch (LOWORD(wParam))
				{
					case ID_NEW_EASY:
					{
						NewGame(hwnd, pState, 9, 9, 10, 0, 0);
						pState->NG = true;
					}
					return 0;
					case ID_NEW_NORMAL:
					{
						NewGame(hwnd, pState, 16, 16, 40, 0, 0);
						pState->NG = true;
					}
					return 0;
					case ID_NEW_HARD:
					{
						NewGame(hwnd, pState, 30, 16, 99, 0, 0);
						pState->NG = true;
					}
					return 0;
					case ID_NEW_CUSTOM:
					{
						//TODO: input dialog box is needed
						NewGame(hwnd, pState, 50, 40, 50, 0, 0);
						pState->NG = true;
					}
					return 0;
				}
			}
		}
		return 0;
		case WM_MOUSEMOVE:
		{
			pState->mouse.XP = pState->mouse.X;
			pState->mouse.YP = pState->mouse.Y;
			pState->mouse.X = GET_X_LPARAM(lParam);
			pState->mouse.Y = GET_Y_LPARAM(lParam);
			debug(hwnd, pState);
		}
		return 0;
		case WM_LBUTTONDOWN:
		{
			pState->mouse.LDX = GET_X_LPARAM(lParam);
			pState->mouse.LDY = GET_Y_LPARAM(lParam);
			if (!pState->GO && pState->mouse.LDX >= pState->grid.gx && pState->mouse.LDY >= pState->grid.gy)
			{
				int dx = (pState->mouse.LDX - pState->grid.gx) / pState->grid.w;
				int dy = (pState->mouse.LDY - pState->grid.gy) / pState->grid.h;
				if ((pState->map.FieldValue[dx][dy] & FV_Clear) != FV_Clear && (pState->map.FieldValue[dx][dy] & FV_Flag) != FV_Flag)
				{
					pState->map.FieldValue[dx][dy] = pState->map.FieldValue[dx][dy] | FV_Pushed;
				}
				RECT r = {
					{pState->grid.gx + dx * pState->grid.w},
					{pState->grid.gy + dy * pState->grid.h},
					{pState->grid.gx + ++dx * pState->grid.w},
					{pState->grid.gy + ++dy * pState->grid.h} };
				InvalidateRect(hwnd, &r, TRUE);
			}
			debug(hwnd, pState);
		}
		return 0;
		case WM_LBUTTONUP:
		{
			pState->mouse.LUX = GET_X_LPARAM(lParam);
			pState->mouse.LUY = GET_Y_LPARAM(lParam);
			int ux = (pState->mouse.LUX - pState->grid.gx) / pState->grid.w;
			int uy = (pState->mouse.LUY - pState->grid.gy) / pState->grid.h;
			int dx = (pState->mouse.LDX - pState->grid.gx) / pState->grid.w;
			int dy = (pState->mouse.LDY - pState->grid.gy) / pState->grid.h;
			if (pState->mouse.LDX >= pState->grid.gx && pState->mouse.LDY >= pState->grid.gy)
			{
				if (pState->map.FieldValue[dx][dy] & FV_Pushed)
				{
					pState->map.FieldValue[dx][dy] = pState->map.FieldValue[dx][dy] ^ FV_Pushed;
				}
				if (pState->map.FieldValue[ux][uy] & FV_Pushed)
				{
					pState->map.FieldValue[ux][uy] = pState->map.FieldValue[ux][uy] ^ FV_Pushed;
				}
				if (dx == ux && dy == uy && (pState->map.FieldValue[ux][uy] & FV_Flag) != FV_Flag)// && !pState->GO)
				{
					if (pState->NG)
					{
						NewGame(hwnd, pState, pState->map.sizeX, pState->map.sizeY, pState->map.cMine, ux + 1, uy + 1);
						pState->NG = false;
					}
					if (UnhideField(hwnd, pState, ux, uy))
					{
						GameOver(hwnd, pState);
					}
				}
				else
				{
					RECT r = { {pState->grid.gx + dx * pState->grid.w},	{pState->grid.gy + dy * pState->grid.h}, {pState->grid.gx + ++dx * pState->grid.w}, {pState->grid.gy + ++dy * pState->grid.h} };
					InvalidateRect(hwnd, &r, TRUE);
				}
			}
			
			debug(hwnd, pState);
		}
		return 0;
		case WM_RBUTTONDOWN:
		{
			pState->mouse.RDX = GET_X_LPARAM(lParam);
			pState->mouse.RDY = GET_Y_LPARAM(lParam);
			if (!pState->GO)
			{
				int dx = (pState->mouse.RDX - pState->grid.gx) / pState->grid.w;
				int dy = (pState->mouse.RDY - pState->grid.gy) / pState->grid.h;
				if (pState->map.FieldValue[dx][dy] & FV_Flag)
				{
					pState->map.FieldValue[dx][dy] = pState->map.FieldValue[dx][dy] ^ FV_Flag;
				}
				else
				{
					pState->map.FieldValue[dx][dy] = pState->map.FieldValue[dx][dy] | FV_Flag;
				}
				RECT r = {
					{pState->grid.gx + dx * pState->grid.w},
					{pState->grid.gy + dy * pState->grid.h},
					{pState->grid.gx + ++dx * pState->grid.w},
					{pState->grid.gy + ++dy * pState->grid.h} };
				InvalidateRect(hwnd, &r, TRUE);
			}
			debug(hwnd, pState);
		}
		return 0;
		case WM_RBUTTONUP:
		{
			pState->mouse.RUX = GET_X_LPARAM(lParam);
			pState->mouse.RUY = GET_Y_LPARAM(lParam);
			debug(hwnd, pState);
		}
		return 0;
		case WM_TIMER:
		{
			HDC hdc = GetWindowDC(hwnd);
			std::wstring ws;
			const wchar_t* cs;
			wchar_t wts[30];
			int ls;
			ws = std::to_wstring(pState->GameTime++);
			cs = ws.c_str();
			wcscpy_s(wts, cs);
//			wcscat_s(wts, cs);
			ls = std::wcslen(wts);
			TextOutW(hdc, 200, 60, wts, ls);
			ReleaseDC(hwnd, hdc);
		}
		return 0;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HRGN hrgn = NULL;
			if (GetUpdateRect(hwnd, NULL, FALSE))
			{
				GetUpdateRgn(hwnd, hrgn, FALSE);
				HDC hdc = BeginPaint(hwnd, &ps);
				SelectObject(hdc, GetStockObject(DC_BRUSH));
				SelectObject(hdc, GetStockObject(DC_PEN));
				DrawMap(hdc, pState, hrgn);
				EndPaint(hwnd, &ps);
			}
		}
		return 0;
		case WM_SETCURSOR:
		{
			HCURSOR hc = NULL;
			switch (LOWORD(lParam))
			{
				case HTCLIENT:hc = LoadCursorW(NULL, IDC_ARROW); break;
				case HTCAPTION:hc = LoadCursorW(NULL, IDC_ARROW); break;
				case HTSYSMENU:hc = LoadCursorW(NULL, IDC_ARROW); break;
				case HTMENU:hc = LoadCursorW(NULL, IDC_ARROW); break;
				case HTHSCROLL:hc = LoadCursorW(NULL, IDC_CROSS); break;
				case HTVSCROLL:hc = LoadCursorW(NULL, IDC_CROSS); break;
				case HTMINBUTTON:hc = LoadCursorW(NULL, IDC_ARROW); break;
				case HTMAXBUTTON:hc = LoadCursorW(NULL, IDC_ARROW); break;
				case HTLEFT:hc = LoadCursorW(NULL, IDC_SIZEWE); break;
				case HTRIGHT:hc = LoadCursorW(NULL, IDC_SIZEWE); break;
				case HTTOP:hc = LoadCursorW(NULL, IDC_SIZENS); break;
				case HTTOPLEFT:hc = LoadCursorW(NULL, IDC_SIZENWSE); break;
				case HTTOPRIGHT:hc = LoadCursorW(NULL, IDC_SIZENESW); break;
				case HTBOTTOM: hc = LoadCursorW(NULL, IDC_SIZENS); break;
				case HTBOTTOMLEFT:hc = LoadCursorW(NULL, IDC_SIZENESW); break;
				case HTBOTTOMRIGHT:hc = LoadCursorW(NULL, IDC_SIZENWSE); break;
				default: hc = LoadCursorW(NULL, IDC_ARROW); break;
			}
			if (hc)
			{
				SetCursor(hc);
				return TRUE;
			}
		}
		return 0;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		return 0;
	}
	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

int NewGame(HWND hwnd, StateInfo* pState, int sizeX, int sizeY, int cMine, int clickX, int clickY)
{
	pState->GO = false;
	pState->grid.cx = sizeX;
	pState->grid.cy = sizeY;
	pState->map.sizeX = sizeX;
	pState->map.sizeY = sizeY;
	pState->map.cMine = cMine;
	ClearMap(pState);
	int click = clickX * clickY;
	int mapsize = sizeX * sizeY;
	if (click > 1 && click < mapsize)
	{
		long f = (mapsize - 1) / (cMine - 1);
		int c1 = click / f;
		if (c1 >= cMine) { c1 = cMine - 1; }
		int c2 = cMine - c1 - 1;
		FillMap(pState, -1, click, c1);
		FillMap(pState, click, mapsize, c2);
	}
	else if (click == 1)
	{
		FillMap(pState, 0, mapsize, cMine);
	}
	else if (click == mapsize)
	{
		FillMap(pState, -1, mapsize - 1, cMine);
	}
	FillMapWithNumbers(pState);
	InvalidateRect(hwnd, NULL, TRUE);
	return 0;
}

int ClearMap(StateInfo* pState)
{
	for (int i = 0; i < pState->map.sizeX; ++i)
	{
		for (int j = 0; j < pState->map.sizeY; ++j)
		{
			pState->map.FieldValue[i][j] = 0;
		}
	}
	return 0;
}

int FillMap(StateInfo* pState, int a, int b, int cMine)
{
	int n = rand() % (b - a - 1) + a + 1;
	int nx = n / pState->map.sizeY;
	int ny = n % pState->map.sizeY;
	pState->map.FieldValue[nx][ny] = pState->map.FieldValue[nx][ny] | FV_Mine;
	if (cMine > 1)
	{
		long f = (b - a - 2) / (cMine - 1);
		int c1 = (n - a - 1) / f;
		if (c1 >= cMine) { c1 = cMine - 1; }
		int c2 = cMine - c1 - 1;
		if (c1) { FillMap(pState, a, n, c1); }
		if (c2) { FillMap(pState, n, b, c2); }
	}
	return 0;
}

int FillMapWithNumbers(StateInfo* pState)
{
	int cm = 0;
	for (int i = 0; i < pState->map.sizeX; ++i)
	{
		for (int j = 0; j < pState->map.sizeY; ++j)
		{
			cm = 0;
			if ((pState->map.FieldValue[i][j] & FV_Mine) != FV_Mine)
			{
/*				if (i>0 && j>0 && pState->map.FieldValue[i - 1][j - 1] & FV_Mine) { ++cm; }
				if (j>0 && pState->map.FieldValue[i    ][j - 1] & FV_Mine) { ++cm; }
				if (i<pState->map.sizeX && j > 0 && pState->map.FieldValue[i + 1][j - 1] & FV_Mine) { ++cm; }
				if (i < pState->map.sizeX && pState->map.FieldValue[i + 1][j    ] & FV_Mine) { ++cm; }
				if (i < pState->map.sizeX && j < pState->map.sizeY && pState->map.FieldValue[i + 1][j + 1] & FV_Mine) { ++cm; }
				if (j < pState->map.sizeY && pState->map.FieldValue[i    ][j + 1] & FV_Mine) { ++cm; }
				if (i>0 && j < pState->map.sizeY && pState->map.FieldValue[i - 1][j + 1] & FV_Mine) { ++cm; }
				if (i>0 && pState->map.FieldValue[i - 1][j    ] & FV_Mine) { ++cm; }*/
				if (i>0 && j>0 && (pState->map.FieldValue[i - 1][j - 1] & FV_Mine)) { ++cm; }
				if (j>0 && (pState->map.FieldValue[i    ][j - 1] & FV_Mine)) { ++cm; }
				if (i<pState->map.sizeX-1 && j > 0 && (pState->map.FieldValue[i + 1][j - 1] & FV_Mine)) { ++cm; }
				if (i < pState->map.sizeX-1 && (pState->map.FieldValue[i + 1][j    ] & FV_Mine)) { ++cm; }
				if (i < pState->map.sizeX-1 && j < pState->map.sizeY-1 && (pState->map.FieldValue[i + 1][j + 1] & FV_Mine)) { ++cm; }
				if (j < pState->map.sizeY-1 && (pState->map.FieldValue[i    ][j + 1] & FV_Mine)) { ++cm; }
				if (i>0 && j < pState->map.sizeY-1 && (pState->map.FieldValue[i - 1][j + 1] & FV_Mine)) { ++cm; }
				if (i>0 && (pState->map.FieldValue[i - 1][j    ] & FV_Mine)) { ++cm; }
				switch (cm)
				{
				case 0:pState->map.FieldValue[i][j] = pState->map.FieldValue[i][j] | FV_0; break;
				case 1:pState->map.FieldValue[i][j] = pState->map.FieldValue[i][j] | FV_1; break;
				case 2:pState->map.FieldValue[i][j] = pState->map.FieldValue[i][j] | FV_2; break;
				case 3:pState->map.FieldValue[i][j] = pState->map.FieldValue[i][j] | FV_3; break;
				case 4:pState->map.FieldValue[i][j] = pState->map.FieldValue[i][j] | FV_4; break;
				case 5:pState->map.FieldValue[i][j] = pState->map.FieldValue[i][j] | FV_5; break;
				case 6:pState->map.FieldValue[i][j] = pState->map.FieldValue[i][j] | FV_6; break;
				case 7:pState->map.FieldValue[i][j] = pState->map.FieldValue[i][j] | FV_7; break;
				case 8:pState->map.FieldValue[i][j] = pState->map.FieldValue[i][j] | FV_8; break;
				}
			}
		}
	}
	return 0;
}
int DrawField(HDC hdc, StateInfo* pState, int i, int j)
{
	long gx = pState->grid.gx;
	long gy = pState->grid.gy;
	int w = pState->grid.w;
	int h = pState->grid.h;
	if (pState->map.FieldValue[i][j] & FV_Pushed)						//button pushed
	{
		SetDCBrushColor(hdc, clr_GridBack);
		SetDCPenColor(hdc, clr_GridLine);
		Rectangle(hdc, gx + i * w, gy + j * h, gx + i * w + w + 1, gy + j * h + h + 1);
	}
	else if (pState->map.FieldValue[i][j] & FV_Clear)					//unhidden area
	{
			SetDCBrushColor(hdc, clr_GridBack);
			SetDCPenColor(hdc, clr_GridLine);
			if (pState->map.FieldValue[i][j] & FV_Mine && !pState->GO)	//there is a mine
			{
				SetDCBrushColor(hdc, clr_MineBackGO);
				SetDCPenColor(hdc, clr_MineBackGO);
			}
			Rectangle(hdc, gx + i * w, gy + j * h, gx + i * w + w, gy + j * h + h);
			SetBkColor(hdc, clr_GridBack);
			SetTextAlign(hdc, TA_CENTER);
			TEXTMETRICW tm;
			GetTextMetricsW(hdc, &tm);
			int dx = 10, dy = 10 - tm.tmHeight / 2;
			if (pState->map.FieldValue[i][j] & FV_1) { SetTextColor(hdc, clr_1); TextOutW(hdc, gx + dx + i * w, gy + dy + j * h, L"1", 1); }
			if (pState->map.FieldValue[i][j] & FV_2) { SetTextColor(hdc, clr_2); TextOutW(hdc, gx + dx + i * w, gy + dy + j * h, L"2", 1); }
			if (pState->map.FieldValue[i][j] & FV_3) { SetTextColor(hdc, clr_3); TextOutW(hdc, gx + dx + i * w, gy + dy + j * h, L"3", 1); }
			if (pState->map.FieldValue[i][j] & FV_4) { SetTextColor(hdc, clr_4); TextOutW(hdc, gx + dx + i * w, gy + dy + j * h, L"4", 1); }
			if (pState->map.FieldValue[i][j] & FV_5) { SetTextColor(hdc, clr_5); TextOutW(hdc, gx + dx + i * w, gy + dy + j * h, L"5", 1); }
			if (pState->map.FieldValue[i][j] & FV_6) { SetTextColor(hdc, clr_6); TextOutW(hdc, gx + dx + i * w, gy + dy + j * h, L"6", 1); }
			if (pState->map.FieldValue[i][j] & FV_7) { SetTextColor(hdc, clr_7); TextOutW(hdc, gx + dx + i * w, gy + dy + j * h, L"7", 1); }
			if (pState->map.FieldValue[i][j] & FV_8) { SetTextColor(hdc, clr_8); TextOutW(hdc, gx + dx + i * w, gy + dy + j * h, L"8", 1); }
			if (pState->map.FieldValue[i][j] & FV_Mine)
			{
				DrawMine(hdc, pState, i, j);
			}
	}
	else																//hidden area
	{
		SetDCBrushColor(hdc, clr_BtnLightEdge);
		SetDCPenColor(hdc, clr_BtnLightEdge);
		Rectangle(hdc,
			gx + i * w,
			gy + j * h,
			gx + i * w + w,
			gy + j * h + h);
		SetDCBrushColor(hdc, clr_BtnDarkEdge);
		SetDCPenColor(hdc, clr_BtnDarkEdge);
		POINT apt[3] = {};
		apt[0].x = gx + i * w + w;
		apt[0].y = gy + j * h;
		apt[1].x = apt[0].x;
		apt[1].y = gy + j * h + h;
		apt[2].x = gx + i * w;
		apt[2].y = apt[1].y;
		Polygon(hdc, apt, 3);
		SetDCBrushColor(hdc, clr_BtnFace);
		SetDCPenColor(hdc, clr_BtnFace);
		int border = 2;
		Rectangle(hdc, gx + i * w + border, gy + j * h + border, gx + i * w + w - border, gy + j * h + h - border);
		if ((pState->map.FieldValue[i][j] & FV_Flag) == FV_Flag)
		{
			DrawFlag(hdc, pState, i, j);
		}
	}

	return 0;
}

int UnhideField(HWND hwnd, StateInfo* pState, int i, int j)
{
	if ((pState->map.FieldValue[i][j] & FV_Flag) != FV_Flag && (pState->map.FieldValue[i][j] & FV_Clear) != FV_Clear)
	{
		RECT r = { {pState->grid.gx + i * pState->grid.w},	{pState->grid.gy + j * pState->grid.h}, {pState->grid.gx + (i+1) * pState->grid.w}, {pState->grid.gy + (j+1) * pState->grid.h} };
		InvalidateRect(hwnd, &r, TRUE);
		pState->map.FieldValue[i][j] = pState->map.FieldValue[i][j] | FV_Clear;
		if (pState->map.FieldValue[i][j] & FV_0)
		{
			if (i > 0)
			{
				UnhideField(hwnd, pState, i - 1, j);
				if (j > 0)
				{
					UnhideField(hwnd, pState, i - 1, j - 1);
				}
			}
			if (j > 0)
			{
				UnhideField(hwnd, pState, i, j - 1);
				if (i < pState->map.sizeX)
				{
					UnhideField(hwnd, pState, i + 1, j - 1);
				}
			}
			if (i < pState->map.sizeX)
			{
				UnhideField(hwnd, pState, i + 1, j);
				if (j < pState->map.sizeY)
				{
					UnhideField(hwnd, pState, i + 1, j + 1);
				}
			}
			if (j < pState->map.sizeY)
			{
				UnhideField(hwnd, pState, i, j + 1);
				if (i > 0)
				{
					UnhideField(hwnd, pState, i - 1, j + 1);
				}
			}
		}
	}
	if (pState->map.FieldValue[i][j] & FV_Mine)
	{
		return 1;
	}
	return 0;
}

int GameOver(HWND hwnd, StateInfo* pState)
{
	pState->GO = true;
	for (int i = 0; i < pState->map.sizeX; ++i)
	{
		for (int j = 0; j < pState->map.sizeY; ++j)
		{
			if (pState->map.FieldValue[i][j] & FV_Mine)
			{
				pState->map.FieldValue[i][j] = pState->map.FieldValue[i][j] | FV_Clear;
				RECT r = { {pState->grid.gx + i * pState->grid.w},	{pState->grid.gy + j * pState->grid.h}, {pState->grid.gx + (i + 1) * pState->grid.w}, {pState->grid.gy + (j + 1) * pState->grid.h} };
				InvalidateRect(hwnd, &r, TRUE);
			}
			UnhideField(hwnd, pState, i, j);
		}
	}
	return 0;
}

int DrawMine(HDC hdc, StateInfo* pState, int i, int j)
{
	long gx = pState->grid.gx;
	long gy = pState->grid.gy;
	int w = pState->grid.w;
	int h = pState->grid.h;
	int left = gx + i * w + 4;
	int top = gy + j * h + 4;
	int right = left + w - 7;
	int bottom = top + h - 7;
	SetDCBrushColor(hdc, clr_Mine);
	SetDCPenColor(hdc, clr_Mine);
	Ellipse(hdc, left, top, right, bottom);
	MoveToEx(hdc, left, top, NULL);
	LineTo(hdc, right, bottom);
	MoveToEx(hdc, right, top, NULL);
	LineTo(hdc, left, bottom);
	MoveToEx(hdc, (left + right) / 2, top-1, NULL);
	LineTo(hdc, (left + right) / 2, bottom+1);
	MoveToEx(hdc, left-1, (top + bottom) / 2, NULL);
	LineTo(hdc, right+1, (top + bottom) / 2);
	left += 3;
	top += 3;
	right -= 6;
	bottom -= 6;
	SetDCBrushColor(hdc, clr_MineGlitter);
	SetDCPenColor(hdc, clr_MineGlitter);
	Ellipse(hdc, left, top, right, bottom);
	return 0;
}

int DrawFlag(HDC hdc, StateInfo* pState, int i, int j)
{
	long gx = pState->grid.gx;
	long gy = pState->grid.gy;
	int w = pState->grid.w;
	int h = pState->grid.h;
	RECT r;
	r.left = gx + i * w;
	r.right = r.left + w;
	r.top = gy + j * h;
	r.bottom = r.top + h;
	SetDCBrushColor(hdc, clr_Flagstaff);
	SetDCPenColor(hdc, clr_Flagstaff);
	r.left += 4;
	r.right -= 4;
	r.top += 14;
	r.bottom -= 3;
	Rectangle(hdc, r.left, r.top, r.right, r.bottom);
	r.left += 3;
	r.right -= 3;
	r.top -= 2;
	r.bottom = r.top + 2;
	Rectangle(hdc, r.left, r.top, r.right, r.bottom);
	r.left = gx + i * w + 10;
	r.right = r.left + 2;
	r.top = gy + j * h + 3;
	r.bottom = r.top + 10;
	Rectangle(hdc, r.left, r.top, r.right, r.bottom);
	POINT apt[3] = {};
	apt[0].x = r.right-1;
	apt[0].y = r.top;
	apt[1].x = r.right - 1;
	apt[1].y = apt[0].y + 6;
	apt[2].x = apt[0].x - 6;
	apt[2].y = (apt[0].y + apt[1].y) / 2;
	SetDCBrushColor(hdc, clr_Flag);
	SetDCPenColor(hdc, clr_Flag);
	Polygon(hdc, apt, 3);
	return 0;
}

int DrawMap(HDC hdc, StateInfo* pState, HRGN hrgn)
{
	long gx = pState->grid.gx;
	long gy = pState->grid.gy;
	int cx = pState->grid.cx;
	int cy = pState->grid.cy;
	int w = pState->grid.w;
	int h = pState->grid.h;
	HRGN hrgnDst = NULL;
	HRGN hrgnSrc1 = hrgn;
	HRGN hrgnSrc2;
	for (int j = 0; j < cy; ++j)
	{
		for (int i = 0; i < cx; ++i)
		{
			hrgnSrc2 = CreateRectRgn(
				gx + i * w,
				gy + j * h,
				gx + i * w + w,
				gy + j * h + h);
			if (CombineRgn(hrgnDst, hrgnSrc1, hrgnSrc2, RGN_AND) != NULLREGION)
			{
				DrawField(hdc, pState, i, j);
			}
			DeleteObject(hrgnSrc2);
		}
	}
	return 0;
}

int debugStr(long k, int i, HDC hdc, const wchar_t* s)
{
	std::wstring ws;
	const wchar_t* cs;
	wchar_t wts[30];
	int ls;
	ws = std::to_wstring(k);
	cs = ws.c_str();
	wcscpy_s(wts, s);
	wcscat_s(wts, cs);
	ls = std::wcslen(wts);
	TextOutW(hdc, 700, 100 + i * 20,
		wts,
		ls
	);
	return 0;
}


int debug(HWND hwnd, StateInfo* pState)
{
	HDC hdc = GetWindowDC(hwnd);
	SetBkColor(hdc, RGB(0, 0, 0));
	SetTextColor(hdc, RGB(255, 255, 255));
	SelectObject(hdc, GetStockObject(DC_BRUSH));
	SelectObject(hdc, GetStockObject(DC_PEN));
	SetDCBrushColor(hdc, RGB(100, 200, 200));
	SetDCPenColor(hdc, RGB(100, 200, 200));
	Rectangle(hdc, 690, 90, 950, 550);
	debugStr(pState->mouse.LDX, 0, hdc, L"LDX: ");
	debugStr(pState->mouse.LDY, 1, hdc, L"LDY: ");
	debugStr(pState->mouse.LUX, 2, hdc, L"LUX: ");
	debugStr(pState->mouse.LUY, 3, hdc, L"LUY: ");
	debugStr(pState->mouse.RDX, 4, hdc, L"RDX: ");
	debugStr(pState->mouse.RDY, 5, hdc, L"RDY: ");
	debugStr(pState->mouse.RUX, 6, hdc, L"RUX: ");
	debugStr(pState->mouse.RUY, 7, hdc, L"RUY: ");
	debugStr(pState->mouse.XP, 8, hdc, L"XP: ");
	debugStr(pState->mouse.YP, 9, hdc, L"YP: ");
	debugStr(pState->mouse.X, 10, hdc, L"X: ");
	debugStr(pState->mouse.Y, 11, hdc, L"Y: ");
	SetTextColor(hdc, RGB(0, 0, 255)); SetBkColor(hdc, RGB(220, 220, 220)); debugStr(1, 12, hdc, L"1: ");
	SetTextColor(hdc, RGB(0, 155, 0)); SetBkColor(hdc, RGB(220, 220, 220)); debugStr(2, 13, hdc, L"2: ");
	SetTextColor(hdc, RGB(255, 0, 0)); SetBkColor(hdc, RGB(220, 220, 220)); debugStr(3, 14, hdc, L"3: ");
	SetTextColor(hdc, RGB(0, 0, 155)); SetBkColor(hdc, RGB(220, 220, 220)); debugStr(4, 15, hdc, L"4: ");
	SetTextColor(hdc, RGB(155, 0, 0)); SetBkColor(hdc, RGB(220, 220, 220)); debugStr(5, 16, hdc, L"5: ");
	SetTextColor(hdc, RGB(0, 155, 155)); SetBkColor(hdc, RGB(220, 220, 220)); debugStr(6, 17, hdc, L"6: ");
	SetTextColor(hdc, RGB(0, 0, 0)); SetBkColor(hdc, RGB(220, 220, 220)); debugStr(7, 18, hdc, L"7: ");
	SetTextColor(hdc, RGB(155, 155, 155)); SetBkColor(hdc, RGB(220, 220, 220)); debugStr(8, 19, hdc, L"8: ");
	//SetTextColor(hdc, RGB(255, 255, 255)); SetBkColor(hdc, RGB(0, 0, 0)); debugStr(pState->map.FieldValue[0][0] & FV_Pushed, 20, hdc, L"Field[0][0] | FV_Pushed: ");
	//SetTextColor(hdc, RGB(255, 255, 255)); SetBkColor(hdc, RGB(0, 0, 0)); debugStr(pState->map.FieldValue[0][0] & FV_Clear, 21, hdc, L"Field[0][0] | FV_Clear: ");

	ReleaseDC(hwnd, hdc);
	return 0;
}