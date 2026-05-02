param(
    [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"

$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoBuildExe = Join-Path $projectRoot "build\$Configuration\LuminaCapture.exe"
$packageExe = Join-Path $projectRoot "LuminaCapture.exe"

if (Test-Path $packageExe) {
    $exePath = $packageExe
} elseif (Test-Path $repoBuildExe) {
    $exePath = $repoBuildExe
} else {
    throw "No se encontró LuminaCapture.exe ni en el paquete actual ni en build\$Configuration."
}

$installDir = Join-Path $env:LOCALAPPDATA "Programs\LuminaCapture"
$configDir = Join-Path $env:APPDATA "LuminaCapture"
$picturesDir = [Environment]::GetFolderPath('MyPictures')
$defaultSaveDir = Join-Path $picturesDir "screenshots"
$runKey = "HKCU:\Software\Microsoft\Windows\CurrentVersion\Run"

New-Item -ItemType Directory -Force -Path $installDir | Out-Null
New-Item -ItemType Directory -Force -Path $configDir | Out-Null
New-Item -ItemType Directory -Force -Path $defaultSaveDir | Out-Null

Write-Host "Se instalará Lumina Capture en: $installDir" -ForegroundColor Cyan
Write-Host "Ruta por defecto de capturas: $defaultSaveDir" -ForegroundColor Cyan
$confirmation = Read-Host "¿Deseas continuar? (S/N)"
if ($confirmation -notin @('S', 's', 'Y', 'y')) {
    Write-Host "Instalación cancelada por el usuario." -ForegroundColor Yellow
    exit 0
}

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
