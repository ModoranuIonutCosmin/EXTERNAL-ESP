#pragma once
#include <Windows.h>
#include "Game.h"
class Window
{
public:

	Window(HINSTANCE hs);
	bool InitializeWindow();
	void Run();

	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HINSTANCE Inst;
	std::unique_ptr<ESPOverlay> g_game=std::make_unique<ESPOverlay>();
	HWND hwnd;
};

