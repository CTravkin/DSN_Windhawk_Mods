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
        $source, '-lgdi32', '-ladvapi32', '-o', $executable
    )
    & $compiler @arguments
    if ($LASTEXITCODE -ne 0) {
        throw "Lifecycle test compilation failed ($architecture)"
    }

    foreach ($scenario in 'host-reentrant', 'host-throttle', 'unload-contract', 'reentrant', 'shared', 'cache', 'slow', 'reload',
                          'inherited', 'stopping', 'limit', 'reuse', 'refresh', 'command-line', 'selection-setting') {
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

    $fixture = Join-Path $buildRoot "cursor-unload-fixture-$architecture.dll"
    $unloadTest = Join-Path $buildRoot "cursor-unload-dll-tests-$architecture.exe"
    $common = @('-target', "$architecture-w64-windows-gnu", '-std=c++23', '-static',
                '-O2', '-DUNICODE', '-D_UNICODE', '-D_WIN32_WINNT=0x0A00', '-DWINVER=0x0A00')
    & $compiler @common '-shared' '-DWH_MOD' '-DWH_EDITING' `
        '-I' (Join-Path $CompilerRoot 'include') `
        (Join-Path $PSScriptRoot 'cursor-unload-fixture.cpp') `
        '-lgdi32' '-ladvapi32' '-o' $fixture
    if ($LASTEXITCODE -ne 0) { throw "Unload fixture compilation failed ($architecture)" }
    & $compiler @common '-municode' (Join-Path $PSScriptRoot 'cursor-unload-dll-tests.cpp') `
        '-lgdi32' '-o' $unloadTest
    if ($LASTEXITCODE -ne 0) { throw "Unload test compilation failed ($architecture)" }
    $unloadLog = Join-Path $buildRoot "cursor-native-unload-$architecture.txt"
    $process = Start-Process -FilePath $unloadTest -ArgumentList ('"' + $fixture + '"') `
        -WindowStyle Hidden -PassThru -RedirectStandardOutput $unloadLog
    if (-not $process.WaitForExit(10000)) {
        Stop-Process -Id $process.Id
        throw "Native DLL unload test timed out ($architecture)"
    }
    Get-Content -LiteralPath $unloadLog
    if ($process.ExitCode -ne 0) { throw "Native DLL unload test failed ($architecture)" }
}
