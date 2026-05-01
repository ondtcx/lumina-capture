#include "ToastWindow.h"
#include <gdiplus.h>
#include <windowsx.h>
#include <shellapi.h>

HWND ToastWindow::s_hwnd = NULL;
std::wstring ToastWindow::s_folderPath = L"";
bool ToastWindow::s_bHovering = false;

void ToastWindow::Show(HINSTANCE hInstance, const std::wstring& folderPath) {
    if (s_hwnd != NULL) {
        // Si ya hay un toast, reiniciar el timer y actualizar la ruta
        s_folderPath = folderPath;
        SetTimer(s_hwnd, TIMER_ID, 3000, NULL);
        InvalidateRect(s_hwnd, NULL, FALSE);
        return;
    }

    s_folderPath = folderPath;

    const wchar_t CLASS_NAME[] = L"ToastWindowClass";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassW(&wc);

    int width = 300;
    int height = 50;
    int marginX = 20;
    int marginY = 60; // Para que no tape la barra de tareas
    
    // Obtener tamaño del monitor principal (o donde esté la barra de tareas)
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

    int startX = workArea.right - width - marginX;
    int startY = workArea.bottom - height - marginY;

    s_hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, 
        CLASS_NAME, L"",
        WS_POPUP, 
        startX, startY, width, height,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(s_hwnd, SW_SHOWNA); // Mostrar sin robar el foco
    UpdateWindow(s_hwnd);

    // Timer de 3 segundos
    SetTimer(s_hwnd, TIMER_ID, 3000, NULL);
}

LRESULT CALLBACK ToastWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rect;
            GetClientRect(hwnd, &rect);
            int w = rect.right - rect.left;
            int h = rect.bottom - rect.top;

            // Doble buffer
            HDC hMemDC = CreateCompatibleDC(hdc);
            HBITMAP hMemBmp = CreateCompatibleBitmap(hdc, w, h);
            HBITMAP hOldMemBmp = (HBITMAP)SelectObject(hMemDC, hMemBmp);

            Gdiplus::Graphics graphics(hMemDC);
            graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

            // Fondo estilo Lumina Capture
            Gdiplus::SolidBrush bgBrush(Gdiplus::Color(245, 23, 31, 51));
            graphics.FillRectangle(&bgBrush, 0, 0, w, h);

            Gdiplus::SolidBrush accentBrush(Gdiplus::Color(255, 160, 120, 255));
            graphics.FillRectangle(&accentBrush, 0, 0, 4, h);

            // Borde sutil
            Gdiplus::Pen borderPen(Gdiplus::Color(110, 218, 226, 253), 1);
            graphics.DrawRectangle(&borderPen, 0, 0, w - 1, h - 1);

            // Texto: "Guardado y Copiado"
            Gdiplus::FontFamily fontFamily(L"Segoe UI");
            Gdiplus::Font font(&fontFamily, 14, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
            Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 218, 226, 253));
            
            Gdiplus::StringFormat format;
            format.SetAlignment(Gdiplus::StringAlignmentNear);
            format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            
            // Checkmark verde
            Gdiplus::Pen greenPen(Gdiplus::Color(255, 173, 198, 255), 2);
            graphics.DrawLine(&greenPen, 15, 25, 20, 30);
            graphics.DrawLine(&greenPen, 20, 30, 28, 18);

            graphics.DrawString(L"Captura guardada y copiada", -1, &font, 
                                Gdiplus::RectF(38, 0, w - 80, h), &format, &textBrush);

            // "Botón" de carpeta (un rectángulo con un icono de carpeta)
            // Área de la carpeta: w-40, 10, 30, 30
            Gdiplus::SolidBrush folderBgBrush(Gdiplus::Color(255, 34, 42, 61));
            graphics.FillRectangle(&folderBgBrush, w - 40, 10, 30, 30);
            
            Gdiplus::SolidBrush folderIconBrush(Gdiplus::Color(255, 173, 198, 255));
            graphics.FillRectangle(&folderIconBrush, w - 32, 17, 14, 11); // base
            graphics.FillRectangle(&folderIconBrush, w - 32, 15, 6, 3); // tab

            // Copiar buffer
            BitBlt(hdc, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);

            SelectObject(hMemDC, hOldMemBmp);
            DeleteObject(hMemBmp);
            DeleteDC(hMemDC);

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_TIMER: {
            if (wParam == TIMER_ID) {
                KillTimer(hwnd, TIMER_ID);
                DestroyWindow(hwnd);
                s_hwnd = NULL;
            }
            return 0;
        }
        case WM_MOUSEMOVE: {
            if (!s_bHovering) {
                s_bHovering = true;
                KillTimer(hwnd, TIMER_ID);
                TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0 };
                TrackMouseEvent(&tme);
            }
            return 0;
        }
        case WM_MOUSELEAVE: {
            s_bHovering = false;
            SetTimer(hwnd, TIMER_ID, 3000, NULL);
            return 0;
        }
        case WM_SETCURSOR: {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);
            RECT rect;
            GetClientRect(hwnd, &rect);
            int w = rect.right - rect.left;
            
            // Si el cursor está sobre la "carpeta"
            if (pt.x >= w - 40 && pt.x <= w - 10 && pt.y >= 10 && pt.y <= 40) {
                SetCursor(LoadCursor(NULL, IDC_HAND));
                return TRUE;
            }
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return TRUE;
        }
        case WM_LBUTTONUP: {
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            
            RECT rect;
            GetClientRect(hwnd, &rect);
            int w = rect.right - rect.left;

            // Si hizo clic en la carpeta
            if (pt.x >= w - 40 && pt.x <= w - 10 && pt.y >= 10 && pt.y <= 40) {
                // Abrir la carpeta y seleccionar el archivo
                std::wstring param = L"/select,\"" + s_folderPath + L"\"";
                ShellExecuteW(NULL, L"open", L"explorer.exe", param.c_str(), NULL, SW_SHOWNORMAL);
                
                KillTimer(hwnd, TIMER_ID);
                DestroyWindow(hwnd);
                s_hwnd = NULL;
            }
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
