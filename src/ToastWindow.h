#pragma once
#include <windows.h>
#include <string>

class ToastWindow {
public:
    static void Show(HINSTANCE hInstance, const std::wstring& folderPath);
private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static HWND s_hwnd;
    static std::wstring s_folderPath;
    static const UINT_PTR TIMER_ID = 100;
    static bool s_bHovering;
};