# Lumina Capture v0.1.0-pre

Primera pre-release pública de **Lumina Capture**, una utilidad de capturas para Windows escrita en **C++ Win32 + GDI+**, enfocada en alto rendimiento, bajo consumo y una experiencia visual cuidada.

## Incluye

- Captura por **recorte**, **ventana** y **pantalla completa**
- Soporte **multi-monitor**
- Copia automática al **portapapeles**
- Guardado en **PNG** o **JPG**
- **Toast** visual de confirmación
- **Atajo global configurable**
- Configuración persistente en `%APPDATA%\LuminaCapture`
- Instalación local con **autoarranque** mediante `install.ps1`
- Desinstalación limpia mediante `uninstall.ps1`

## Instalación

1. Descomprime el paquete.
2. Ejecuta PowerShell en la carpeta del paquete.
3. Lanza:

```powershell
powershell -ExecutionPolicy Bypass -File .\install.ps1
```

## Notas

- Esta es una **pre-release** (`v0.1.0-pre`).
- El objetivo de esta versión es validar instalación, flujo de captura y configuración.
- Próximos pasos recomendados: refinamiento visual del diálogo de configuración, pipeline de releases y empaquetado más formal.
