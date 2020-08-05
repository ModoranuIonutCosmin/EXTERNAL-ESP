#include "pch.h"
#include "Window.h"
#include "Game.h"

Window::Window(HINSTANCE hs)
{
	this->Inst = hs;
    this->InitializeWindow();
}

bool Window::InitializeWindow()
{
    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if (FAILED(hr))
        return 1;

    // Register class and create window
    {
        // Register class
        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEXW);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = StaticWndProc;
        wcex.hInstance = Inst;
        wcex.hIcon = 0;
        wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)0;
        wcex.lpszClassName = L"DRAWING_CLASSWindowClass";
        wcex.hIconSm = 0;
        if (!RegisterClassExW(&wcex))
            return 1;

        // Create window
        int w, h;
        g_game->GetDefaultSize(w, h);

        RECT rc = { 0, 0, static_cast<LONG>(w), static_cast<LONG>(h) };

        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

        hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, L"DRAWING_CLASSWindowClass", L"DRAWING CLASS", WS_POPUP,
            CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, Inst,
            nullptr);
        // TODO: Change to CreateWindowExW(WS_EX_TOPMOST, L"DRAWING_CLASSWindowClass", L"DRAWING CLASS", WS_POPUP,
        // to default to fullscreen.

        if (!hwnd)
            return 1;

        SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
        ShowWindow(hwnd,SW_SHOWNORMAL);
        // TODO: Change nCmdShow to SW_SHOWMAXIMIZED to default to fullscreen.

        //SetWindowLongW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(g_game.get()));

        g_game->Initialize(this->hwnd);
    }
    return TRUE;
}

void Window::Run()
{
    MSG msg = {};
    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            g_game->Tick();
        }
        if (WM_QUIT == msg.message)
            return;
    }
}

LRESULT Window::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   
    Window* wnd = 0;
    if (message == WM_NCCREATE) 
    {
        SetWindowLong(hWnd, GWL_USERDATA, long((LPCREATESTRUCT(lParam))->lpCreateParams));
    }
    wnd = (Window*)(GetWindowLong(hWnd, GWL_USERDATA));

    if (wnd)
    {
        _asm nop;
        wnd->WndProc(hWnd, message, wParam, lParam);
    }
    else
        return DefWindowProc(hWnd, message, wParam, lParam);
    
}

LRESULT Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_PAINT:
        if  (g_game)
        {
            g_game->Tick();
        }
        else
        {
            PAINTSTRUCT ps;
            (void)BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;


    case WM_DESTROY:
        PostQuitMessage(0);
        break;


    case WM_MENUCHAR:
        // A menu is active and the user presses a key that does not correspond
        // to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
        return MAKELRESULT(0, MNC_CLOSE);
    }
    g_game.reset();

    CoUninitialize();
    return DefWindowProc(hWnd, message, wParam, lParam);
}
