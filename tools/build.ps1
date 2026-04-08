param(
    [ValidateSet("Debug", "Release")]
    [string]$Preset = "Debug",

    [switch]$ConfigureOnly,
    [switch]$BuildOnly,
    [switch]$Clean,

    [string]$BuildDir
)

$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $PSScriptRoot
. (Join-Path $PSScriptRoot "cubeide-env.ps1")
$tools = Use-CubeIdeToolchain

Write-Host "Using STM32CubeIDE from: $($tools.CubeRoot)"

if (-not $BuildDir) {
    $BuildDir = Join-Path $repoRoot ("build\codex-" + $Preset)
}

Push-Location $repoRoot
try {
    $configureArgs = @(
        "-S", $repoRoot,
        "-B", $BuildDir,
        "-G", "Ninja",
        "-D", "CMAKE_TOOLCHAIN_FILE=$repoRoot\cmake\gcc-arm-none-eabi.cmake",
        "-D", "CMAKE_BUILD_TYPE=$Preset"
    )

    if (-not $BuildOnly) {
        & cmake @configureArgs
        if ($LASTEXITCODE -ne 0) {
            exit $LASTEXITCODE
        }
    }

    if (-not $ConfigureOnly) {
        if ($Clean) {
            & cmake --build $BuildDir --target clean
            if ($LASTEXITCODE -ne 0) {
                exit $LASTEXITCODE
            }
        }

        & cmake --build $BuildDir
        if ($LASTEXITCODE -ne 0) {
            exit $LASTEXITCODE
        }
    }
}
finally {
    Pop-Location
}
