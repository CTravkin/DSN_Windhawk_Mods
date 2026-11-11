[CmdletBinding()]
param(
    [string]$Compiler = (Join-Path $env:ProgramFiles 'Windhawk\Compiler\bin\clang++.exe')
)

$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..\..')).Path
$buildRoot = Join-Path $repoRoot 'Builds\Explorer_Item_Pins\Tests'
$work = Join-Path $buildRoot ('explorer-commands-' + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $work | Out-Null
$exe = Join-Path $work 'explorer-commands.exe'
$source = Join-Path $PSScriptRoot 'explorer-commands.cpp'

& $Compiler --target=x86_64-w64-mingw32 -std=c++23 -D_WIN32_WINNT=0x0A00 -municode -Wall -Wextra -Werror -static -o $exe $source -lruntimeobject -lole32 -loleaut32 -lshell32 -lshlwapi -luuid -luser32 -lgdi32 -lcomctl32
if ($LASTEXITCODE -ne 0) { throw "Compilation failed with exit code $LASTEXITCODE" }
& $exe $work commands
if ($LASTEXITCODE -ne 0) { throw "Native Explorer commands test failed with exit code $LASTEXITCODE" }
& $exe $work ordering
if ($LASTEXITCODE -ne 0) { throw "Native Explorer ordering test failed with exit code $LASTEXITCODE" }
& $exe $work grouping
if ($LASTEXITCODE -ne 0) { throw "Native Explorer grouping test failed with exit code $LASTEXITCODE" }
& $exe $work indicator
if ($LASTEXITCODE -ne 0) { throw "Native Explorer indicator test failed with exit code $LASTEXITCODE" }

$resolvedWork = (Resolve-Path -LiteralPath $work).Path
$resolvedBuild = (Resolve-Path -LiteralPath $buildRoot).Path
if ([IO.Path]::GetDirectoryName($resolvedWork) -ne $resolvedBuild -or $resolvedWork -ne $work) {
    throw "Unexpected test cleanup path: $resolvedWork"
}
Remove-Item -LiteralPath $resolvedWork -Recurse
