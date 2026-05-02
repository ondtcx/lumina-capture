param(
    [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"

$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$exePath = Join-Path $projectRoot "build\$Configuration\LuminaCapture.exe"

if (-not (Test-Path $exePath)) {
    throw "No se encontró el ejecutable en $exePath. Compila primero el proyecto."
}

$installDir = Join-Path $env:LOCALAPPDATA "Programs\LuminaCapture"
$configDir = Join-Path $env:APPDATA "LuminaCapture"
$picturesDir = [Environment]::GetFolderPath('MyPictures')
$defaultSaveDir = Join-Path $picturesDir "screenshots"
$runKey = "HKCU:\Software\Microsoft\Windows\CurrentVersion\Run"

New-Item -ItemType Directory -Force -Path $installDir | Out-Null
New-Item -ItemType Directory -Force -Path $configDir | Out-Null
New-Item -ItemType Directory -Force -Path $defaultSaveDir | Out-Null

Copy-Item $exePath (Join-Path $installDir "LuminaCapture.exe") -Force

$configPath = Join-Path $configDir "config.ini"
if (-not (Test-Path $configPath)) {
    @"
[Settings]
SavePath=$defaultSaveDir
Format=PNG
HotkeyModifier=5
HotkeyKey=83
"@ | Set-Content -Path $configPath -Encoding Unicode
}

Set-ItemProperty -Path $runKey -Name "LuminaCapture" -Value ('"' + (Join-Path $installDir "LuminaCapture.exe") + '"')

Write-Host "Instalación completada." -ForegroundColor Green
Write-Host "Ejecutable: $installDir\LuminaCapture.exe"
Write-Host "Capturas por defecto: $defaultSaveDir"
Write-Host "Inicio automático habilitado."
