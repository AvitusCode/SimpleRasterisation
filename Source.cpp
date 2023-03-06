
// WinApi
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>

#include "Display.h"
#include "Model.h"
#include "renderer.h"

constexpr int WIDTH  = 800;
constexpr int HEIGHT = 800;
constexpr int MAX_WIDTH = 1920;
constexpr int MAX_HEIGHT = 1080;
constexpr int DEPTH = 255;
constexpr int PIXEL_SIZE = 1;

using namespace jd;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

bool jdCreateWindow(HINSTANCE hInstance, PSTR szCmdLine, int iCmdShow, HWND& hwnd, WNDCLASSEX& wndclass)
{
	const int clientWidth = WIDTH * PIXEL_SIZE;
	const int clientHeight = HEIGHT * PIXEL_SIZE;
	static TCHAR szAppName[] = _T("MainWindow");

	RECT rect = { 0, 0, clientWidth, clientHeight };

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_BYTEALIGNCLIENT;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = sizeof(jd::Window*) + sizeof(Display*) + sizeof(Model*);
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wndclass.lpszMenuName = L"";
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);

	hwnd = CreateWindow(szAppName,		 // window class name
		_T("SimpleRasterisation"),		 // window caption
		WS_OVERLAPPEDWINDOW,	 // window style
		CW_USEDEFAULT,			 // initial x position
		CW_USEDEFAULT,			 // initial y position
		clientWidth,			 // initial x size
		clientHeight,			 // initial y size
		NULL,					// parent window handle
		NULL,					// window menu handle
		hInstance,				 // program instance handle
		szCmdLine);					 // creation parameters

	if (!hwnd) {
		return false;
	}

	GetWindowRect(hwnd, &rect);
	rect.bottom = rect.left + clientHeight;
	rect.right = rect.top + clientWidth;
	AdjustWindowRect(&rect, GetWindowLong(hwnd, GWL_STYLE), 0);

	SetWindowPos(hwnd, 0, rect.left, rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top, 0);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	HWND hwnd;
	WNDCLASSEX wndclass;

	if (!jdCreateWindow(hInstance, szCmdLine, iCmdShow, hwnd, wndclass)) {
		return false;
	}

	Window* window = (Window*)GetWindowLongPtr(hwnd, sizeof(Display*));
	window->setHWND(hwnd);

	jd::startProgramm(*window);

	return window->getMSG().wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	POINT point;

	switch (iMsg)
	{
	case WM_CREATE:
	{
		Display* image = nullptr;
		Window* window = nullptr;

		try {
			image = new Display{ WIDTH, HEIGHT, DEPTH, PIXEL_SIZE, new jdByte[MAX_WIDTH * MAX_HEIGHT * 4], new jdByte[MAX_WIDTH * MAX_HEIGHT]};
			window = new Window{};
		}
		catch (std::bad_alloc& e) {
			MessageBoxA(hwnd, (LPCSTR)L"Memmory allocation error", (LPCSTR)L"Error", MB_OK | MB_ICONERROR);
			if (!window)
			{
				if (image) {
					imgFree(*image);
					delete image;
				}
			}
			PostMessage(hwnd, WM_DESTROY, 0, 0);
			break;
		}

		SetWindowLongPtr(hwnd, 0, (LONG_PTR)image);
		SetWindowLongPtr(hwnd, sizeof(Display*), (LONG_PTR)window);
		break;
	}

	case WM_LBUTTONDOWN:
	{
		Window* window = (Window*)GetWindowLongPtr(hwnd, sizeof(Display*));
		window->setMove(true);
		RECT& WinRect = window->getRect();
		POINT& MouseInWin = window->getMouseIn();
		GetWindowRect(hwnd, &WinRect);

		point.x = GET_X_LPARAM(lParam);
		point.y = GET_Y_LPARAM(lParam);
		ClientToScreen(hwnd, (LPPOINT)&point);

		MouseInWin.x = point.x - WinRect.left;
		MouseInWin.y = point.y - WinRect.top;

		SetCapture(hwnd);
		break;
	}
	case WM_MOUSEMOVE:
	{
		const Window* const window = (Window*)GetWindowLongPtr(hwnd, sizeof(Display*));
		GetCursorPos((LPPOINT)&point);
		if (window->getMove()) {
			const POINT& MouseInWin = window->getMouseIn();
			const RECT& WinRect = window->getRect();
			SetWindowPos(hwnd, 0,
				point.x - MouseInWin.x,
				point.y - MouseInWin.y,
				WinRect.right - WinRect.left,
				WinRect.bottom - WinRect.top, 0);
			UpdateWindow(hwnd);
		}
		break;
	}
	case WM_LBUTTONUP:
	{
		Window* window = (Window*)GetWindowLongPtr(hwnd, sizeof(Display*));
		window->setMove(false);
		ReleaseCapture();
		break;
	}
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostMessage(hwnd, WM_DESTROY, 0, 0);
		}
		break;
	case WM_SIZE:
	{
		Display* image = (Display*)GetWindowLongPtr(hwnd, 0);
	
		image->width = LOWORD(lParam);
		image->height = HIWORD(lParam);

		UpdateWindow(hwnd);
		break;
	}
	case WM_DESTROY:
	{
		Display* image = (Display*)GetWindowLongPtr(hwnd, 0);
		Window* window = (Window*)GetWindowLongPtr(hwnd, sizeof(Display*));
		imgFree(*image);
		delete window;
		delete image;
		PostQuitMessage(0);
		ExitProcess(0);
		break;
	}
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}
