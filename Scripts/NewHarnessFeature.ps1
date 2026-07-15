param(
    [Parameter(Mandatory = $true)]
    [string]$Name
)

$ErrorActionPreference = "Stop"

function Convert-ToSlug {
    param([string]$Value)

    $Slug = $Value.Trim().ToLowerInvariant()
    $Slug = $Slug -replace '[^\p{L}\p{Nd}]+', '-'
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

## Lazy Codex Ultrawork Packets

| Packet | Goal | Touched Area | Completion Condition | Validation |
| --- | --- | --- | --- | --- |
| 1 |  |  |  |  |

## Affected Files

## C++ / Blueprint Boundary

## Implementation Steps

## Build And Validation Plan

## ULW Loop Exit Gate

- [ ] Scope is clear.
- [ ] Packets are small enough to validate.
- [ ] Build/check command is selected.
- [ ] Review risks are listed.
- [ ] DevLog update is planned.

## Risks And Questions
"@ | Set-Content -Path $ArchitectPath -Encoding UTF8
}

if (-not (Test-Path $DocPath)) {
@"
# $Name - Development Summary

## Summary

## Completed Ultrawork Packets

| Packet | Result | Validation |
| --- | --- | --- |
| 1 |  |  |

## Changed Files

## Build Result

## Review Result

## Editor Or Blueprint Setup

## Playtest Notes

## ULW Loop Exit Gate

- [ ] Requested work is implemented or documented.
- [ ] Validation result is recorded.
- [ ] Review result is recorded.
- [ ] DevLog is updated in Korean.
- [ ] Remaining risks are listed.
"@ | Set-Content -Path $DocPath -Encoding UTF8
}

Write-Host "Created or found:"
Write-Host "  $ArchitectPath"
Write-Host "  $DocPath"
