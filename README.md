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
cmake --build build --config Release
```

## Instalación local

```powershell
powershell -ExecutionPolicy Bypass -File .\install.ps1
```

## Empaquetado de release

Después de compilar en `Release`, genera el ZIP distribuible con:

```powershell
powershell -ExecutionPolicy Bypass -File .\package-release.ps1
```

Esto crea:

- `dist\LuminaCapture-v0.1.0-win64.zip`

## Releases automáticas con GitHub Actions

El proyecto incluye dos workflows:

- `CI`: compila en `Release` en cada push a `main` y en cada Pull Request.
- `Release`: publica automáticamente una release cuando empujas un tag `v*`.

### Flujo recomendado

1. Haz commit de tus cambios.
2. Empuja `main`.
3. Crea y empuja un tag semántico:

```powershell
git tag v0.2.0
git push origin v0.2.0
```

GitHub Actions compilará `Release`, generará el ZIP y publicará la release automáticamente.

## Desinstalación local

```powershell
powershell -ExecutionPolicy Bypass -File .\uninstall.ps1
```

El instalador:

- copia el ejecutable a `%LOCALAPPDATA%\Programs\LuminaCapture`
- crea la carpeta por defecto `%USERPROFILE%\Pictures\screenshots`
- inicializa el archivo de configuración si no existe
- registra la app para iniciar con Windows

La desinstalación:

- elimina el inicio automático
- elimina el ejecutable instalado
- conserva la configuración en `%APPDATA%\LuminaCapture`

## Estado actual

La base del producto ya está lista para publicar pre-releases manuales en GitHub.
