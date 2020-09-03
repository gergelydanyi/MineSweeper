#ifndef UNICODE
#define UNICODE
#endif

#include <Windows.h>
#include <windowsx.h>
#include "resource.h"
#include <string>

// Field values for 

constexpr auto FV_Mine		= 0x0001;
constexpr auto FV_Flag		= 0x0002;
constexpr auto FV_Ask		= 0x0004;
constexpr auto FV_0			= 0x0008;
constexpr auto FV_1			= 0x0010;
constexpr auto FV_2			= 0x0020;
constexpr auto FV_3			= 0x0040;
constexpr auto FV_4			= 0x0080;
constexpr auto FV_5			= 0x0100;
constexpr auto FV_6			= 0x0200;
constexpr auto FV_7			= 0x0400;
constexpr auto FV_8			= 0x0800;
constexpr auto FV_Clear		= 0x1000;
constexpr auto FV_Change	= 0x2000;

struct MapInfo
{
	const int Xmax = 100;
	const int Ymax = 100;
	int sizeX = 15;
	int sizeY = 20;
	short int FieldValue[100][100] = {};
};
struct GridInfo
{
	long gx = 20, gy = 40;
	int cx = 15, cy = 20, w = 20, h = 20;
};
struct MouseInfo
{
	long LDX, LDY, LUX, LUY, RDX, RDY, RUX, RUY, XP, YP, X, Y;
};
struct StateInfo
{
	GridInfo grid;
	MapInfo map;
	MouseInfo mouse;
	bool DEBUG = true;
	int c = 0;
};

LRESULT CALLBACK HandleMessages(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int NewGame(HWND hwnd, StateInfo* pState, int sizeX, int sizeY, int cMine);
int ClearMap(StateInfo* pState);
int DrawField(HDC hdc, StateInfo* pState, int i, int j);
int DrawMap(HDC hdc, StateInfo* pState, HRGN hrgn);
int FillMap(StateInfo* pState, int a, int b, int cMine);
int FillMapWithNumbers(StateInfo* pState);
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
	//wc.hCursor = (HCURSOR)IDC_ARROW;

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
						NewGame(hwnd, pState, 9, 9, 10);
					}
					return 0;
					case ID_NEW_NORMAL:
					{
						NewGame(hwnd, pState, 16, 16, 40);
					}
					return 0;
					case ID_NEW_HARD:
					{
						NewGame(hwnd, pState, 30, 16, 99);
					}
					return 0;
					case ID_NEW_CUSTOM:
					{
						NewGame(hwnd, pState, 10, 10, 50);
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
			int fx = (pState->mouse.LDX - pState->grid.gx) / pState->grid.w;
			int fy = (pState->mouse.LDY - pState->grid.gy) / pState->grid.h;
			debug(hwnd, pState);
			RECT r = {
				{r.left = pState->grid.gx + fx * pState->grid.w},
				{r.top = pState->grid.gy + fy * pState->grid.h},
				{r.right = pState->grid.gx + ++fx * pState->grid.w},
				{r.bottom = pState->grid.gy + ++fy * pState->grid.h} };
			InvalidateRect(hwnd, &r, TRUE);
		}
		case WM_LBUTTONUP:
		{
			pState->mouse.LUX = GET_X_LPARAM(lParam);
			pState->mouse.LUY = GET_Y_LPARAM(lParam);
			int fx = (pState->mouse.LDX - pState->grid.gx) / pState->grid.w;
			int fy = (pState->mouse.LDY - pState->grid.gy) / pState->grid.h;
			debug(hwnd, pState);
			pState->map.FieldValue[fx][fy] = pState->map.FieldValue[fx][fy] | FV_Clear | FV_Change;
			RECT r = {
				{r.left = pState->grid.gx + fx * pState->grid.w},
				{r.top = pState->grid.gy + fy * pState->grid.h},
				{r.right = pState->grid.gx + ++fx * pState->grid.w},
				{r.bottom = pState->grid.gy + ++fy * pState->grid.h} };
			InvalidateRect(hwnd, &r, TRUE);
		}
		return 0;
		case WM_RBUTTONDOWN:
		{
			pState->mouse.RDX = GET_X_LPARAM(lParam);
			pState->mouse.RDY = GET_Y_LPARAM(lParam);
			int fx = (pState->mouse.LDX - pState->grid.gx) / pState->grid.w;
			int fy = (pState->mouse.LDY - pState->grid.gy) / pState->grid.h;
			debug(hwnd, pState);
			RECT r = {
				{r.left = pState->grid.gx + fx * pState->grid.w},
				{r.top = pState->grid.gy + fy * pState->grid.h},
				{r.right = pState->grid.gx + ++fx * pState->grid.w},
				{r.bottom = pState->grid.gy + ++fy * pState->grid.h} };
			InvalidateRect(hwnd, &r, TRUE);
		}
		case WM_RBUTTONUP:
		{
			pState->mouse.RUX = GET_X_LPARAM(lParam);
			pState->mouse.RUY = GET_Y_LPARAM(lParam);
			int fx = (pState->mouse.LDX - pState->grid.gx) / pState->grid.w;
			int fy = (pState->mouse.LDY - pState->grid.gy) / pState->grid.h;
			debug(hwnd, pState);
			pState->map.FieldValue[fx][fy] = pState->map.FieldValue[fx][fy] | FV_Flag | FV_Change;
			RECT r = {
				{r.left = pState->grid.gx + fx * pState->grid.w},
				{r.top = pState->grid.gy + fy * pState->grid.h},
				{r.right = pState->grid.gx + ++fx * pState->grid.w},
				{r.bottom = pState->grid.gy + ++fy * pState->grid.h} };
			InvalidateRect(hwnd, &r, TRUE);
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
			else
			{
				HDC hdc = BeginPaint(hwnd, &ps);
				EndPaint(hwnd, &ps);
			}
		}
		return 0;
		case WM_SETCURSOR:
		{
			HCURSOR hc = NULL;
			switch (LOWORD(lParam))
			{
				case HTCLIENT:hc = LoadCursorW(NULL, IDC_CROSS); break;
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

int NewGame(HWND hwnd, StateInfo* pState, int sizeX, int sizeY, int cMine)
{
	pState->grid.cx = sizeX;
	pState->grid.cy = sizeY;
	pState->map.sizeX = sizeX;
	pState->map.sizeY = sizeY;
	ClearMap(pState);
	FillMap(pState, -1, pState->map.sizeX*pState->map.sizeY, cMine);
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
	pState->c++;
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
			if (!(pState->map.FieldValue[i][j] & FV_Mine))
			{
				if (i>0 && j>0 && pState->map.FieldValue[i - 1][j - 1] & FV_Mine) { ++cm; }
				if (j>0 && pState->map.FieldValue[i    ][j - 1] & FV_Mine) { ++cm; }
				if (i<pState->map.sizeX && j > 0 && pState->map.FieldValue[i + 1][j - 1] & FV_Mine) { ++cm; }
				if (i < pState->map.sizeX && pState->map.FieldValue[i + 1][j    ] & FV_Mine) { ++cm; }
				if (i < pState->map.sizeX && j < pState->map.sizeY && pState->map.FieldValue[i + 1][j + 1] & FV_Mine) { ++cm; }
				if (j < pState->map.sizeY && pState->map.FieldValue[i    ][j + 1] & FV_Mine) { ++cm; }
				if (i>0 && j < pState->map.sizeY && pState->map.FieldValue[i - 1][j + 1] & FV_Mine) { ++cm; }
				if (i>0 && pState->map.FieldValue[i - 1][j    ] & FV_Mine) { ++cm; }
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

	if (pState->map.FieldValue[i][j] & FV_Clear)
	{
		if (pState->map.FieldValue[i][j] & FV_Mine)
		{
			SetDCBrushColor(hdc, RGB(200, 30, 60));
		}
		else
		{
			SetDCBrushColor(hdc, RGB(255, 255, 255));
		}
		Rectangle(hdc,		gx + i * w,		gy + j * h,		gx + i * w + w + 1,		gy + j * h + h + 1);
		SetBkColor(hdc, RGB(255, 255, 255));
		SetTextColor(hdc, RGB(255, 0, 0));
		SetTextAlign(hdc, TA_CENTER);
		TEXTMETRICW tm;
		GetTextMetricsW(hdc, &tm);
		int dx = 10, dy = 10 - tm.tmHeight / 2;
		if (pState->map.FieldValue[i][j] & FV_0) { TextOutW(hdc, gx + dx + i * w, gy + dy + j * h, L"0", 1); }
		if (pState->map.FieldValue[i][j] & FV_1) { TextOutW(hdc, gx + dx + i * w, gy + dy + j * h, L"1", 1); }
		if (pState->map.FieldValue[i][j] & FV_2) { TextOutW(hdc, gx + dx + i * w, gy + dy + j * h, L"2", 1); }
		if (pState->map.FieldValue[i][j] & FV_3) { TextOutW(hdc, gx + dx + i * w, gy + dy + j * h, L"3", 1); }
		if (pState->map.FieldValue[i][j] & FV_4) { TextOutW(hdc, gx + dx + i * w, gy + dy + j * h, L"4", 1); }
		if (pState->map.FieldValue[i][j] & FV_5) { TextOutW(hdc, gx + dx + i * w, gy + dy + j * h, L"5", 1); }
		if (pState->map.FieldValue[i][j] & FV_6) { TextOutW(hdc, gx + dx + i * w, gy + dy + j * h, L"6", 1); }
		if (pState->map.FieldValue[i][j] & FV_7) { TextOutW(hdc, gx + dx + i * w, gy + dy + j * h, L"7", 1); }
		if (pState->map.FieldValue[i][j] & FV_8) { TextOutW(hdc, gx + dx + i * w, gy + dy + j * h, L"8", 1); }

	}
	else
	{
		SetDCBrushColor(hdc, RGB(255, 255, 255));
		SetDCPenColor(hdc, RGB(255, 255, 255));
		Rectangle(hdc,
			gx + i * w,
			gy + j * h,
			gx + i * w + w,
			gy + j * h + h);
		SetDCBrushColor(hdc, RGB(200, 200, 200));
		SetDCPenColor(hdc, RGB(200, 200, 200));
		POINT apt[3] = {};
		apt[0].x = gx + i * w + w;
		apt[0].y = gy + j * h;
		apt[1].x = apt[0].x;
		apt[1].y = gy + j * h + h;
		apt[2].x = gx + i * w;
		apt[2].y = apt[1].y;
		Polygon(hdc, apt, 3);
		SetDCBrushColor(hdc, RGB(220, 220, 220));
		SetDCPenColor(hdc, RGB(220, 220, 220));
		int border = 2;
		Rectangle(hdc, gx + i * w + border, gy + j * h + border, gx + i * w + w - border, gy + j * h + h - border);
	}

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
				gx + i * w + w + 1,
				gy + j * h + h + 1);
			if (CombineRgn(hrgnDst, hrgnSrc1, hrgnSrc2, RGN_AND) != NULLREGION)// && pState->map.FieldValue[i][j]) & FV_Change)
			{
				DrawField(hdc, pState, i, j);
				pState->map.FieldValue[i][j] = pState->map.FieldValue[i][j] ^ FV_Change;
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
	Rectangle(hdc, 690, 90, 850, 350);
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
	ReleaseDC(hwnd, hdc);
	return 0;
}