param()

$ErrorActionPreference = "Stop"

$installDir = Join-Path $env:LOCALAPPDATA "Programs\LuminaCapture"
$configDir = Join-Path $env:APPDATA "LuminaCapture"
$runKey = "HKCU:\Software\Microsoft\Windows\CurrentVersion\Run"

Get-Process -Name "LuminaCapture" -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue

if (Test-Path $runKey) {
    Remove-ItemProperty -Path $runKey -Name "LuminaCapture" -ErrorAction SilentlyContinue
}

if (Test-Path $installDir) {
    Remove-Item -Path $installDir -Recurse -Force
}

Write-Host "Lumina Capture fue desinstalada del directorio de programas." -ForegroundColor Green
Write-Host "Configuración conservada en: $configDir"
Write-Host "Si quieres eliminar también tu configuración, borra manualmente esa carpeta."
