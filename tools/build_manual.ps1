<#
.SYNOPSIS
    Builds a Visual Studio C++ solution in Debug or Release (Win32)
    without launching the bloated Visual Studio IDE

.PARAMETER SolutionPath
    The full or relative path to the .sln file to build.

.PARAMETER DebugBuild
    Build only the Debug configuration.

.PARAMETER Release
    Build only the Release configuration.

    If neither -DebugBuild nor -Release is specified both are built.

.PARAMETER ErrorsOnly
    Print only errors; suppress all informational output.
    When a build fails, the actual MSBuild error lines are shown.

.EXAMPLE
    .\build_manual.ps1 -SolutionPath "C:\Projects\AfterRS\AfterRS.sln"
    # Builds Debug and Release

.EXAMPLE
    .\build_manual.ps1 -SolutionPath "C:\Projects\AfterRS\AfterRS.sln" -DebugBuild
    # Builds only Debug

.EXAMPLE
    .\build_manual.ps1 -SolutionPath "C:\Projects\AfterRS\AfterRS.sln" -Release -ErrorsOnly
    # Builds only Release, prints only errors
#>

param(
    [Parameter(Mandatory = $true, HelpMessage = "Path to the .sln file")]
    [string]$SolutionPath,

    [Parameter(HelpMessage = "Build only the Debug configuration")]
    [switch]$DebugBuild, # "Debug" is reserved...

    [Parameter(HelpMessage = "Build only the Release configuration")]
    [switch]$Release,

    [Parameter(HelpMessage = "Print only errors; suppress all informational output")]
    [switch]$ErrorsOnly
)

# write only when not in ErrorsOnly mode
function Write-Info {
    param([string]$Message, [string]$Color = "White")
    if (-not $ErrorsOnly) {
        Write-Host $Message -ForegroundColor $Color
    }
}

# Validate the solution file exists
if (-not (Test-Path -Path $SolutionPath -PathType Leaf)) {
    Write-Error "Solution file not found: $SolutionPath"
    exit 1
}

$solutionPath = (Resolve-Path -Path $SolutionPath).Path
Write-Info "Solution : $solutionPath" -Color Cyan

# Locate vswhere.exe
$vswherePath = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"

if (-not (Test-Path -Path $vswherePath)) {
    #  try 32-bit ProgramFiles on 32-bit OS
    $vswherePath = Join-Path $env:ProgramFiles "Microsoft Visual Studio\Installer\vswhere.exe"
}

if (-not (Test-Path -Path $vswherePath)) {
    Write-Error "vswhere.exe not found. Is Visual Studio 2017+ or Build Tools installed?"
    exit 1
}

Write-Info "vswhere : $vswherePath" -Color Gray

# Use vswhere to find the latest MSBuild.exe that supports C++
$msbuildPath = & $vswherePath -latest `
    -requires Microsoft.Component.MSBuild `
    -products * `
    -find "MSBuild\**\Bin\MSBuild.exe" |
    Select-Object -First 1

if (-not $msbuildPath) {
    Write-Error "MSBuild.exe not found via vswhere. Ensure the MSBuild workload is installed"
    exit 1
}

if (-not (Test-Path -Path $msbuildPath)) {
    Write-Error "MSBuild.exe path reported by vswhere does not exist: $msbuildPath"
    exit 1
}

Write-Info "MSBuild : $msbuildPath" -Color Green


$builds = @()

if ($DebugBuild) {
    $builds += @{ Configuration = "Debug";   Platform = "Win32" }
}
if ($Release) {
    $builds += @{ Configuration = "Release"; Platform = "Win32" }
}

# Default: build both when neither switch is specified
if ($builds.Count -eq 0) {
    $builds = @(
        @{ Configuration = "Debug";   Platform = "Win32" },
        @{ Configuration = "Release"; Platform = "Win32" }
    )
}

$verbosity = "minimal"

$allSucceeded = $true

foreach ($build in $builds) {
    $config = $build.Configuration
    $platform = $build.Platform

    Write-Info ""
    Write-Info "============================================" -Color Yellow
    Write-Info "  Building $config | $platform" -Color Yellow
    Write-Info "============================================" -Color Yellow

    # Build arguments for MSBuild
    $msbuildArgs = @(
        $solutionPath,
        "/p:Configuration=$config",
        "/p:Platform=$platform",
        "/t:Build",
        "/m", # parallel build (use all CPU cores)
        "/v:$verbosity",
        "/nologo" # suppress ugly startup banner
    )

    # Capture output so we can extract real error lines
    if ($ErrorsOnly) {
        $output = & $msbuildPath @msbuildArgs 2>&1
    } else {
        & $msbuildPath @msbuildArgs
    }

    $buildExitCode = $LASTEXITCODE

    if ($buildExitCode -ne 0) {
        if ($ErrorsOnly) {
            # Filter to lines that contain the word "error" (case-insensitive,
            # word-boundary match to avoid false positives like "errorsOnly").
            # This catches lines like:
            #   C:\src\file.cpp(42): error C2065: 'x': undeclared identifier
            #   error MSB4062: ...
            #   1 Error(s)
            $errorLines = $output | Where-Object {
                $_ -match '\berror\b'
            }

            if ($errorLines) {
                $errorLines | ForEach-Object {
                    Write-Host $_ -ForegroundColor Red
                }
            } else {
                # Dump everything so we're not left guessing
                Write-Host "[build_manual] No error-pattern lines found full output:" -ForegroundColor Yellow
                $output | ForEach-Object {
                    Write-Host $_ -ForegroundColor Yellow
                }
            }
        }

        Write-Error "  >> $config | $platform FAILED (exit code $buildExitCode)"
        $allSucceeded = $false
    } else {
        Write-Info "  >> $config | $platform SUCCEEDED" -Color Green
    }
}

Write-Info ""
Write-Info "============================================" -Color Cyan
if ($allSucceeded) {
    if ($ErrorsOnly) {
        Write-Host "Build succeeded." -ForegroundColor Green
    } else {
        Write-Info "  All builds completed successfully." -Color Green
    }
    exit 0
} else {
    Write-Error "  One or more builds FAILED."
    exit 1
}
