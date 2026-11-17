param(
    [string]$CompilerRoot = 'C:\Program Files\Windhawk\Compiler'
)

$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$buildRoot = Join-Path $projectRoot 'Builds\Cursor_Scheme_Extension'
$source = Join-Path $PSScriptRoot 'cursor-lifecycle-tests.cpp'
$compiler = Join-Path $CompilerRoot 'bin\clang++.exe'
New-Item -ItemType Directory -Path $buildRoot -Force | Out-Null

foreach ($architecture in 'x86_64', 'i686') {
    $executable = Join-Path $buildRoot "cursor-lifecycle-tests-$architecture.exe"
    $arguments = @(
        '-target', "$architecture-w64-windows-gnu",
        '-std=c++23', '-static', '-O2',
        '-DUNICODE', '-D_UNICODE', '-D_WIN32_WINNT=0x0A00', '-DWINVER=0x0A00',
        '-DWH_MOD', '-DWH_EDITING',
        '-I', (Join-Path $CompilerRoot 'include'),
        $source, '-lgdi32', '-ladvapi32', '-lshell32', '-o', $executable
    )
    & $compiler @arguments
    if ($LASTEXITCODE -ne 0) {
        throw "Lifecycle test compilation failed ($architecture)"
    }

    foreach ($scenario in 'reentrant', 'unload', 'shared', 'cache', 'slow', 'reload',
                          'inherited', 'discovery-failure', 'responsive', 'cbt', 'orphan') {
        $log = Join-Path $buildRoot "cursor-lifecycle-$architecture-$scenario.txt"
        $launch = @{
            FilePath = $executable
            ArgumentList = $scenario
            WindowStyle = 'Hidden'
            PassThru = $true
            RedirectStandardOutput = $log
        }
        $process = Start-Process @launch
        if (-not $process.WaitForExit(8000)) {
            Stop-Process -Id $process.Id
            throw "Lifecycle test timed out ($architecture, $scenario)"
        }
        Get-Content -LiteralPath $log
        if ($process.ExitCode -ne 0) {
            throw "Lifecycle test failed ($architecture, $scenario)"
        }
    }
}
