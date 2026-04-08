param()

$ErrorActionPreference = "Stop"

function Get-LatestMatchingDirectory {
    param(
        [Parameter(Mandatory = $true)]
        [string]$BasePath,

        [Parameter(Mandatory = $true)]
        [string]$Filter
    )

    $matches = Get-ChildItem -Path $BasePath -Directory -Filter $Filter -ErrorAction Stop |
        Sort-Object Name -Descending

    if (-not $matches) {
        throw "No directory matched '$Filter' under '$BasePath'."
    }

    return $matches[0].FullName
}

function Get-CubeIdeToolPaths {
    $cubeRoot = Get-LatestMatchingDirectory -BasePath "C:\ST" -Filter "STM32CubeIDE_*"
    $pluginsPath = Join-Path $cubeRoot "STM32CubeIDE\plugins"

    $cmakePath = Join-Path (Get-LatestMatchingDirectory -BasePath $pluginsPath -Filter "com.st.stm32cube.ide.mcu.externaltools.cmake.win32_*") "tools\bin"
    $ninjaPath = Join-Path (Get-LatestMatchingDirectory -BasePath $pluginsPath -Filter "com.st.stm32cube.ide.mcu.externaltools.ninja.win32_*") "tools\bin"
    $gccPath = Join-Path (Get-LatestMatchingDirectory -BasePath $pluginsPath -Filter "com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.*") "tools\bin"

    return [PSCustomObject]@{
        CubeRoot = $cubeRoot
        PluginsPath = $pluginsPath
        CMake = $cmakePath
        Ninja = $ninjaPath
        Gcc = $gccPath
    }
}

$script:CubeIdeTools = Get-CubeIdeToolPaths

function Use-CubeIdeToolchain {
    $toolPaths = @(
        $script:CubeIdeTools.CMake
        $script:CubeIdeTools.Ninja
        $script:CubeIdeTools.Gcc
    )

    $env:PATH = (($toolPaths -join ";") + ";" + $env:PATH)
    return $script:CubeIdeTools
}
