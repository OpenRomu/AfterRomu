<#
.SYNOPSIS
    Launches AfterRS.exe under the CDB command-line debugger with PDB symbols
    without VS.

.DESCRIPTION
    Detects whether the target is 32-bit or 64-bit (PE header), selects the
    matching cdb.exe (x86 or x64) and launches the game in an debug session
    with proper symbol and source paths.

    Initial commands are written to a temp script file (-cf) instead of
    a semicolon-separated -c string.  This avoids a weird parsing bug:
    .sympath treats semicolons as symbol-path delimiters and would consume
    subsequent commands as part of the path, corrupting the symbol search
    and makes cdb even more useless.

.PARAMETER ExePath
    Path to AfterRS.exe. Defaults to ".\Debug\AfterRS.exe".

.PARAMETER SourcePath
    Root directory of the source code for source-level stepping.
    Defaults to the current directory.

.PARAMETER Arguments
    Any arguments to pass to AfterRS.exe.

.PARAMETER BreakOnStart
    Stop at the loader breakpoint so you can set breakpoints
    before the game starts running.  When omitted the game
    auto-continues to the main window.

.EXAMPLE
    # Launches the game, CDB breaks only on crashes
    .\debug_manual.ps1

.EXAMPLE
    # Stops before the game runs and set breakpoints then type g
    .\debug_manual.ps1 -BreakOnStart

.EXAMPLE
    # If needed
    .\debug_manual.ps1 -SourcePath "C:\Projects\AfterRS\src"
#>

param(
    [Parameter(HelpMessage = "Path to AfterRS.exe")]
    [string]$ExePath = ".\Debug\AfterRS.exe",

    [Parameter(HelpMessage = "Root source directory for source-level debugging")]
    [string]$SourcePath = ".",

    [Parameter(HelpMessage = "Arguments to pass to AfterRS.exe")]
    [string]$Arguments = "",

    [Parameter(HelpMessage = "Stop at loader entry to set breakpoints before running")]
    [switch]$BreakOnStart
)

function Write-Info  { param([string]$Message, [string]$Color = "White");  Write-Host $Message -ForegroundColor $Color }
function Write-Fail  { param([string]$Message);                            Write-Host $Message -ForegroundColor Red    }

if (-not (Test-Path -Path $ExePath -PathType Leaf)) {
    Write-Fail "Executable not found: $ExePath"
    exit 1
}

$exeFullPath  = (Resolve-Path -Path $ExePath).Path
$exeDirectory = Split-Path -Parent $exeFullPath
$exeName      = Split-Path -Leaf $exeFullPath

$bytes = [System.IO.File]::ReadAllBytes($exeFullPath)

if ($bytes.Length -lt 64) {
    Write-Fail "File too small to be a valid PE: $exeFullPath"
    exit 1
}

# MZ signature
if ($bytes[0] -ne 0x4D -or $bytes[1] -ne 0x5A) {
    Write-Fail "Not a valid PE (no MZ signature): $exeFullPath"
    exit 1
}

$peOffset = [BitConverter]::ToInt32($bytes, 0x3C)

if ($peOffset -lt 0 -or ($peOffset + 0x1A) -gt $bytes.Length) {
    Write-Fail "Invalid PE header offset in: $exeFullPath"
    exit 1
}

# PE\0\0 signature
if ($bytes[$peOffset] -ne 0x50 -or $bytes[$peOffset + 1] -ne 0x45) {
    Write-Fail "Not a valid PE (no PE signature): $exeFullPath"
    exit 1
}

$magic = [BitConverter]::ToUInt16($bytes, $peOffset + 0x18)

$is32Bit = $false
if ($magic -eq 0x10B) {
    $is32Bit = $true
    Write-Info "Arch    : 32-bit (PE32 / Win32)" -Color Cyan
} elseif ($magic -eq 0x20B) {
    $is32Bit = $false
    Write-Info "Arch    : 64-bit (PE32+ / x64)" -Color Cyan
} else {
    Write-Fail "Unknown PE magic 0x$($magic.ToString('X4')) in: $exeFullPath"
    exit 1
}

$pdbCandidate = Join-Path $exeDirectory "AfterRS.pdb"

if (Test-Path -Path $pdbCandidate -PathType Leaf) {
    Write-Info "PDB     : $pdbCandidate" -Color Green
} else {
    Write-Info "PDB     : not found at $pdbCandidate" -Color Yellow
    Write-Info "          Build the Debug configuration first." -Color Yellow
}

$dataFolder = Join-Path $exeDirectory "data"
if (Test-Path -Path $dataFolder -PathType Container) {
    Write-Info "data/   : $dataFolder" -Color Green
} else {
    Write-Info "data/   : not found at $dataFolder (game may fail to load assets)" -Color Yellow
}

$srcFullPath = (Resolve-Path -Path $SourcePath -ErrorAction SilentlyContinue)
if ($srcFullPath) {
    $srcFullPath = $srcFullPath.Path
} else {
    $srcFullPath = $SourcePath
    Write-Info "Source  : '$SourcePath' could not be resolved" -Color Yellow
}

Write-Info ""
Write-Info "Locating cdb.exe ..." -Color Cyan

$kitsRoot    = "${env:ProgramFiles(x86)}\Windows Kits"
$targetArch  = if ($is32Bit) { "x86" } else { "x64" }
$fallbackArch = if ($is32Bit) { "x64" } else { "x86" }

$cdbPath = $null

# Search Windows Kits for the matching architecture to avoid the wrong cdb used
# MS never heard of gdb-multiarch...
if (Test-Path $kitsRoot) {
    $cdbPath = Get-ChildItem -Path $kitsRoot -Recurse -Filter "cdb.exe" `
                 -ErrorAction SilentlyContinue |
               Where-Object { $_.FullName -match "\\$targetArch\\" } |
               Sort-Object FullName -Descending |
               Select-Object -First 1 -ExpandProperty FullName
}

# Known WDK path
if (-not $cdbPath) {
    $wdkPath = Join-Path ${env:ProgramFiles(x86)} "Windows Kits\10\Debuggers\$targetArch\cdb.exe"
    if (Test-Path $wdkPath) { $cdbPath = $wdkPath }
}

# Fallback to the other architecture if it breaks
if (-not $cdbPath -and (Test-Path $kitsRoot)) {
    $cdbPath = Get-ChildItem -Path $kitsRoot -Recurse -Filter "cdb.exe" `
                 -ErrorAction SilentlyContinue |
               Where-Object { $_.FullName -match "\\$fallbackArch\\" } |
               Sort-Object FullName -Descending |
               Select-Object -First 1 -ExpandProperty FullName

    if ($cdbPath) {
        Write-Info ""
        Write-Info "WARNING: Only $fallbackArch cdb.exe found, target is $targetArch." -Color Yellow
        Write-Info "         Debugging will work but you will see WOW64 breakpoints." -Color Yellow
        Write-Info "         Install $targetArch Debugging Tools for the best experience." -Color Yellow
    }
}

if (-not $cdbPath) {
    $cdbOnPath = Get-Command "cdb.exe" -ErrorAction SilentlyContinue
    if ($cdbOnPath) { $cdbPath = $cdbOnPath.Source }
}

if (-not $cdbPath -or -not (Test-Path $cdbPath)) {
    Write-Fail ""
    Write-Fail "cdb.exe not found!"
    Write-Fail "Install the Debugging Tools for Windows (Not installed by default)!"
    Write-Fail ""
    Write-Fail "Expected: C:\Program Files (x86)\Windows Kits\10\Debuggers\$targetArch\cdb.exe"
    exit 1
}

Write-Info "CDB     : $cdbPath" -Color Green

# Symbol path
$symPath = @(
    $exeDirectory,
    "srv*${env:LOCALAPPDATA}\SymbolCache*https://msdl.microsoft.com/download/symbols"
) -join ";"

# Use -cf (command file) instead of -c (inline commands)
# because .sympath treats semicolons as symbol-path delimiters (?!) and would
# consume everything after it as part of the path.  One command per line
# in a file avoids this parser ambiguity entirely
#
# NEVER CALL .sympath at all — the -y flag on the command line
# should already sets it.  Calling .sympath inside -c would corrupt it.

$scriptFile = [System.IO.Path]::GetTempFileName()

$scriptLines = @(
    ".lines",     # enable source-line mapping (off by default)
    ".reload"     # force symbol load for all modules now
)

if (-not $BreakOnStart) {
    $scriptLines += "g"   # auto-continue past the initial loader breakpoint
}

$scriptLines | Set-Content -Path $scriptFile -Encoding ASCII

Write-Info "InitCmd : $scriptFile" -Color Gray

Write-Info ""
Write-Info "============================================" -Color Yellow
Write-Info "  Launching $exeName under CDB ($targetArch)" -Color Yellow
Write-Info "============================================" -Color Yellow

if ($BreakOnStart) {
    Write-Info ""
    Write-Info "Stopped at the loader breakpoint." -Color Cyan
    Write-Info "Set breakpoints, then type g to run the game." -Color Cyan
    Write-Info "  e.g.  bu AfterRS!main  ;  g" -Color Gray
} else {
    Write-Info ""
    Write-Info "The game will auto-start.  CDB breaks on crashes." -Color Cyan
}

# IT'S NOT GDB, HOPE IT'S HELPFUL
Write-Info ""
Write-Info "Quick reference:" -Color Cyan
Write-Info "  g          = Go (continue)" -Color Gray
Write-Info "  kn         = Call stack with source" -Color Gray
Write-Info "  .frame N   = Switch to frame N" -Color Gray
Write-Info "  dv         = Local variables" -Color Gray
Write-Info "  lm         = List modules (symbol status)" -Color Gray
Write-Info "  bu <sym>   = Deferred breakpoint (resolves on load)" -Color Gray
Write-Info "  bp <addr>  = Set breakpoint at address" -Color Gray
Write-Info "  bl         = List breakpoints" -Color Gray
Write-Info "  bc *       = Clear all breakpoints" -Color Gray
Write-Info "  r          = Registers" -Color Gray
Write-Info "  q          = Quit" -Color Gray
Write-Info " .hh command = Help for a command" -Color Gray
Write-Info ""

#   -y <symPath>        symbol search path
#   -srcpath <srcPath>  source file search path
#   -cf <file>          initial commands from file (one per line)
$cdbArgs = @(
    "-y",       $symPath,
    "-srcpath", $srcFullPath,
    "-cf",      $scriptFile
)

$cdbArgs += $exeFullPath

if ($Arguments) {
    $cdbArgs += $Arguments -split ' '
}

# Run from the exe's directory so the game finds data/ and not crash
Push-Location $exeDirectory

try {
    & $cdbPath @cdbArgs
} finally {
    Pop-Location
    # Clean up temp script
    Remove-Item -Path $scriptFile -Force -ErrorAction SilentlyContinue
}

$debugExit = $LASTEXITCODE

Write-Info ""
if ($debugExit -eq 0) {
    Write-Info "Debug session ended normally." -Color Green
} else {
    Write-Info "Debug session ended with exit code $debugExit" -Color Yellow
}
