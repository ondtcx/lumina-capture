#include <windows.h>
#include <shellapi.h>
#include <gdiplus.h>
#include <shlobj.h>
#include <commctrl.h>
#include "OverlayWindow.h"
#include "Config.h"
#include "resource.h"

#pragma comment(lib, "comctl32.lib")

// Identificadores (IDs)
#define ID_TRAY_APP_ICON    1001
#define ID_TRAY_EXIT_MENU   3000
#define WM_TRAYICON         (WM_USER + 1)
#define HOTKEY_ID           1

// Variables globales
NOTIFYICONDATA nid = {};
HINSTANCE hInst;
HWND g_hwndMain = NULL;

// Declaración del procedimiento de ventana principal
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Procedimiento de la ventana de configuración
INT_PTR CALLBACK SettingsDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_INITDIALOG: {
            SetDlgItemTextW(hwndDlg, IDC_PATH_EDIT, Config::SavePath.c_str());
            SendDlgItemMessage(hwndDlg, IDC_FORMAT_COMBO, CB_ADDSTRING, 0, (LPARAM)L"PNG");
            SendDlgItemMessage(hwndDlg, IDC_FORMAT_COMBO, CB_ADDSTRING, 0, (LPARAM)L"JPG");
            int formatSel = (Config::Format == L"JPG") ? 1 : 0;
            SendDlgItemMessage(hwndDlg, IDC_FORMAT_COMBO, CB_SETCURSEL, formatSel, 0);

            // Cargar Hotkey actual en el control msctls_hotkey32
            BYTE hkMods = 0;
            if (Config::HotkeyModifier & MOD_ALT) hkMods |= HOTKEYF_ALT;
            if (Config::HotkeyModifier & MOD_CONTROL) hkMods |= HOTKEYF_CONTROL;
            if (Config::HotkeyModifier & MOD_SHIFT) hkMods |= HOTKEYF_SHIFT;
            WORD wHotkey = MAKEWORD(Config::HotkeyKey, hkMods);
            SendDlgItemMessage(hwndDlg, IDC_HOTKEY_COMBO, HKM_SETHOTKEY, wHotkey, 0);

            return TRUE;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == IDC_BROWSE_BTN) {
                BROWSEINFOW bi = { 0 };
                bi.lpszTitle = L"Seleccione la carpeta de guardado";
                bi.ulFlags = BIF_RETURNONLYFSDIRS;
                LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
                if (pidl != 0) {
                    wchar_t path[MAX_PATH];
                    if (SHGetPathFromIDListW(pidl, path)) {
                        SetDlgItemTextW(hwndDlg, IDC_PATH_EDIT, path);
                    }
                    CoTaskMemFree(pidl);
                }
            } else if (LOWORD(wParam) == IDC_SAVE_BTN || LOWORD(wParam) == IDOK) {
                wchar_t path[MAX_PATH];
                GetDlgItemTextW(hwndDlg, IDC_PATH_EDIT, path, MAX_PATH);
                Config::SavePath = path;

                int formatSel = SendDlgItemMessage(hwndDlg, IDC_FORMAT_COMBO, CB_GETCURSEL, 0, 0);
                Config::Format = (formatSel == 1) ? L"JPG" : L"PNG";

                LRESULT hkRes = SendDlgItemMessage(hwndDlg, IDC_HOTKEY_COMBO, HKM_GETHOTKEY, 0, 0);
                BYTE hkMods = HIBYTE(hkRes);
                Config::HotkeyKey = LOBYTE(hkRes);
                Config::HotkeyModifier = 0;
                if (hkMods & HOTKEYF_ALT) Config::HotkeyModifier |= MOD_ALT;
                if (hkMods & HOTKEYF_CONTROL) Config::HotkeyModifier |= MOD_CONTROL;
                if (hkMods & HOTKEYF_SHIFT) Config::HotkeyModifier |= MOD_SHIFT;

                Config::Save();
                
                // Re-registrar Hotkey
                if (g_hwndMain) {
                    UnregisterHotKey(g_hwndMain, HOTKEY_ID);
                    if (!RegisterHotKey(g_hwndMain, HOTKEY_ID, Config::HotkeyModifier | MOD_NOREPEAT, Config::HotkeyKey)) {
                        MessageBoxW(hwndDlg, L"No se pudo registrar el atajo. Puede que otra aplicación lo esté usando.", L"Error", MB_ICONERROR);
                    }
                }
                
                EndDialog(hwndDlg, IDOK);
                return TRUE;
            } else if (LOWORD(wParam) == IDCANCEL) {
                EndDialog(hwndDlg, IDCANCEL);
                return TRUE;
            }
            break;
        }
        case WM_CLOSE:
            EndDialog(hwndDlg, IDCANCEL);
            return TRUE;
    }
    return FALSE;
}

// Función para inicializar el ícono en la bandeja del sistema (System Tray)
void InitNotifyIconData(HWND hwnd) {
    memset(&nid, 0, sizeof(NOTIFYICONDATA));
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = ID_TRAY_APP_ICON;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    
    nid.hIcon = (HICON)LoadImageW(hInst, MAKEINTRESOURCEW(IDI_APP_ICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
    if (!nid.hIcon) {
        nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    }
    lstrcpy(nid.szTip, TEXT("Lumina Capture"));

    Shell_NotifyIcon(NIM_ADD, &nid);
}

// Punto de entrada de la aplicación Win32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
    // Evitar que Windows nos haga "zoom" si el monitor está a más del 100% de escala
    SetProcessDPIAware();

    hInst = hInstance;

    // Inicializar COM para el diálogo de carpetas
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    // Inicializar GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    Config::Load();

    const wchar_t CLASS_NAME[] = L"LuminaCaptureClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = (HICON)LoadImageW(hInstance, MAKEINTRESOURCEW(IDI_APP_ICON), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassW(&wc)) {
        return 0;
    }

    // Creamos una ventana *invisible* (Message-Only Window)
    // Su único propósito es recibir mensajes del sistema (hotkeys, clics del mouse, etc.)
    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"Lumina Capture Hidden", 
        WS_OVERLAPPEDWINDOW, 
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        return 0;
    }
    
    g_hwndMain = hwnd;

    // Agregamos el ícono a la bandeja del sistema (al lado del reloj)
    InitNotifyIconData(hwnd);

    // Registrar Atajo Global
    if (!RegisterHotKey(hwnd, HOTKEY_ID, Config::HotkeyModifier | MOD_NOREPEAT, Config::HotkeyKey)) {
        MessageBoxW(NULL, L"No se pudo registrar el atajo. Puede que otra aplicación lo esté usando.", L"Error", MB_ICONERROR);
    }

    // Bucle principal de mensajes (Message Loop)
    // Aquí es donde el programa se queda "escuchando"
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Limpiar GDI+ antes de salir
    Gdiplus::GdiplusShutdown(gdiplusToken);
    CoUninitialize();

    return 0;
}

// Procesador de eventos para la ventana
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_TRAYICON:
            // Lógica cuando el usuario interactúa con el ícono de la bandeja
            if (lParam == WM_RBUTTONUP || lParam == WM_LBUTTONUP) {
                // Mostrar un menú contextual básico al hacer clic derecho o izquierdo
                POINT pt;
                GetCursorPos(&pt);
                HMENU hMenu = CreatePopupMenu();
                InsertMenuW(hMenu, static_cast<UINT>(-1), MF_BYPOSITION | MF_STRING, ID_TRAY_SETTINGS_MENU, L"Configuración");
                InsertMenu(hMenu, -1, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
                InsertMenuW(hMenu, static_cast<UINT>(-1), MF_BYPOSITION | MF_STRING, ID_TRAY_EXIT_MENU, L"Salir");
                
                // SetForegroundWindow es crucial para que el menú desaparezca si el usuario hace clic fuera
                SetForegroundWindow(hwnd);
                TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
                DestroyMenu(hMenu);
            }
            break;

        case WM_COMMAND:
            // Lógica al hacer clic en las opciones del menú
            if (LOWORD(wParam) == ID_TRAY_SETTINGS_MENU) {
                DialogBoxW(hInst, MAKEINTRESOURCEW(IDD_SETTINGS_DIALOG), NULL, SettingsDialogProc);
            } else if (LOWORD(wParam) == ID_TRAY_EXIT_MENU) {
                UnregisterHotKey(hwnd, HOTKEY_ID);
                Shell_NotifyIcon(NIM_DELETE, &nid); // Limpiar ícono
                PostQuitMessage(0);                 // Cerrar el bucle de mensajes
            }
            break;

        case WM_HOTKEY:
            if (wParam == HOTKEY_ID) {
                // Llamamos a la función de captura
                OverlayWindow::ShowOverlay(hInst);
            }
            break;

        case WM_DESTROY:
            UnregisterHotKey(hwnd, HOTKEY_ID);
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
            return 0;
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
