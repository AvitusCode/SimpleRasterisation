#pragma once
#include <windows.h>
#include "Model.h"

namespace jd
{
	enum class RENDER : uint8_t
	{
		CUBE,
		TRIANGE,
		FRACTAL
	};

	enum class BUTTON : uint8_t
	{
		NOTHING,
		RIGHT,
		LEFT
	};

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

		bool work() const;
		void setStop();

		// TEST SECTION
		void setRender(RENDER rend) {
			render = rend;
		}
		RENDER getRender() const {
			return render;
		}
		void setButtonClick(BUTTON b) {
			if (button == BUTTON::NOTHING || b == BUTTON::NOTHING) {
				button = b;
			}
		}
		BUTTON getButtonClick() const {
			return button;
		}

		// END TEST SECTION

	private:
		HWND wnd;
		MSG msg;

		POINT mouseInWin;
		RECT winRect;
		bool moveWin = false;

		DWORD curTime;
		DWORD oldTime;

		RENDER render = RENDER::CUBE;
		BUTTON button = BUTTON::NOTHING;

		bool renderLoop = true;
	};
}