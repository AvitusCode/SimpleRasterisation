#pragma once
#include <windows.h>
#include <windowsx.h>

// MENU defines
#define CUBE_DEMO        1
#define TRIANGULATE_DEMO 2
#define FRACTAL_DEMO     3
#define PROGRAM_EXIT     4

#define PROGRAM_INFO     8

void WndProcMenu(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void WndAddMenus(HWND hwnd);