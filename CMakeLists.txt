cmake_minimum_required(VERSION 3.15)
project(LuminaCapture VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Añadir el ejecutable. WIN32 evita que se abra una ventana de consola de fondo.
add_executable(LuminaCapture WIN32 src/main.cpp src/OverlayWindow.cpp src/ToastWindow.cpp src/Config.cpp src/ScreenshotTool.rc)
set_target_properties(LuminaCapture PROPERTIES OUTPUT_NAME "LuminaCapture")

# FORZAR UNICODE (Soluciona el error de LPCSTR vs wchar_t)
add_compile_definitions(UNICODE _UNICODE)

# Enlazar con las librerías del sistema de Windows necesarias
target_link_libraries(LuminaCapture PRIVATE user32 shell32 gdi32 gdiplus)
