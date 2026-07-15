$ErrorActionPreference = "Stop"

$ForbiddenPatterns = @(
    '^Binaries/',
    '^Intermediate/',
    '^Saved/',
    '^DerivedDataCache/',
    '\.generated\.h$',
    '\.gen\.cpp$'
)

$BinaryAssetPatterns = @(
    '\.uasset$',
    '\.umap$'
)

$ChangedFiles = git -c safe.directory=D:/Project/Rogue10m diff --cached --name-only
if (-not $ChangedFiles) {
    $ChangedFiles = git -c safe.directory=D:/Project/Rogue10m diff --name-only
}

$Blocked = @()
$BinaryAssets = @()

foreach ($File in $ChangedFiles) {
    $Normalized = $File -replace '\\', '/'

    foreach ($Pattern in $ForbiddenPatterns) {
        if ($Normalized -match $Pattern) {
            $Blocked += $File
            break
        }
    }

    foreach ($Pattern in $BinaryAssetPatterns) {
        if ($Normalized -match $Pattern) {
            $BinaryAssets += $File
            break
        }
    }
}

if ($Blocked.Count -gt 0) {
    Write-Error "Generated/cache files are changed and should not be committed:`n$($Blocked -join "`n")"
}

if ($BinaryAssets.Count -gt 0) {
    Write-Warning "Binary Unreal assets changed. Confirm they were edited in Unreal Editor:`n$($BinaryAssets -join "`n")"
}

Write-Host "Harness path check passed."
