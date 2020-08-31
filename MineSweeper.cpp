#ifndef UNICODE
#define UNICODE
#endif

#include <Windows.h>
#include <windowsx.h>

struct GridInfo
{
	long gx, gy;
	int cx, cy, w, h;
};
struct StateInfo
{

};

LRESULT CALLBACK HandleMessages(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR cmdLine, int nCmdShow)
{
	StateInfo* pState = new StateInfo;
	
	const wchar_t CLASS_NAME[] = L"MineSweeperWindowClass";

	WNDCLASS wc = {};
	wc.lpfnWndProc = HandleMessages;
	wc.lpszClassName = CLASS_NAME;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

	RegisterClassW(&wc);

	HWND hWnd = CreateWindowW(
		CLASS_NAME,
		L"MineSweeper",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, &pState
	);

	ShowWindow(hWnd, nCmdShow);

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
	switch (uMsg)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	return 0;
	}
	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}