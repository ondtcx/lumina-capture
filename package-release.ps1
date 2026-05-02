param(
    [string]$Version = "v0.1.0",
    [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"

$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$exePath = Join-Path $projectRoot "build\$Configuration\LuminaCapture.exe"
$releaseNotes = Join-Path $projectRoot "RELEASE_NOTES_$Version.md"
$distDir = Join-Path $projectRoot "dist"
$packageDir = Join-Path $distDir "LuminaCapture-$Version"
$zipPath = Join-Path $distDir "LuminaCapture-$Version-win64.zip"

if (-not (Test-Path $exePath)) {
    throw "No se encontró el ejecutable en $exePath. Compila primero con configuración $Configuration."
}

if (-not (Test-Path $releaseNotes)) {
    throw "No se encontró el archivo de release notes: $releaseNotes"
}

if (Test-Path $packageDir) {
    Remove-Item $packageDir -Recurse -Force
}

if (Test-Path $zipPath) {
    Remove-Item $zipPath -Force
}

New-Item -ItemType Directory -Force -Path $packageDir | Out-Null

Copy-Item $exePath (Join-Path $packageDir "LuminaCapture.exe") -Force
Copy-Item (Join-Path $projectRoot "install.ps1") (Join-Path $packageDir "install.ps1") -Force
Copy-Item (Join-Path $projectRoot "uninstall.ps1") (Join-Path $packageDir "uninstall.ps1") -Force
Copy-Item $releaseNotes (Join-Path $packageDir "RELEASE_NOTES.md") -Force
Copy-Item (Join-Path $projectRoot "README.md") (Join-Path $packageDir "README.md") -Force

New-Item -ItemType Directory -Force -Path $distDir | Out-Null
Compress-Archive -Path "$packageDir\*" -DestinationPath $zipPath -Force

Write-Host "Paquete generado correctamente:" -ForegroundColor Green
Write-Host $zipPath
