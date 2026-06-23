param(
    [string]$EngineRoot = "D:\Program Files\UE_5.7",
    [string]$ProjectRoot = "D:\Project\Rogue10m",
    [string]$Target = "Rogue10mEditor",
    [string]$Platform = "Win64",
    [string]$Configuration = "Development",
    [switch]$NoHotReload
)

$ErrorActionPreference = "Stop"

$BuildBat = Join-Path $EngineRoot "Engine\Build\BatchFiles\Build.bat"
$ProjectFile = Join-Path $ProjectRoot "Rogue10m.uproject"

if (-not (Test-Path $BuildBat)) {
    throw "Build.bat not found: $BuildBat"
}

if (-not (Test-Path $ProjectFile)) {
    throw "Project file not found: $ProjectFile"
}

$Arguments = @(
    $Target,
    $Platform,
    $Configuration,
    "-Project=$ProjectFile",
    "-WaitMutex"
)

if ($NoHotReload) {
    $Arguments += "-NoHotReload"
}

& $BuildBat @Arguments
exit $LASTEXITCODE
