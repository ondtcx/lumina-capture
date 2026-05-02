param()

$ErrorActionPreference = "Stop"
[Console]::OutputEncoding = [System.Text.UTF8Encoding]::new()

$owner = "ondtcx"
$repo = "lumina-capture"
$apiBase = "https://api.github.com/repos/$owner/$repo"
$headers = @{ "User-Agent" = "LuminaCaptureInstaller" }

Write-Host "Buscando la última release disponible de Lumina Capture..." -ForegroundColor Cyan

$release = $null

try {
    $release = Invoke-RestMethod -Uri "$apiBase/releases/latest" -Headers $headers
} catch {
    Write-Host "No se encontró una release estable. Buscando pre-releases..." -ForegroundColor Yellow
}

if (-not $release) {
    $releases = Invoke-RestMethod -Uri "$apiBase/releases" -Headers $headers
    $release = $releases | Where-Object { -not $_.draft } | Select-Object -First 1
}

if (-not $release) {
    throw "No se encontró ninguna release instalable de Lumina Capture."
}

$asset = $release.assets | Where-Object { $_.name -like "LuminaCapture-*-win64.zip" } | Select-Object -First 1

if (-not $asset) {
    throw "La release $($release.tag_name) no contiene un ZIP instalable compatible."
}

Write-Host "Release encontrada: $($release.tag_name)" -ForegroundColor Green
Write-Host "Archivo: $($asset.name)" -ForegroundColor Green

$confirmation = Read-Host "Se descargará e instalará Lumina Capture. ¿Deseas continuar? (S/N)"
if ($confirmation -notin @('S', 's', 'Y', 'y')) {
    Write-Host "Instalación cancelada por el usuario." -ForegroundColor Yellow
    exit 0
}

$tempRoot = Join-Path $env:TEMP "LuminaCaptureInstall"
$zipPath = Join-Path $tempRoot $asset.name
$extractPath = Join-Path $tempRoot "extracted"

if (Test-Path $tempRoot) {
    Remove-Item $tempRoot -Recurse -Force
}

New-Item -ItemType Directory -Force -Path $extractPath | Out-Null

Write-Host "Descargando paquete..." -ForegroundColor Cyan
Invoke-WebRequest -Uri $asset.browser_download_url -Headers $headers -OutFile $zipPath

Write-Host "Extrayendo archivos..." -ForegroundColor Cyan
Expand-Archive -Path $zipPath -DestinationPath $extractPath -Force

$installScript = Get-ChildItem -Path $extractPath -Filter "install.ps1" -Recurse | Select-Object -First 1

if (-not $installScript) {
    throw "No se encontró install.ps1 dentro del paquete descargado."
}

Write-Host "Lanzando instalador local..." -ForegroundColor Cyan
& $installScript.FullName

Write-Host "Limpiando temporales..." -ForegroundColor Cyan
Remove-Item $tempRoot -Recurse -Force -ErrorAction SilentlyContinue

Write-Host "Lumina Capture instalada correctamente desde la release $($release.tag_name)." -ForegroundColor Green
