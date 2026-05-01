# Lumina Capture

Herramienta de capturas para Windows escrita en C++ con Win32 API y GDI+.

## Características

- Captura por recorte, ventana o pantalla completa
- Copia automática al portapapeles
- Guardado en PNG o JPG
- Toast visual minimalista
- Atajo global configurable
- Configuración persistente en `%APPDATA%\LuminaCapture\config.ini`

## Compilación

Compila con CMake desde VS Code o con MSVC:

```powershell
cmake -B build -S .
cmake --build build --config Debug
```

## Instalación local

```powershell
powershell -ExecutionPolicy Bypass -File .\install.ps1
```

El instalador:

- copia el ejecutable a `%LOCALAPPDATA%\Programs\LuminaCapture`
- crea la carpeta por defecto `%USERPROFILE%\Pictures\screenshots`
- inicializa el archivo de configuración si no existe
- registra la app para iniciar con Windows

## Estado actual

Pendientes recomendados:

- pipeline para publicar releases en GitHub
- script de desinstalación
