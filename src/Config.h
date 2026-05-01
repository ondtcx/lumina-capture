#pragma once
#include <windows.h>
#include <string>

class Config {
public:
    static std::wstring SavePath;
    static std::wstring Format; // L"PNG" o L"JPG"
    static UINT HotkeyModifier;
    static UINT HotkeyKey;

    static void Load();
    static void Save();
};