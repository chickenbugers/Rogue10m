param(
	[string]$Configuration = "Development",
	[string]$ArchiveDirectory = "D:\Project\Rogue10m\Build\Android"
)

$ErrorActionPreference = "Stop"

$ProjectRoot = Split-Path -Parent $PSScriptRoot
$ProjectFile = Join-Path $ProjectRoot "Rogue10m.uproject"
$EngineRoot = "D:\Program Files\UE_5.7"
$RunUAT = Join-Path $EngineRoot "Engine\Build\BatchFiles\RunUAT.bat"

& $RunUAT BuildCookRun `
	-project="$ProjectFile" `
	-noP4 `
	-platform=Android `
	-clientconfig=$Configuration `
	-build `
	-cook `
	-stage `
	-pak `
	-package `
	-archive `
	-archivedirectory="$ArchiveDirectory"

if ($LASTEXITCODE -ne 0)
{
	throw "Android package failed with exit code $LASTEXITCODE"
}

