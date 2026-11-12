[CmdletBinding()]
param(
    [string]$Compiler = (Join-Path $env:ProgramFiles 'Windhawk\Compiler\bin\clang++.exe')
)

$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..\..')).Path
$buildRoot = Join-Path $repoRoot 'Builds\Explorer_Item_Pins\Tests'
$work = Join-Path $buildRoot ('pin-store-' + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $work | Out-Null
$exe = Join-Path $work 'pin-store.exe'
$source = Join-Path $PSScriptRoot 'pin-store.cpp'

& $Compiler --target=x86_64-w64-mingw32 -std=c++23 -D_WIN32_WINNT=0x0A00 -DWH_MOD -DWH_EDITING -DEXPLORER_ITEM_PINS_TEST -municode -Wall -Wextra -Werror -static -o $exe $source -lruntimeobject -lole32 -loleaut32 -lshell32 -luuid -ladvapi32
if ($LASTEXITCODE -ne 0) { throw "Compilation failed with exit code $LASTEXITCODE" }

& $exe self-test $work
if ($LASTEXITCODE -ne 0) { throw "Pin store self-test failed with exit code $LASTEXITCODE" }

$database = Join-Path $work 'concurrent\pins.json'
$writerCount = 8
$eventName = 'Local\ExplorerItemPinsTest-' + [guid]::NewGuid().ToString('N')
$startEvent = [Threading.EventWaitHandle]::new($false, [Threading.EventResetMode]::ManualReset, $eventName)
try {
    $quotedDatabase = '"{0}"' -f $database
    $writers = for ($i = 0; $i -lt $writerCount; ++$i) {
        $errors = Join-Path $work "writer-$i.err"
        $process = Start-Process -FilePath $exe -ArgumentList @('write', $quotedDatabase, $i, $eventName) -WindowStyle Hidden -RedirectStandardError $errors -PassThru
        [pscustomobject]@{ Process = $process; Errors = $errors }
    }
    $errors = Join-Path $work 'reader.err'
    $process = Start-Process -FilePath $exe -ArgumentList @('read', $quotedDatabase, $eventName) -WindowStyle Hidden -RedirectStandardError $errors -PassThru
    $reader = [pscustomobject]@{ Process = $process; Errors = $errors }
    [void]$startEvent.Set()
    foreach ($child in @($writers) + $reader) {
        $process = $child.Process
        if (-not $process.WaitForExit(15000)) { throw "Concurrent process $($process.Id) timed out" }
        if ($process.ExitCode -eq 0) { continue }
        $detail = ([string](Get-Content -Raw -LiteralPath $child.Errors)).Trim()
        throw "Concurrent process $($process.Id) exited with code $($process.ExitCode). $detail"
    }
} finally {
    $startEvent.Dispose()
}
& $exe verify-writers $database $writerCount
if ($LASTEXITCODE -ne 0) { throw "Concurrent writes test failed with exit code $LASTEXITCODE" }

$resolvedWork = (Resolve-Path -LiteralPath $work).Path
$resolvedBuild = (Resolve-Path -LiteralPath $buildRoot).Path
if ([IO.Path]::GetDirectoryName($resolvedWork) -ne $resolvedBuild -or $resolvedWork -ne $work) {
    throw "Unexpected test cleanup path: $resolvedWork"
}
Remove-Item -LiteralPath $resolvedWork -Recurse
