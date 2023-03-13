#include "Display.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm")

namespace jd
{
	HWND Window::getHWND() const {
		return wnd;
	}

	void Window::setHWND(HWND _wnd) {
		wnd = _wnd;
	}

	POINT& Window::getMouseIn() {
		return mouseInWin;
	}

	RECT& Window::getRect() {
		return winRect;
	}

	const POINT& Window::getMouseIn() const {
		return mouseInWin;
	}

	const RECT& Window::getRect() const {
		return winRect;
	}

	bool Window::getMove() const {
		return moveWin;
	}

	void Window::setMove(bool mv) {
		moveWin = mv;
	}

	void Window::drawBuffer(const Display& image) const
	{
		BITMAPINFO bitmapInfo;
		HDC DC = GetDC(wnd);

		bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfo.bmiHeader.biWidth = image.width;
		bitmapInfo.bmiHeader.biHeight = -image.height;
		bitmapInfo.bmiHeader.biPlanes = 1;
		bitmapInfo.bmiHeader.biBitCount = 32;
		bitmapInfo.bmiHeader.biCompression = BI_RGB;
		bitmapInfo.bmiHeader.biSizeImage = image.width * image.height * 4;

		StretchDIBits(DC,
			0, 0, image.width * image.pixel, image.height * image.pixel,
			0, 0, image.width, image.height,
			image.displayBitMap,
			&bitmapInfo,
			DIB_RGB_COLORS,
			SRCCOPY);

		ReleaseDC(wnd, DC);
	}


	MSG Window::getMSG() const {
		return msg;
	}

	void Window::setMSG(MSG _msg) {
		msg = _msg;
	}

	void Window::setPause(DWORD value)
	{
		if (value == 0) {
			value = 1;
		}

		curTime = value;

		timeBeginPeriod(1);
		oldTime = timeGetTime() + curTime;
	}

	void Window::wait()
	{
		// main loop
		while (PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE) != 0)
		{
			if (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		while (timeGetTime() < oldTime) {
			Sleep(1); // wait until next frame
		}
		oldTime = timeGetTime() + curTime;
	}

	bool Window::work() const {
		return renderLoop;
	}

	void Window::setStop() {
		renderLoop = false;
	}
}