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