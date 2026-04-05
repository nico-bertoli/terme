# ------------------------------ read params
param (
    [Alias("r")]
    [Switch]$Release,

    [Alias("d")]
    [Switch]$Debug
)

# ------------------------------ error handling
$ErrorActionPreference = "Stop"

# ------------------------------ build type 
$BuildType = "Debug"
if ($Release) { $BuildType = "Release" }

Write-Host "--- Starting Project Build Process ($BuildType) ---" -ForegroundColor Cyan

# ------------------------------ find nbkit in cache / download it if not foundd
Write-Host "[INFO] Checking for nbkit/1.0.0..." -ForegroundColor Gray

$nbkitExists = conan list nbkit/1.0.0 --format json | ConvertFrom-Json
$hasNbkit = $nbkitExists.LocalCache.Keys.Count -gt 0

if (-not $hasNbkit) {
    Write-Host "[INFO] nbkit not found. Cloning and creating..." -ForegroundColor Yellow
    $TmpDir = Join-Path ([System.IO.Path]::GetTempPath()) ([guid]::NewGuid().ToString())
    
    git clone --depth 1 https://github.com/nico-bertoli/nbkit.git $TmpDir
    conan create $TmpDir --build=missing -s compiler.cppstd=20 -s build_type=$BuildType
    
    Remove-Item -Recurse -Force $TmpDir
} else {
    Write-Host "[INFO] nbkit/1.0.0 found in cache." -ForegroundColor Green
}

# ------------------------------ install dependencies
Write-Host "[INFO] Installing dependencies..." -ForegroundColor Yellow
conan install . --output-folder=build --build=missing -s build_type=$BuildType -s compiler.cppstd=20

# ------------------------------ cmake setup
Write-Host "[INFO] Configuring CMake..." -ForegroundColor Yellow
cmake -S . -B build `
      -G "Visual Studio 17 2022" `
      -DCMAKE_TOOLCHAIN_FILE="build/generators/conan_toolchain.cmake" `
      -DCMAKE_BUILD_TYPE=$BuildType

# ------------------------------ build
Write-Host "[INFO] Building Project ($BuildType)..." -ForegroundColor Yellow
cmake --build build --config $BuildType

# ------------------------------ create conan package
Write-Host "[INFO] Creating local Conan package..." -ForegroundColor Cyan
conan create . --build=missing -s build_type=$BuildType -s compiler.cppstd=20

Write-Host "`n[SUCCESS] Build and Package Creation completed." -ForegroundColor Green