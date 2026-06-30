param(
	[string]$Configuration = "Development",
	[string]$ArchiveDirectory = "D:\Project\Rogue10m\Build\Windows"
)

$ErrorActionPreference = "Stop"

$ProjectRoot = Split-Path -Parent $PSScriptRoot
$ProjectFile = Join-Path $ProjectRoot "Rogue10m.uproject"
$EngineRoot = "D:\Program Files\UE_5.7"
$RunUAT = Join-Path $EngineRoot "Engine\Build\BatchFiles\RunUAT.bat"

& $RunUAT BuildCookRun `
	-project="$ProjectFile" `
	-noP4 `
	-platform=Win64 `
	-clientconfig=$Configuration `
	-build `
	-cook `
	-stage `
	-pak `
	-archive `
	-archivedirectory="$ArchiveDirectory"

if ($LASTEXITCODE -ne 0)
{
	throw "Windows package failed with exit code $LASTEXITCODE"
}

