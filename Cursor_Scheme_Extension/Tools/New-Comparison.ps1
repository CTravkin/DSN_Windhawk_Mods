<#
Renders comparison artwork offscreen from installed CUR files and Office resources.
Does not capture the desktop, run Office, change the active scheme, or install the mod.
Requires Windows x64, the Windhawk compiler, Office, and the illustrated themes.
Theme directories use link.cur, up.cur, ns.cur, ew.cur, move.cur and cross.cur.
Intermediate sprites and logs stay in ignored Builds/Cursor_Scheme_Extension/Comparison.
#>
[CmdletBinding()]
param(
    [string]$OutputDirectory = (Join-Path $PSScriptRoot '..\Assets\Previews'),
    [string]$WindowsCursors = (Join-Path $env:WINDIR 'Cursors'),
    [string]$W11Directory = (Join-Path $env:WINDIR 'Cursors\W11 Concept Light Classic'),
    [string]$VisionDirectory = (Join-Path $env:WINDIR 'Cursors\Vision White'),
    [string]$OfficeExecutable = (Join-Path $env:ProgramFiles 'Microsoft Office\root\Office16\EXCEL.EXE'),
    [string]$CompilerRoot = 'C:\Program Files\Windhawk\Compiler'
)
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing
$cursorProject = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$cursorBuild = Join-Path $cursorProject 'Builds\Cursor_Scheme_Extension\Comparison'
$cursorSprites = Join-Path $cursorBuild 'Sprites'
New-Item -ItemType Directory -Path $cursorBuild,$cursorSprites,$OutputDirectory -Force | Out-Null
$cursorExecutable = Join-Path $cursorBuild 'Extract-Comparison.exe'
$cursorCompilerArguments = @(
    '-target', 'x86_64-w64-windows-gnu', '-std=c++23', '-static', '-municode', '-O2',
    '-DUNICODE', '-D_UNICODE', '-DWH_MOD', '-DWH_EDITING',
    '-D_WIN32_WINNT=0x0A00', '-DWINVER=0x0A00',
    '-I', (Join-Path $CompilerRoot 'include'),
    (Join-Path $PSScriptRoot 'Extract-Comparison.cpp'),
    '-lgdiplus', '-lole32', '-lgdi32', '-ladvapi32', '-lshell32', '-o', $cursorExecutable
)
& (Join-Path $CompilerRoot 'bin\clang++.exe') @cursorCompilerArguments
if ($LASTEXITCODE -ne 0) { throw 'Comparison renderer compilation failed' }
$cursorNativeArguments = @($cursorSprites,$WindowsCursors,$W11Directory,$VisionDirectory,$OfficeExecutable)
foreach ($cursorArgument in $cursorNativeArguments) {
    if ($cursorArgument.Contains('"') -or $cursorArgument.EndsWith('\')) {
        throw 'Comparison paths must not contain quotes or end in a backslash'
    }
    if (-not (Test-Path -LiteralPath $cursorArgument)) { throw "Missing comparison input: $cursorArgument" }
}
$cursorQuotedArguments = ($cursorNativeArguments | ForEach-Object { '"' + $_ + '"' }) -join ' '
$cursorProcess = Start-Process -FilePath $cursorExecutable -ArgumentList $cursorQuotedArguments `
    -WindowStyle Hidden -PassThru `
    -RedirectStandardOutput (Join-Path $cursorBuild 'Extract.log') `
    -RedirectStandardError (Join-Path $cursorBuild 'Extract.err')
if (-not $cursorProcess.WaitForExit(20000)) {
    Stop-Process -Id $cursorProcess.Id
    throw 'Comparison renderer timed out'
}
Get-Content -LiteralPath (Join-Path $cursorBuild 'Extract.log')
if ($cursorProcess.ExitCode -ne 0) {
    Get-Content -LiteralPath (Join-Path $cursorBuild 'Extract.err')
    throw "Comparison renderer exited $($cursorProcess.ExitCode)"
}

$cursorOutput = Join-Path $OutputDirectory 'Comparison.en.png'
$cursorScale = 2
$cursorWidth = 852
$cursorHeight = 620
$cursorBitmap = [System.Drawing.Bitmap]::new($cursorWidth * $cursorScale, $cursorHeight * $cursorScale)
$cursorGraphics = [System.Drawing.Graphics]::FromImage($cursorBitmap)
$cursorGraphics.Clear([System.Drawing.ColorTranslator]::FromHtml('#0d1117'))
$cursorGraphics.ScaleTransform($cursorScale, $cursorScale)
$cursorGraphics.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::AntiAliasGridFit
$cursorGraphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
$cursorText = [System.Drawing.SolidBrush]::new([System.Drawing.ColorTranslator]::FromHtml('#e6edf3'))
$cursorMuted = [System.Drawing.SolidBrush]::new([System.Drawing.ColorTranslator]::FromHtml('#8b949e'))
$cursorAccent = [System.Drawing.SolidBrush]::new([System.Drawing.ColorTranslator]::FromHtml('#79c0ff'))
$cursorLine = [System.Drawing.Pen]::new([System.Drawing.ColorTranslator]::FromHtml('#21262d'), 1)
$cursorNameFont = [System.Drawing.Font]::new('Segoe UI', 17, [System.Drawing.FontStyle]::Bold, [System.Drawing.GraphicsUnit]::Pixel)
$cursorLabelFont = [System.Drawing.Font]::new('Segoe UI', 14, [System.Drawing.FontStyle]::Regular, [System.Drawing.GraphicsUnit]::Pixel)
$cursorCenter = [System.Drawing.StringFormat]::new()
$cursorCenter.Alignment = [System.Drawing.StringAlignment]::Center
$cursorCenter.LineAlignment = [System.Drawing.StringAlignment]::Center

function Draw-Text([string]$Text, $Font, $Brush, [single]$X, [single]$Y, [single]$Width, [single]$Height, $Format) {
    $cursorGraphics.DrawString($Text, $Font, $Brush, [System.Drawing.RectangleF]::new($X,$Y,$Width,$Height), $Format)
}

$cursorGroups = @('Without mod', 'With mod')
$cursorStart = 24
$cursorCell = 126
$cursorGap = 24
$cursorNames = @('Aero (Windows Default)', 'W11 Concept', 'Vision')
for ($cursorTheme = 0; $cursorTheme -lt 3; $cursorTheme++) {
    $cursorBase = $cursorStart + $cursorTheme * (2 * $cursorCell + $cursorGap)
    Draw-Text $cursorNames[$cursorTheme] $cursorNameFont $cursorText $cursorBase 24 (2 * $cursorCell) 28 $cursorCenter
    for ($cursorGroup = 0; $cursorGroup -lt 2; $cursorGroup++) {
        $cursorX = $cursorBase + $cursorGroup * $cursorCell
        Draw-Text $cursorGroups[$cursorGroup] $cursorLabelFont $(if ($cursorGroup) {$cursorAccent} else {$cursorMuted}) $cursorX 60 $cursorCell 24 $cursorCenter
    }
    if ($cursorTheme -lt 2) { $cursorGraphics.DrawLine($cursorLine, $cursorBase + 2 * $cursorCell + $cursorGap / 2, 24, $cursorBase + 2 * $cursorCell + $cursorGap / 2, 596) }
}
$cursorGraphics.DrawLine($cursorLine, 24, 96, 828, 96)
$cursorTypes = @('hand', 'origin', 'up', 'diagonal', 'resize', 'cell')
$cursorThemes = @('aero', 'w11', 'vision')
$cursorAxes = @('vertical', 'horizontal', 'both')
for ($cursorRow = 0; $cursorRow -lt 6; $cursorRow++) {
    $cursorY = 104 + $cursorRow * 82
    for ($cursorTheme = 0; $cursorTheme -lt 3; $cursorTheme++) {
        for ($cursorGroup = 0; $cursorGroup -lt 2; $cursorGroup++) {
            $cursorSpriteName = if ($cursorGroup) {
                "after-$($cursorThemes[$cursorTheme])-$($cursorTypes[$cursorRow])"
            } elseif ($cursorRow -eq 1) {
                "before-origin-$($cursorAxes[$cursorTheme])"
            } else { "before-$($cursorTypes[$cursorRow])" }
            $cursorImage = [System.Drawing.Image]::FromFile((Join-Path $cursorSprites "$cursorSpriteName.png"))
            try {
                $cursorX = $cursorStart + $cursorTheme * (2 * $cursorCell + $cursorGap) + $cursorGroup * $cursorCell
                $cursorDestination = [System.Drawing.Rectangle]::new(
                    [int]($cursorX + ($cursorCell - 2 * $cursorImage.Width) / 2),
                    [int]($cursorY + (82 - 2 * $cursorImage.Height) / 2),
                    $cursorImage.Width * 2, $cursorImage.Height * 2)
                $cursorGraphics.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::NearestNeighbor
                $cursorGraphics.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::Half
                $cursorGraphics.DrawImage($cursorImage, $cursorDestination, 0, 0, $cursorImage.Width, $cursorImage.Height, [System.Drawing.GraphicsUnit]::Pixel)
            } finally { $cursorImage.Dispose() }
        }
    }
    if ($cursorRow -lt 5) { $cursorGraphics.DrawLine($cursorLine, 24, $cursorY + 82, 828, $cursorY + 82) }
}
$cursorGraphics.Dispose()
$cursorBitmap.Save($cursorOutput, [System.Drawing.Imaging.ImageFormat]::Png)
$cursorBitmap.Dispose()
foreach ($cursorResource in @($cursorText,$cursorMuted,$cursorAccent,$cursorLine,$cursorNameFont,$cursorLabelFont,$cursorCenter)) { $cursorResource.Dispose() }
Get-Item -LiteralPath $cursorOutput | Select-Object FullName,Length
