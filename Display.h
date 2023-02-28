#pragma once
#include <windows.h>
#include "Model.h"

namespace jd
{
	class Window
	{
	public:
		Window() = default;

		MSG getMSG() const;
		void setMSG(MSG _msg);
		HWND getHWND() const;
		void setHWND(HWND _wnd);

		POINT& getMouseIn();
		RECT& getRect();
		const POINT& getMouseIn() const;
		const RECT& getRect() const;
		bool getMove() const;
		void setMove(bool mv);

		void drawBuffer(const Display& image) const;
		void setPause(DWORD value);
		void wait();

	private:
		HWND wnd;
		MSG msg;

		POINT mouseInWin;
		RECT winRect;
		bool moveWin = false;

		DWORD curTime;
		DWORD oldTime;
	};
}