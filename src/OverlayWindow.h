#pragma once
#include <windows.h>
#include <vector>

class OverlayWindow {
public:
    enum CaptureMode { MODE_REGION, MODE_WINDOW, MODE_FULLSCREEN };

    static void ShowOverlay(HINSTANCE hInstance);
    static void HideOverlay();
private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static HWND s_hwnd;
    static HBITMAP s_hbmScreen;
    static bool s_bDragging;
    static POINT s_ptStart;
    static POINT s_ptEnd;
    
    static CaptureMode s_currentMode;
    static int s_hoveredButton;
    static RECT s_rectHovered;
    static std::vector<RECT> s_windowRects;
    
    static void CaptureScreen();
    static void DrawOverlay(HWND hwnd, HDC hdc);
    static void ProcessSelection(int x, int y, int w, int h);
    static void DrawToolbar(void* graphicsPtr, int screenW);
    static int HitTestToolbar(int x, int y, int screenW);
};