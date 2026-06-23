param(
    [Parameter(Mandatory = $true)]
    [string]$Name
)

$ErrorActionPreference = "Stop"

function Convert-ToSlug {
    param([string]$Value)

    $Slug = $Value.Trim().ToLowerInvariant()
    $Slug = $Slug -replace '[^a-z0-9가-힣]+', '-'
    $Slug = $Slug.Trim('-')

    if ([string]::IsNullOrWhiteSpace($Slug)) {
        return "feature"
    }

    return $Slug
}

$Date = Get-Date -Format "yyyy-MM-dd"
$Slug = Convert-ToSlug $Name

$ArchitectDir = Join-Path "Feature" "architect"
$DocDir = Join-Path "Feature" "doc"

New-Item -ItemType Directory -Force -Path $ArchitectDir | Out-Null
New-Item -ItemType Directory -Force -Path $DocDir | Out-Null

$ArchitectPath = Join-Path $ArchitectDir "$Date`_$Slug.md"
$DocPath = Join-Path $DocDir "$Date`_$Slug.md"

if (-not (Test-Path $ArchitectPath)) {
@"
# $Name - Architecture Plan

## Feature Summary

## Existing Context

## Technical Approach

## Affected Files

## C++ / Blueprint Boundary

## Implementation Steps

## Build And Validation Plan

## Risks And Questions
"@ | Set-Content -Path $ArchitectPath -Encoding UTF8
}

if (-not (Test-Path $DocPath)) {
@"
# $Name - Development Summary

## Summary

## Changed Files

## Build Result

## Review Result

## Editor Or Blueprint Setup

## Playtest Notes
"@ | Set-Content -Path $DocPath -Encoding UTF8
}

Write-Host "Created or found:"
Write-Host "  $ArchitectPath"
Write-Host "  $DocPath"
