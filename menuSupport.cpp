#include "menuSupport.h"
#include "Display.h"
#include "Model.h"

void WndProcMenu(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	jd::Window* window = (jd::Window*)GetWindowLongPtr(hwnd, sizeof(jd::Display*));

	switch (id)
	{
	case PROGRAM_INFO:
		MessageBoxA(hwnd, "Avitus demo projects", "about", MB_OK);
		break;

	case CUBE_DEMO:
		window->setRender(jd::RENDER::CUBE);
		break;

	case TRIANGULATE_DEMO:
		window->setRender(jd::RENDER::TRIANGE);
		break;

	case FRACTAL_DEMO:
		window->setRender(jd::RENDER::FRACTAL);
		break;

	case PROGRAM_EXIT:
		PostMessage(hwnd, WM_DESTROY, 0, 0);
		break;
	}

	return FORWARD_WM_COMMAND(hwnd, id, hwndCtl, codeNotify, DefWindowProc);
}

void WndAddMenus(HWND hwnd)
{
	HMENU rootMenu = CreateMenu();
	HMENU subMenu = CreateMenu();

	AppendMenu(subMenu, MF_STRING, CUBE_DEMO, L"Cube demo");
	AppendMenu(subMenu, MF_STRING, TRIANGULATE_DEMO, L"Triag demo");
	AppendMenu(subMenu, MF_STRING, FRACTAL_DEMO, L"Fractal demo");
	AppendMenu(subMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(subMenu, MF_STRING, PROGRAM_EXIT, L"Exit");

	AppendMenu(rootMenu, MF_POPUP, (UINT_PTR)subMenu, L"File");
	AppendMenu(rootMenu, MF_STRING, PROGRAM_INFO, L"About");

	SetMenu(hwnd, rootMenu);
}