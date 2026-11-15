$ErrorActionPreference='Stop'
$root=$PSScriptRoot
$moduleRoot=Split-Path -Parent $root
$mod=Join-Path $moduleRoot 'Mod'
$build=Join-Path (Split-Path -Parent $moduleRoot) ('Builds/'+(Split-Path -Leaf $moduleRoot))
$buildTemp=Join-Path $build 'BuildTemp'
$metadata=Get-Content -LiteralPath (Join-Path $mod 'Source/sound-native.cpp') -Raw
$modId=[regex]::Match($metadata,'(?m)^// @id\s+(\S+)').Groups[1].Value
$modName=[regex]::Match($metadata,'(?m)^// @name[ \t]+([^\r\n]+)').Groups[1].Value
$modVersion=[regex]::Match($metadata,'(?m)^// @version\s+(\S+)').Groups[1].Value
$buildNumber=[regex]::Match($metadata,'(?m)^// Build: (\d+)').Groups[1].Value
if(!$modId -or !$modName -or !$modVersion -or !$buildNumber){throw 'Missing mod metadata'}
$output=Join-Path $build "$modName $modVersion (build $buildNumber) x64.dll"
& python (Join-Path $root 'package_mod.py')
if($LASTEXITCODE){throw 'Packaging failed'}
New-Item -ItemType Directory -Force $buildTemp | Out-Null
$oldTemp=$env:TEMP;$oldTmp=$env:TMP
$env:TEMP=$buildTemp;$env:TMP=$env:TEMP
Push-Location 'C:/Program Files/Windhawk/Compiler'
try{
 # Arguments go through a response file (POSIX quoting): Windows PowerShell 5.1
 # strips the inner quotes of -DWH_MOD_ID=L"..." when it starts a native program.
 $arguments=@('-std=c++23','-O1','-DUNICODE','-D_UNICODE','-DWIN32_LEAN_AND_MEAN','-D_WIN32_WINNT=0x0A00','-DWINVER=0x0A00','-DNTDDI_VERSION=0x0A000008','-target','x86_64-w64-mingw32','-shared','-DWH_MOD',('-DWH_MOD_ID=L"'+$modId+'"'),('-DWH_MOD_VERSION=L"'+$modVersion+'"'),'-include','windhawk_api.h',(Join-Path $mod ($modId+'.wh.cpp')),'C:/Program Files/Windhawk/Engine/1.7.3/64/windhawk.lib','-lcomctl32','-lole32','-loleaut32','-lruntimeobject','-liphlpapi','-lshlwapi','-lshell32','-lpowrprof','-luuid','-Wl,--export-all-symbols','-o',$output)
 $rsp=Join-Path $env:TEMP 'clang-args.rsp'
 [IO.File]::WriteAllLines($rsp,[string[]]($arguments|ForEach-Object{
  $argument=if($_ -match '^[A-Za-z]:[\\/]'){ $_.Replace('\','/') }else{ $_ }
  "'"+($argument -replace "'","'\''")+"'"
 }))
 & './bin/clang++.exe' '--rsp-quoting=posix' "@$rsp"
 if($LASTEXITCODE){throw "Compilation failed: $LASTEXITCODE"}
}finally{Pop-Location;$env:TEMP=$oldTemp;$env:TMP=$oldTmp}
Write-Output "Built: $output"
