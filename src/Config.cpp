#include "Config.h"
#include <shlobj.h>

std::wstring Config::SavePath = L"";
std::wstring Config::Format = L"PNG";
UINT Config::HotkeyModifier = MOD_ALT | MOD_SHIFT;
UINT Config::HotkeyKey = 0x53; // 'S'

static std::wstring GetDefaultSavePath() {
    wchar_t picturesPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_MYPICTURES, NULL, 0, picturesPath))) {
        std::wstring screenshotsDir = std::wstring(picturesPath) + L"\\screenshots";
        CreateDirectoryW(screenshotsDir.c_str(), NULL);
        return screenshotsDir;
    }

    CreateDirectoryW(L"images", NULL);
    wchar_t fullPath[MAX_PATH];
    GetFullPathNameW(L"images", MAX_PATH, fullPath, NULL);
    return fullPath;
}

std::wstring GetConfigPath() {
    wchar_t path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path))) {
        std::wstring dir = std::wstring(path) + L"\\LuminaCapture";
        CreateDirectoryW(dir.c_str(), NULL);
        return dir + L"\\config.ini";
    }
    return L".\\config.ini";
}

void Config::Load() {
    std::wstring iniPath = GetConfigPath();
    wchar_t buffer[MAX_PATH];
    
    // Path
    GetPrivateProfileStringW(L"Settings", L"SavePath", L"", buffer, MAX_PATH, iniPath.c_str());
    if (wcslen(buffer) > 0) {
        SavePath = buffer;
    } else {
        SavePath = GetDefaultSavePath();
    }
    
    // Format
    GetPrivateProfileStringW(L"Settings", L"Format", L"PNG", buffer, MAX_PATH, iniPath.c_str());
    Format = buffer;
    
    HotkeyModifier = GetPrivateProfileIntW(L"Settings", L"HotkeyModifier", MOD_ALT | MOD_SHIFT, iniPath.c_str());
    HotkeyKey = GetPrivateProfileIntW(L"Settings", L"HotkeyKey", 0x53, iniPath.c_str());
}

void Config::Save() {
    std::wstring iniPath = GetConfigPath();
    WritePrivateProfileStringW(L"Settings", L"SavePath", SavePath.c_str(), iniPath.c_str());
    WritePrivateProfileStringW(L"Settings", L"Format", Format.c_str(), iniPath.c_str());

    WritePrivateProfileStringW(L"Settings", L"HotkeyModifier", std::to_wstring(HotkeyModifier).c_str(), iniPath.c_str());
    WritePrivateProfileStringW(L"Settings", L"HotkeyKey", std::to_wstring(HotkeyKey).c_str(), iniPath.c_str());
}
