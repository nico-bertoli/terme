#----------------------- read params
param (
    [Alias("r")]
    [Switch]$Release,

    [Alias("d")]
    [Switch]$Debug
)

#----------------------- error handling
$ErrorActionPreference = "Stop"

#----------------------- determine build type
$BuildType = "Debug"
if ($Release) { 
    $BuildType = "Release" 
}

Write-Host "--- Preparing to Run Project ($BuildType) ---" -ForegroundColor Cyan

#----------------------- check build directory
if (-not (Test-Path "build")) {
    Write-Error "[ERROR] Build directory not found. Please run xbuild.ps1 first."
    exit 1
}

#----------------------- find and run the example executable
$ExeName = "terme_example.exe"

# Fix: Join-Path for 3 segments (compatible with all PowerShell versions)
$ConfigPath = Join-Path "build" $BuildType
$TargetPath = Join-Path $ConfigPath $ExeName

Write-Host "[INFO] Searching for $BuildType executable at: $TargetPath" -ForegroundColor Gray

if (Test-Path $TargetPath) {
    Write-Host "[INFO] Running: $TargetPath" -ForegroundColor Green
    Write-Host "----------------------------------------------" -ForegroundColor Gray
    & $TargetPath
}
else {
    Write-Host "[ERROR] $ExeName not found for $BuildType configuration." -ForegroundColor Red
    Write-Host "[TIP] Expected path: $TargetPath" -ForegroundColor Yellow
    
    # Debugging: show what actually exists
    Write-Host "`n[DEBUG] Searching for any executables in 'build/'..." -ForegroundColor Gray
    Get-ChildItem "build" -Recurse -Filter "*.exe" | Select-Object FullName
    exit 1
}

Write-Host "----------------------------------------------" -ForegroundColor Gray
Write-Host "[SUCCESS] Execution finished." -ForegroundColor Green