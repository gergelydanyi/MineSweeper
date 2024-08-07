﻿#ifndef UNICODE
#define UNICODE
#endif

#include <Windows.h>
#include <windowsx.h>
#include "resource.h"
#include <string>
#include <time.h>
#include "StateInfo.h"

// TODO: Move graphical functions into a separate class

LRESULT CALLBACK HandleMessages(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void DrawCustomTitleBar(HWND hwnd, HDC hdc);
void DrawCustomControlButtons(HWND hwnd, HDC hdc);
void DrawCustomBorder(HWND hwnd, HDC hdc);
void InvalidateNeighbourhood(HWND, LPARAM, StateInfo*);
int DrawField(HDC hdc, StateInfo* pState, int i, int j, bool cascade = true);
int DrawFlag(HDC hdc, StateInfo* pState, int i, int j);
int DrawMine(HDC hdc, StateInfo* pState, int i, int j);
int DrawMap(HDC hdc, StateInfo* pState, HRGN hrgn);
int debug(HWND hwnd, StateInfo* pState);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR cmdLine, int nCmdShow)
{
	srand(time(0));
	StateInfo* pState = new StateInfo;
	// DONE: insert Clear function to the constructor of StateInfo class

	if(pState == NULL)
	{
		return 0;
	}

	const wchar_t CLASS_NAME[] = L"MineSweeperWindowClass";

	WNDCLASS wc = {};
	wc.lpfnWndProc = HandleMessages;
	wc.lpszClassName = CLASS_NAME;
	wc.hInstance = hInstance;
	//wc.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
	wc.hbrBackground = CreateSolidBrush(RGB(100, 30, 10));
	wc.lpszMenuName = MAKEINTRESOURCEW(IDR_MENU1);
	//wc.hCursor = (HCURSOR)IDC_ARROW;									// this caused an unknown error for some reason

	RegisterClassW(&wc);
	// TODO: Create function which gives back the dimensions of the window
	HWND hWnd = CreateWindowW(
		CLASS_NAME,
		L"MineSweeper",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 235, 290,
		NULL, NULL, hInstance, pState
	);
	pState->mainWindow = hWnd;

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
		pState->NewGame(9, 9, 10, 0, 0);
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
						pState->NewGame(9, 9, 10, 0, 0);
						pState->NG = true;
					}
					return 0;
					case ID_NEW_NORMAL:
					{
						pState->NewGame(16, 16, 40, 0, 0);
						pState->NG = true;
					}
					return 0;
					case ID_NEW_HARD:
					{
						pState->NewGame(30, 16, 99, 0, 0);
						pState->NG = true;
					}
					return 0;
					case ID_NEW_CUSTOM:
					{
						// TODO: input dialog box is needed
						pState->NewGame(90, 50, 750, 0, 0);
						pState->NG = true;
					}
					return 0;
				}
			}
		}
		return 0;
		case WM_MOUSEMOVE:
		{
			pState->OnMouseMove(lParam);
			debug(hwnd, pState);
		}
		return 0;
		case WM_LBUTTONDOWN:
		{
			pState->OnLeftButtonDown(lParam);
			debug(hwnd, pState);
		}
		return 0;
		case WM_LBUTTONUP:
		{
			pState->OnLeftButtonUp(lParam);
			debug(hwnd, pState);
		}
		return 0;
		case WM_RBUTTONDOWN:
		{
			pState->OnRightButtonDown(lParam);
			InvalidateNeighbourhood(hwnd, lParam, pState);
			debug(hwnd, pState);
		}
		return 0;
		case WM_RBUTTONUP:
		{
			pState->OnRightButtonUp(lParam);
			debug(hwnd, pState);
		}
		return 0;
		case WM_KEYDOWN:
		{
			if (wParam == 0x41)
			{
				pState->AutoClick();
			}
			if (wParam == 0x46)
			{
				pState->AutoFlag();
			}
		}
		return 0;
		case WM_TIMER:
		{
			// TODO: consider creating a Callback function for timer
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
			// TODO: Using bitmaps for rendering instead of direct drawing every time
			//RECT UpdateRect = {};
			//BOOL GetUpdateRectReturnValue = GetUpdateRect(hwnd, &UpdateRect, FALSE);
			BOOL GetUpdateRectReturnValue = GetUpdateRect(hwnd, NULL, FALSE);
			if (GetUpdateRectReturnValue)
			{
				PAINTSTRUCT ps;
				HRGN hrgn = CreateRectRgn(0, 0, 0, 0);
				int GetUpdateRgnReturnValue = GetUpdateRgn(hwnd, hrgn, TRUE);
				switch (GetUpdateRgnReturnValue)
				{
				case ERROR:
					break;
				case NULLREGION:
					break;
				case COMPLEXREGION:
				case SIMPLEREGION:
					HDC hdc = BeginPaint(hwnd, &ps);
					SelectObject(hdc, GetStockObject(DC_BRUSH));
					SelectObject(hdc, GetStockObject(DC_PEN));
					//DrawCustomTitleBar(hwnd, hdc);
					DrawMap(hdc, pState, hrgn);
					EndPaint(hwnd, &ps);
					break;
				}
				DeleteObject(hrgn);
			}
		}
		return 0;
		case WM_NCPAINT:
		{
			// TODO: Clear up this section to work properly
			//DefWindowProcW(hwnd, uMsg, wParam, lParam);
			HDC hdc = GetWindowDC(hwnd);
			DrawCustomTitleBar(hwnd, hdc);
			DrawCustomBorder(hwnd, hdc);
			ReleaseDC(hwnd, hdc);
		}
		return 0;
		case WM_NCACTIVATE:
		{
			return TRUE;
		}
		return 0;
		case WM_NCLBUTTONDOWN :
		{
			if (wParam == HTCLOSE)
			{
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
			else if (wParam == HTMINBUTTON)
			{
				ShowWindow(hwnd, SW_MINIMIZE);
			}
			else if (wParam == HTMAXBUTTON)
			{
				ShowWindow(hwnd, IsZoomed(hwnd) ? SW_RESTORE : SW_MAXIMIZE);
			}
			else
			{
				DefWindowProc(hwnd, uMsg, wParam, lParam);
			}
		}
		return 0;
		/*case WM_SETCURSOR:
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
		return 0;*/
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		return 0;
	}
	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

void DrawCustomTitleBar(HWND hwnd, HDC hdc)
{
	RECT rect;
	bool chatGPTmethod = false;
	if (chatGPTmethod)
	{
		GetWindowRect(hwnd, &rect);
		OffsetRect(&rect, -rect.left, -rect.top);
		rect.bottom = GetSystemMetrics(SM_CYCAPTION);
	}
	else
	{
		TITLEBARINFOEX titleBarInfoEx;
		titleBarInfoEx.cbSize = sizeof(TITLEBARINFOEX);
		SendMessageW(hwnd, WM_GETTITLEBARINFOEX, 0, (LPARAM)&titleBarInfoEx);
		rect = titleBarInfoEx.rcTitleBar;
		OffsetRect(&rect, -rect.left, -rect.top);
	}
	HBRUSH brush = CreateSolidBrush(RGB(10, 100, 10));
	FillRect(hdc, &rect, brush);
	DeleteObject(brush);

	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkMode(hdc, TRANSPARENT);
	DrawText(hdc, L"Minesweeper", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	DrawCustomControlButtons(hwnd, hdc);
}

void DrawCustomControlButtons(HWND hwnd, HDC hdc)
{
	RECT rect;
	GetWindowRect(hwnd, &rect);
	OffsetRect(&rect, -rect.left, -rect.top);
	rect.bottom = GetSystemMetrics(SM_CYCAPTION);

	int buttonWidth = GetSystemMetrics(SM_CXSIZE);
	int buttonHeight = rect.bottom;

	RECT closeRect = { rect.right - buttonWidth, 0, rect.right, buttonHeight };
	HBRUSH closeBrush = CreateSolidBrush(RGB(0, 50, 10));
	FillRect(hdc, &closeRect, closeBrush);
	DeleteObject(closeBrush);
	DrawText(hdc, L"X", -1, &closeRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT maxRect = { rect.right - 2 * buttonWidth, 0, rect.right - buttonWidth, buttonHeight };
	HBRUSH maxBrush = CreateSolidBrush(RGB(20, 70, 10));
	FillRect(hdc, &maxRect, maxBrush);
	DeleteObject(maxBrush);
	DrawText(hdc, L"■", -1, &maxRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT minRect = { rect.right - 3 * buttonWidth, 0, rect.right - 2 * buttonWidth, buttonHeight };
	HBRUSH minBrush = CreateSolidBrush(RGB(40, 70, 10));
	FillRect(hdc, &minRect, minBrush);
	DeleteObject(minBrush);
	DrawText(hdc, L"_", -1, &minRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void DrawCustomBorder(HWND hwnd, HDC hdc)
{
	RECT rect;
	GetWindowRect(hwnd, &rect);
	OffsetRect(&rect, -rect.left, -rect.top);

	HBRUSH borderBrush = CreateSolidBrush(RGB(100, 20, 200));
	FrameRect(hdc, &rect, borderBrush);
	DeleteObject(borderBrush);
}

void InvalidateNeighbourhood(HWND hwnd, LPARAM lParam, StateInfo* pState)
{
	long mouseX = GET_X_LPARAM(lParam);
	long mouseY = GET_Y_LPARAM(lParam);
	int x = (mouseX - pState->grid.gx) / pState->grid.w;
	int y = (mouseY - pState->grid.gy) / pState->grid.h;
	Field& field = pState->map(x, y);
	for (Field* f : field.Neighbours())
	{
		RECT r = pState->grid.GetRect(f->X, f->Y);
		InvalidateRect(hwnd, &r, TRUE);
	}
}

int DrawField(HDC hdc, StateInfo* pState, int i, int j, bool cascade)
{
	long gx = pState->grid.gx;
	long gy = pState->grid.gy;
	int w = pState->grid.w;
	int h = pState->grid.h;
	if (pState->map(i, j).isPushed())						//button pushed
	{
		SetDCBrushColor(hdc, clr_GridBack);
		SetDCPenColor(hdc, clr_GridLine);
		Rectangle(hdc, gx + i * w, gy + j * h, gx + i * w + w + 1, gy + j * h + h + 1);
	}
	else if (pState->map(i, j).isClear())					//unhidden area
	{
			SetDCBrushColor(hdc, clr_GridBack);
			SetDCPenColor(hdc, clr_GridLine);
			SetBkColor(hdc, clr_GridBack);
			if (pState->map(i, j).hasMine() && !pState->GO)	//there is a mine
			{
				SetDCBrushColor(hdc, clr_MineBackGO);
				SetDCPenColor(hdc, clr_MineBackGO);
			}
			else if (pState->map(i, j).SurroundingFlags() < pState->map(i, j).SurroundingMines())
			{
				SetDCBrushColor(hdc, clr_GridBackRed);
				SetBkColor(hdc, clr_GridBackRed);
			}
			else if (pState->map(i, j).SurroundingFlags() == pState->map(i, j).SurroundingMines())
			{
				SetDCBrushColor(hdc, clr_GridBackGreen);
				SetBkColor(hdc, clr_GridBackGreen);
			}
			else
			{
				SetDCBrushColor(hdc, clr_GridBackBlue);
				SetBkColor(hdc, clr_GridBackBlue);
			}
			Rectangle(hdc, gx + i * w, gy + j * h, gx + i * w + w, gy + j * h + h);
			SetTextAlign(hdc, TA_CENTER);
			TEXTMETRICW tm;
			GetTextMetricsW(hdc, &tm);
			int dx = 10, dy = 10 - tm.tmHeight / 2;
			short mines = pState->map(i, j).SurroundingMines();
			if (mines > 0)
			{
				SetTextColor(hdc, pState->colors[mines - 1]);
				TextOutW(hdc, gx + dx + i * w, gy + dy + j * h, (LPCWSTR)pState->fieldTexts[mines - 1], 1);
			}
			if (pState->map(i, j).hasMine())
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
		if (pState->map(i, j).hasFlag())
		{
			SetDCBrushColor(hdc, clr_GridBackGreen);
			SetDCPenColor(hdc, clr_GridBackGreen);
		}
		if (false)
		{
			Field* hiddenField = &(pState->map(i, j));
			if (!hiddenField->isClear() && hiddenField->SurroundingVisibleFields() != 0)
			{
				for (Field* field : hiddenField->Neighbours())
				{
					if (field->SurroundingMines() == field->SurroundingHiddenFields())
					{
						SetDCBrushColor(hdc, clr_BtnFaceForbidden);
						SetDCPenColor(hdc, clr_BtnFaceForbidden);
						break;
					}
					/*else if (field->SurroundingMines() == field->SurroundingFlags() && !hiddenField->hasFlag())
					{
						SetDCBrushColor(hdc, clr_BtnFaceSafe);
						SetDCPenColor(hdc, clr_BtnFaceSafe);
						break;
					}*/
				}
			}
		}
		Field* activeField = pState->GetActiveField();
		if (i == activeField->X && j == activeField->Y && activeField->SurroundingVisibleFields() == 0)
		{
			SetDCBrushColor(hdc, clr_BtnFaceFocus);
			SetDCPenColor(hdc, clr_BtnFaceFocus);
		}
		int border = 2;
		Rectangle(hdc, gx + i * w + border, gy + j * h + border, gx + i * w + w - border, gy + j * h + h - border);
		if (pState->map(i, j).hasFlag())
		{
			DrawFlag(hdc, pState, i, j);
		}
	}

	return 0;
}

int DrawMine(HDC hdc, StateInfo* pState, int i, int j)
{
	// TODO: Use a bitmap instead
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
	// TODO: Use a bitmap instead
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
	HRGN hrgnDst = CreateRectRgn(0, 0, 0, 0);
	HRGN hrgnSrc1 = hrgn;
	HRGN hrgnSrc2 = CreateRectRgn(0, 0, 0, 0);
	for (int j = 0; j < cy; ++j)
	{
		for (int i = 0; i < cx; ++i)
		{
			SetRectRgn(
				hrgnSrc2,
				gx + i * w,
				gy + j * h,
				gx + i * w + w,
				gy + j * h + h);
			if (CombineRgn(hrgnDst, hrgnSrc1, hrgnSrc2, RGN_AND) == SIMPLEREGION)
			{
				DrawField(hdc, pState, i, j);
			}
		}
	}
	DeleteObject(hrgnSrc2);
	DeleteObject(hrgnDst);
	return 0;
}

int debugStr(long k, int i, HDC hdc, const wchar_t* s)
{
	// TODO: It is unnecessary in this form, delete it
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
	// TODO: It is unnecessary in this form, delete it
	return 0;
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
	//SetTextColor(hdc, RGB(255, 255, 255)); SetBkColor(hdc, RGB(0, 0, 0)); debugStr(pState->map.FieldValue[0][0] & FV_Unhidden, 21, hdc, L"Field[0][0] | FV_Unhidden: ");

	ReleaseDC(hwnd, hdc);
	return 0;
}