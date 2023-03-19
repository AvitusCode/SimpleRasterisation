#pragma once
#include <windows.h>
#include "Model.h"
#include "allocator\stack_allocator.h"

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

		void setAllocatorSize() {
			sa.setMemTotal(win_config.d_MEMORY_SIZE);
			sa.init();
		}

		auto& getAllocator() {
			return sa;
		}

		// END TEST SECTION

		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 800;
		static constexpr int MAX_WIDTH = 1920;
		static constexpr int MAX_HEIGHT = 1080;
		static constexpr int DEPTH = 255;
		static constexpr int PIXEL_SIZE = 1;
		static constexpr int MEMORY_SIZE = 10'000;

		struct WindowConfig
		{
			int d_WIDTH = WIDTH;
			int d_HEIGHT = HEIGHT;
			int d_MAX_WIDTH = MAX_WIDTH;
			int d_MAX_HEIGHT = MAX_HEIGHT;
			int d_DEPTH = DEPTH;
			int d_PIXEL_SIZE = PIXEL_SIZE;
			int d_MEMORY_SIZE = MEMORY_SIZE;
		};

		WindowConfig& getWindowConfig() {
			return win_config;
		}

		const WindowConfig& getWindowConfig() const {
			return win_config;
		}

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

		WindowConfig win_config;
		jd::mem::StackAllocator sa{};
	};
}