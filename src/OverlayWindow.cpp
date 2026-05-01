#include "OverlayWindow.h"
#include "ToastWindow.h"
#include "Config.h"
#include <windowsx.h>
#include <gdiplus.h>
#include <gdiplusheaders.h>
#include <string>

#pragma comment (lib,"Gdiplus.lib")
#pragma comment (lib,"Msimg32.lib")

// Helper para obtener el CLSID del codificador de imágenes (PNG, JPG)
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0, size = 0;
    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;
    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)malloc(size);
    if (pImageCodecInfo == NULL) return -1;
    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }
    free(pImageCodecInfo);
    return -1;
}

HWND OverlayWindow::s_hwnd = NULL;
HBITMAP OverlayWindow::s_hbmScreen = NULL;
bool OverlayWindow::s_bDragging = false;
POINT OverlayWindow::s_ptStart = {0};
POINT OverlayWindow::s_ptEnd = {0};

OverlayWindow::CaptureMode OverlayWindow::s_currentMode = OverlayWindow::MODE_REGION;
int OverlayWindow::s_hoveredButton = -1;
RECT OverlayWindow::s_rectHovered = {0};
std::vector<RECT> OverlayWindow::s_windowRects;

void OverlayWindow::CaptureScreen() {
    int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    HDC hScreenDC = GetDC(NULL);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
    s_hbmScreen = CreateCompatibleBitmap(hScreenDC, w, h);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, s_hbmScreen);

    BitBlt(hMemoryDC, 0, 0, w, h, hScreenDC, x, y, SRCCOPY);

    SelectObject(hMemoryDC, hOldBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);

    // Obtener ventanas para el modo "Ventana"
    s_windowRects.clear();
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        if (IsWindowVisible(hwnd) && !IsIconic(hwnd)) {
            RECT r;
            GetWindowRect(hwnd, &r);
            if (r.right - r.left > 0 && r.bottom - r.top > 0) {
                auto* vec = (std::vector<RECT>*)lParam;
                vec->push_back(r);
            }
        }
        return TRUE;
    }, (LPARAM)&s_windowRects);
}

void OverlayWindow::ShowOverlay(HINSTANCE hInstance) {
    if (s_hwnd != NULL) return;

    CaptureScreen();

    const wchar_t CLASS_NAME[] = L"OverlayWindowClass";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_CROSS); // Cursor en cruz
    RegisterClassW(&wc);

    int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    s_hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW, 
        CLASS_NAME, L"",
        WS_POPUP | WS_VISIBLE, 
        x, y, w, h,
        NULL, NULL, hInstance, NULL
    );

    SetWindowPos(s_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    ShowWindow(s_hwnd, SW_SHOW);
    UpdateWindow(s_hwnd);
}

void OverlayWindow::HideOverlay() {
    if (s_hwnd) {
        DestroyWindow(s_hwnd);
        s_hwnd = NULL;
    }
    if (s_hbmScreen) {
        DeleteObject(s_hbmScreen);
        s_hbmScreen = NULL;
    }
    s_bDragging = false;
}

void OverlayWindow::DrawToolbar(void* graphicsPtr, int screenW) {
    Gdiplus::Graphics& graphics = *(Gdiplus::Graphics*)graphicsPtr;
    
    int btnWidth = 100;
    int btnHeight = 40;
    int margin = 10;
    int totalWidth = (btnWidth * 3) + (margin * 4);
    
    int primaryW = GetSystemMetrics(SM_CXSCREEN);
    int startX = -GetSystemMetrics(SM_XVIRTUALSCREEN) + (primaryW / 2) - (totalWidth / 2);
    int startY = -GetSystemMetrics(SM_YVIRTUALSCREEN) + 20;

    Gdiplus::SolidBrush bgBrush(Gdiplus::Color(220, 11, 19, 38));
    Gdiplus::Pen borderPen(Gdiplus::Color(120, 218, 226, 253), 1);
    graphics.FillRectangle(&bgBrush, startX, startY, totalWidth, btnHeight + margin * 2);
    graphics.DrawRectangle(&borderPen, startX, startY, totalWidth, btnHeight + margin * 2);

    Gdiplus::FontFamily fontFamily(L"Segoe UI");
    Gdiplus::Font font(&fontFamily, 14, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);
    format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

    const wchar_t* labels[] = { L"Recorte", L"Ventana", L"Pantalla" };

    for (int i = 0; i < 3; i++) {
        int bx = startX + margin + i * (btnWidth + margin);
        int by = startY + margin;

        Gdiplus::Color btnColor;
        if (s_currentMode == i) {
            btnColor = Gdiplus::Color(255, 160, 120, 255);
        } else if (s_hoveredButton == i) {
            btnColor = Gdiplus::Color(255, 34, 42, 61);
        } else {
            btnColor = Gdiplus::Color(255, 19, 27, 46);
        }

        Gdiplus::SolidBrush btnBrush(btnColor);
        graphics.FillRectangle(&btnBrush, bx, by, btnWidth, btnHeight);
        
        // Un pequeño borde interior en los botones para dar un toque moderno
        Gdiplus::Pen btnBorder(Gdiplus::Color(100, 208, 188, 255), 1);
        if (s_currentMode != i) graphics.DrawRectangle(&btnBorder, bx, by, btnWidth, btnHeight);

        Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 218, 226, 253));
        graphics.DrawString(labels[i], -1, &font, Gdiplus::RectF((Gdiplus::REAL)bx, (Gdiplus::REAL)by, (Gdiplus::REAL)btnWidth, (Gdiplus::REAL)btnHeight), &format, &textBrush);
    }
}

int OverlayWindow::HitTestToolbar(int x, int y, int screenW) {
    int btnWidth = 100;
    int btnHeight = 40;
    int margin = 10;
    int totalWidth = (btnWidth * 3) + (margin * 4);
    int primaryW = GetSystemMetrics(SM_CXSCREEN);
    
    int startX = -GetSystemMetrics(SM_XVIRTUALSCREEN) + (primaryW / 2) - (totalWidth / 2);
    int startY = -GetSystemMetrics(SM_YVIRTUALSCREEN) + 20;

    if (x >= startX && x <= startX + totalWidth && y >= startY && y <= startY + btnHeight + margin * 2) {
        for (int i = 0; i < 3; i++) {
            int bx = startX + margin + i * (btnWidth + margin);
            int by = startY + margin;
            if (x >= bx && x <= bx + btnWidth && y >= by && y <= by + btnHeight) {
                return i;
            }
        }
        return -2; // Toolbar background
    }
    return -1; // No hit
}

void OverlayWindow::DrawOverlay(HWND hwnd, HDC hdc) {
    int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    // Doble buffer para evitar parpadeos
    HDC hMemDC = CreateCompatibleDC(hdc);
    HBITMAP hMemBmp = CreateCompatibleBitmap(hdc, w, h);
    HBITMAP hOldMemBmp = (HBITMAP)SelectObject(hMemDC, hMemBmp);

    // 1. Dibujar el fondo capturado
    HDC hScreenDC = CreateCompatibleDC(hdc);
    HBITMAP hOldScreen = (HBITMAP)SelectObject(hScreenDC, s_hbmScreen);
    BitBlt(hMemDC, 0, 0, w, h, hScreenDC, 0, 0, SRCCOPY);
    SelectObject(hScreenDC, hOldScreen);
    DeleteDC(hScreenDC);

    // 2. Usar GDI+ para oscurecer y dibujar el recorte
    Gdiplus::Graphics graphics(hMemDC);
    
    // Filtro oscuro (negro con 40% de opacidad)
    Gdiplus::SolidBrush darkBrush(Gdiplus::Color(100, 0, 0, 0));
    
    if (s_currentMode == MODE_REGION) {
        int selX = min(s_ptStart.x, s_ptEnd.x);
        int selY = min(s_ptStart.y, s_ptEnd.y);
        int selW = abs(s_ptEnd.x - s_ptStart.x);
        int selH = abs(s_ptEnd.y - s_ptStart.y);

        if (s_bDragging && selW > 0 && selH > 0) {
            Gdiplus::Region region(Gdiplus::Rect(0, 0, w, h));
            region.Exclude(Gdiplus::Rect(selX, selY, selW, selH));
            graphics.FillRegion(&darkBrush, &region);

            Gdiplus::Pen borderPen(Gdiplus::Color(255, 5, 102, 217), 2);
            graphics.DrawRectangle(&borderPen, selX, selY, selW, selH);
        } else {
            graphics.FillRectangle(&darkBrush, 0, 0, w, h);
        }
    } else if (s_currentMode == MODE_WINDOW) {
        if (s_rectHovered.right > s_rectHovered.left) {
            int hx = s_rectHovered.left - GetSystemMetrics(SM_XVIRTUALSCREEN);
            int hy = s_rectHovered.top - GetSystemMetrics(SM_YVIRTUALSCREEN);
            int hw = s_rectHovered.right - s_rectHovered.left;
            int hh = s_rectHovered.bottom - s_rectHovered.top;

            Gdiplus::Region region(Gdiplus::Rect(0, 0, w, h));
            region.Exclude(Gdiplus::Rect(hx, hy, hw, hh));
            graphics.FillRegion(&darkBrush, &region);

            Gdiplus::Pen borderPen(Gdiplus::Color(255, 5, 102, 217), 3);
            graphics.DrawRectangle(&borderPen, hx, hy, hw, hh);
        } else {
            graphics.FillRectangle(&darkBrush, 0, 0, w, h);
        }
    } else if (s_currentMode == MODE_FULLSCREEN) {
        // No oscurecemos o oscurecemos muy poco
        Gdiplus::SolidBrush lightBrush(Gdiplus::Color(30, 0, 0, 0));
        graphics.FillRectangle(&lightBrush, 0, 0, w, h);
    }

    // Dibujar la barra de herramientas siempre por encima
    DrawToolbar(&graphics, w);

    // Copiar el buffer a la ventana
    BitBlt(hdc, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);

    SelectObject(hMemDC, hOldMemBmp);
    DeleteObject(hMemBmp);
    DeleteDC(hMemDC);
}

LRESULT CALLBACK OverlayWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_SETCURSOR: {
            POINT pt;
            GetCursorPos(&pt);
            int hit = HitTestToolbar(pt.x - GetSystemMetrics(SM_XVIRTUALSCREEN), 
                                     pt.y - GetSystemMetrics(SM_YVIRTUALSCREEN), 
                                     GetSystemMetrics(SM_CXVIRTUALSCREEN));
            if (hit >= 0) {
                SetCursor(LoadCursor(NULL, IDC_HAND));
                return TRUE;
            }
            SetCursor(LoadCursor(NULL, IDC_CROSS));
            return TRUE;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            DrawOverlay(hwnd, hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            int mx = GET_X_LPARAM(lParam);
            int my = GET_Y_LPARAM(lParam);
            int hit = HitTestToolbar(mx, my, GetSystemMetrics(SM_CXVIRTUALSCREEN));
            if (hit >= 0) {
                s_currentMode = (CaptureMode)hit;
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }
            if (hit == -2) return 0; // Clicked on toolbar background

            if (s_currentMode == MODE_REGION) {
                s_bDragging = true;
                s_ptStart.x = mx;
                s_ptStart.y = my;
                s_ptEnd = s_ptStart;
                SetCapture(hwnd);
                InvalidateRect(hwnd, NULL, FALSE);
            } else if (s_currentMode == MODE_WINDOW) {
                if (s_rectHovered.right > s_rectHovered.left) {
                    int offsetX = -GetSystemMetrics(SM_XVIRTUALSCREEN);
                    int offsetY = -GetSystemMetrics(SM_YVIRTUALSCREEN);
                    int x = s_rectHovered.left + offsetX;
                    int y = s_rectHovered.top + offsetY;
                    int w = s_rectHovered.right - s_rectHovered.left;
                    int h = s_rectHovered.bottom - s_rectHovered.top;
                    ProcessSelection(x, y, w, h);
                    HideOverlay();
                }
            } else if (s_currentMode == MODE_FULLSCREEN) {
                int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
                int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
                ProcessSelection(0, 0, w, h);
                HideOverlay();
            }
            return 0;
        }
        case WM_MOUSEMOVE: {
            int mx = GET_X_LPARAM(lParam);
            int my = GET_Y_LPARAM(lParam);

            int hit = HitTestToolbar(mx, my, GetSystemMetrics(SM_CXVIRTUALSCREEN));
            if (hit != s_hoveredButton) {
                s_hoveredButton = hit;
                InvalidateRect(hwnd, NULL, FALSE);
            }

            if (s_currentMode == MODE_REGION) {
                if (s_bDragging) {
                    s_ptEnd.x = mx;
                    s_ptEnd.y = my;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            } else if (s_currentMode == MODE_WINDOW && !s_bDragging) {
                int hit = HitTestToolbar(mx, my, GetSystemMetrics(SM_CXVIRTUALSCREEN));
                if (hit != -1) {
                    memset(&s_rectHovered, 0, sizeof(RECT));
                    InvalidateRect(hwnd, NULL, FALSE);
                    return 0;
                }
                
                POINT screenPt = { mx + GetSystemMetrics(SM_XVIRTUALSCREEN), my + GetSystemMetrics(SM_YVIRTUALSCREEN) };
                RECT found = {0};
                for (const auto& r : s_windowRects) {
                    if (PtInRect(&r, screenPt)) {
                        found = r;
                        break;
                    }
                }
                
                if (memcmp(&found, &s_rectHovered, sizeof(RECT)) != 0) {
                    s_rectHovered = found;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            if (s_currentMode == MODE_REGION && s_bDragging) {
                s_bDragging = false;
                ReleaseCapture();
                s_ptEnd.x = GET_X_LPARAM(lParam);
                s_ptEnd.y = GET_Y_LPARAM(lParam);
                InvalidateRect(hwnd, NULL, FALSE);
                
                int selX = min(s_ptStart.x, s_ptEnd.x);
                int selY = min(s_ptStart.y, s_ptEnd.y);
                int selW = abs(s_ptEnd.x - s_ptStart.x);
                int selH = abs(s_ptEnd.y - s_ptStart.y);
                
                ProcessSelection(selX, selY, selW, selH);
                
                HideOverlay();
            }
            return 0;
        }
        case WM_RBUTTONUP:
        case WM_KEYDOWN:
            if (msg == WM_KEYDOWN && wParam != VK_ESCAPE) break;
            HideOverlay();
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void OverlayWindow::ProcessSelection(int x, int y, int w, int h) {
    if (w <= 0 || h <= 0) return;

    HDC hScreenDC = GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hCropBmp = CreateCompatibleBitmap(hScreenDC, w, h);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hCropBmp);

    HDC hOrigDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldOrigBmp = (HBITMAP)SelectObject(hOrigDC, s_hbmScreen);

    // Copiar el recorte
    BitBlt(hMemDC, 0, 0, w, h, hOrigDC, x, y, SRCCOPY);

    SelectObject(hMemDC, hOldBmp);
    SelectObject(hOrigDC, hOldOrigBmp);
    DeleteDC(hOrigDC);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreenDC);

    // 1. Guardar a disco
    std::wstring savedPath = L"";
    Gdiplus::Bitmap* gdiBitmap = Gdiplus::Bitmap::FromHBITMAP(hCropBmp, NULL);
    if (gdiBitmap) {
        CLSID encoderClsid;
        std::wstring mimeType = (Config::Format == L"JPG") ? L"image/jpeg" : L"image/png";
        std::wstring ext = (Config::Format == L"JPG") ? L".jpg" : L".png";
        
        GetEncoderClsid(mimeType.c_str(), &encoderClsid);
        
        // Crear el directorio de destino si no existe
        CreateDirectoryW(Config::SavePath.c_str(), NULL);
        
        SYSTEMTIME st;
        GetLocalTime(&st);
        wchar_t filename[MAX_PATH];
        wsprintfW(filename, L"%s\\screenshot_%04d%02d%02d_%02d%02d%02d%s", 
            Config::SavePath.c_str(), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, ext.c_str());
            
        gdiBitmap->Save(filename, &encoderClsid, NULL);
        delete gdiBitmap;
        
        wchar_t fullPath[MAX_PATH];
        GetFullPathNameW(filename, MAX_PATH, fullPath, NULL);
        savedPath = fullPath;
    }

    // 2. Guardar al Portapapeles
    if (OpenClipboard(NULL)) {
        EmptyClipboard();
        SetClipboardData(CF_BITMAP, hCropBmp);
        CloseClipboard();
    } else {
        DeleteObject(hCropBmp);
    }
    
    // 3. Mostrar Toast Notification
    if (!savedPath.empty()) {
        // Necesitamos el hInstance para crear la ventana del toast.
        // Lo sacamos de la propia ventana del overlay (s_hwnd) antes de destruirla, o de GetModuleHandle.
        ToastWindow::Show(GetModuleHandle(NULL), savedPath);
    }
}
