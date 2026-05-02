# Lumina Capture

Lumina Capture es una herramienta de capturas para Windows escrita en **C++ Win32 + GDI+**, diseñada para ofrecer **alto rendimiento**, **bajo consumo** y una experiencia de uso pulida.

## Qué hace

- Captura por **recorte**, **ventana** y **pantalla completa**
- Soporte **multi-monitor**
- Copia automática al **portapapeles**
- Guardado en **PNG** o **JPG**
- **Toast** visual de confirmación
- **Atajo global configurable**
- Inicio automático con Windows
- Configuración persistente en `%APPDATA%\LuminaCapture\config.ini`

---

## Instalación para usuario final

### Opción recomendada: instalación remota con un solo comando

```powershell
iwr https://raw.githubusercontent.com/ondtcx/lumina-capture/main/install-remote.ps1 -OutFile install-lumina.ps1; powershell -ExecutionPolicy Bypass -File .\install-lumina.ps1
```

Este flujo:

- busca la última **release estable**
- usa **pre-release** como fallback si todavía no existe una estable
- descarga el ZIP oficial desde GitHub
- pide confirmación antes de instalar
- instala la app y limpia temporales

### Opción manual desde release descargada

1. Descarga el ZIP desde la sección de releases.
2. Descomprímelo.
3. Abre PowerShell en la carpeta.
4. Ejecuta:

```powershell
powershell -ExecutionPolicy Bypass -File .\install.ps1
```

El instalador:

- copia `LuminaCapture.exe` a `%LOCALAPPDATA%\Programs\LuminaCapture`
- crea `%USERPROFILE%\Pictures\screenshots` si no existe
- inicializa la configuración si no existe
- registra la app para iniciar con Windows

---

## Uso

- Ejecuta Lumina Capture desde el acceso instalado o al iniciar sesión en Windows.
- Usa el atajo global configurado para abrir la interfaz de captura.
- Por defecto, las imágenes se guardan en:

```text
%USERPROFILE%\Pictures\screenshots
```

- Puedes cambiar:
  - carpeta de guardado
  - formato de imagen
  - hotkey global

desde **Configuración** en el icono de la bandeja del sistema.

---

## Desinstalación

```powershell
powershell -ExecutionPolicy Bypass -File .\uninstall.ps1
```

La desinstalación:

- elimina el inicio automático
- elimina el ejecutable instalado
- conserva la configuración en `%APPDATA%\LuminaCapture`

Si quieres un reset total, borra también manualmente:

```text
%APPDATA%\LuminaCapture
```

---

## Configuración

La configuración se guarda en:

```text
%APPDATA%\LuminaCapture\config.ini
```

Actualmente almacena:

- ruta de guardado
- formato de imagen
- hotkey global personalizada

---

## Desarrollo local

### Compilación

```powershell
cmake -B build -S .
cmake --build build --config Release
```

### Nota sobre Windows y CMake

Si `cmake` no está disponible en tu PowerShell normal, usa la ruta completa del binario instalado por Visual Studio Build Tools:

```powershell
& "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build --config Release
```

También puedes compilar desde:

- **VS Code + CMake Tools**
- **Developer PowerShell for Visual Studio**

### Instalación local desde el repo

Después de compilar:

```powershell
powershell -ExecutionPolicy Bypass -File .\install.ps1
```

---

## Empaquetado de release

Después de compilar en `Release`, genera el ZIP distribuible con:

```powershell
powershell -ExecutionPolicy Bypass -File .\package-release.ps1
```

Esto crea un archivo como:

```text
dist\LuminaCapture-v0.1.0-win64.zip
```

---

## GitHub Actions

El proyecto incluye dos workflows:

- **CI**: compila en `Release` en cada push a `main` y en cada Pull Request
- **Release**: publica automáticamente una release cuando empujas un tag `v*`

### Flujo recomendado para nuevas versiones

```powershell
git tag v0.2.0
git push origin v0.2.0
```

GitHub Actions compilará `Release`, generará el ZIP y publicará la release automáticamente.

---

## Estado del proyecto

Lumina Capture ya está lista para:

- instalación local
- instalación remota con un solo comando
- pre-releases manuales o automáticas
- distribución por GitHub Releases

La siguiente iteración natural del producto es seguir refinando la experiencia visual del diálogo de configuración y ampliar el roadmap funcional.
