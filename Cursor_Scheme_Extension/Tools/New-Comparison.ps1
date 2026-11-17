<#
Renders comparison artwork offscreen from installed CUR files and Office resources.
Does not capture the desktop, run Office, change the active scheme, or install the mod.
#>
[CmdletBinding()]
param(
    [string]$OutputDirectory = (Join-Path $PSScriptRoot '..\Assets\Previews'),
    [string]$WindowsCursors = (Join-Path $env:WINDIR 'Cursors'),
    [string]$BibataDirectory = (Join-Path $env:WINDIR 'Cursors\Bibata Original Ice Regular'),
    [string]$W11Directory = (Join-Path $env:WINDIR 'Cursors\W11 Concept Light Classic'),
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
$cursorNativeArguments = @($cursorSprites,$WindowsCursors,$BibataDirectory,$W11Directory,$OfficeExecutable)
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

foreach ($Language in 'en','ru') {
$cursorOutput = Join-Path $OutputDirectory "Comparison.$Language.png"
$cursorScale = 2
$cursorWidth = 1120
$cursorHeight = 808
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
$cursorTitleFont = [System.Drawing.Font]::new('Segoe UI', 26, [System.Drawing.FontStyle]::Bold, [System.Drawing.GraphicsUnit]::Pixel)
$cursorGroupFont = [System.Drawing.Font]::new('Segoe UI', 16, [System.Drawing.FontStyle]::Bold, [System.Drawing.GraphicsUnit]::Pixel)
$cursorNameFont = [System.Drawing.Font]::new('Segoe UI', 14, [System.Drawing.FontStyle]::Bold, [System.Drawing.GraphicsUnit]::Pixel)
$cursorLabelFont = [System.Drawing.Font]::new('Segoe UI', 14, [System.Drawing.FontStyle]::Regular, [System.Drawing.GraphicsUnit]::Pixel)
$cursorSmallFont = [System.Drawing.Font]::new('Segoe UI', 12, [System.Drawing.FontStyle]::Regular, [System.Drawing.GraphicsUnit]::Pixel)
$cursorCenter = [System.Drawing.StringFormat]::new()
$cursorCenter.Alignment = [System.Drawing.StringAlignment]::Center
$cursorCenter.LineAlignment = [System.Drawing.StringAlignment]::Center
$cursorLeft = [System.Drawing.StringFormat]::new()
$cursorLeft.LineAlignment = [System.Drawing.StringAlignment]::Center

function Draw-Text([string]$Text, $Font, $Brush, [single]$X, [single]$Y, [single]$Width, [single]$Height, $Format) {
    $cursorGraphics.DrawString($Text, $Font, $Brush, [System.Drawing.RectangleF]::new($X,$Y,$Width,$Height), $Format)
}

Draw-Text 'Cursor Scheme Extension' $cursorTitleFont $cursorText 32 24 900 40 $cursorLeft
if ($Language -eq 'ru') {
    $cursorGroups = @('Без мода', 'С модом')
    $cursorRows = @('Рука', "Включение`nавтопрокрутки", "Автопрокрутка`nвверх", "Автопрокрутка`nвниз-вправо", "Изменение`nразмеров", "Выбор`nячейки")
    $cursorSubtitle = 'Одна схема Windows — во встроенных курсорах приложений'
    $cursorFootnote = 'Пропорции курсоров сохранены · Увеличено для сравнения · Сглаживание мода включено'
} else {
    $cursorGroups = @('Without mod', 'With mod')
    $cursorRows = @('Hand', "Autoscroll`nactivation", "Autoscroll`nup", "Autoscroll`ndown-right", 'Resize', "Cell`nselection")
    $cursorSubtitle = 'The selected Windows scheme, extended to application pointers'
    $cursorFootnote = 'Native cursor proportions · Enlarged for comparison · Mod smoothing enabled'
}
Draw-Text $cursorSubtitle $cursorLabelFont $cursorMuted 33 70 1000 26 $cursorLeft
$cursorStart = 214
$cursorCell = 138
$cursorGap = 24
$cursorNames = @('Aero', 'Bibata Original', 'W11 Concept')
$cursorVariants = @('Windows Default', 'Ice', 'Light Classic')
for ($cursorGroup = 0; $cursorGroup -lt 2; $cursorGroup++) {
    $cursorBase = $cursorStart + $cursorGroup * (3 * $cursorCell + $cursorGap)
    Draw-Text $cursorGroups[$cursorGroup] $cursorGroupFont $(if ($cursorGroup) {$cursorAccent} else {$cursorMuted}) $cursorBase 112 (3 * $cursorCell) 26 $cursorCenter
    for ($cursorColumn = 0; $cursorColumn -lt 3; $cursorColumn++) {
        $cursorX = $cursorBase + $cursorColumn * $cursorCell
        Draw-Text $cursorNames[$cursorColumn] $cursorNameFont $cursorText $cursorX 151 $cursorCell 24 $cursorCenter
        Draw-Text $cursorVariants[$cursorColumn] $cursorSmallFont $cursorMuted $cursorX 176 $cursorCell 20 $cursorCenter
    }
}
$cursorGraphics.DrawLine($cursorLine, 32, 213, 1088, 213)
$cursorGraphics.DrawLine($cursorLine, 640, 112, 640, 723)
$cursorTypes = @('hand', 'origin', 'up', 'diagonal', 'resize', 'cell')
$cursorThemes = @('aero', 'bibata', 'w11')
$cursorAxes = @('vertical', 'horizontal', 'both')
for ($cursorRow = 0; $cursorRow -lt 6; $cursorRow++) {
    $cursorY = 219 + $cursorRow * 82
    Draw-Text $cursorRows[$cursorRow] $cursorLabelFont $cursorText 33 $cursorY 170 82 $cursorLeft
    for ($cursorGroup = 0; $cursorGroup -lt 2; $cursorGroup++) {
        for ($cursorColumn = 0; $cursorColumn -lt 3; $cursorColumn++) {
            $cursorSpriteName = if ($cursorGroup) {
                "after-$($cursorThemes[$cursorColumn])-$($cursorTypes[$cursorRow])"
            } elseif ($cursorRow -eq 1) {
                "before-origin-$($cursorAxes[$cursorColumn])"
            } else { "before-$($cursorTypes[$cursorRow])" }
            $cursorImage = [System.Drawing.Image]::FromFile((Join-Path $cursorSprites "$cursorSpriteName.png"))
            try {
                $cursorX = $cursorStart + $cursorGroup * (3 * $cursorCell + $cursorGap) + $cursorColumn * $cursorCell
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
    if ($cursorRow -lt 5) { $cursorGraphics.DrawLine($cursorLine, 32, $cursorY + 82, 1088, $cursorY + 82) }
}
$cursorGraphics.DrawLine($cursorLine, 32, 723, 1088, 723)
Draw-Text $cursorFootnote $cursorSmallFont $cursorMuted 33 736 1055 24 $cursorLeft
Draw-Text 'Windows & Office: Microsoft · Bibata: ful1e5 · W11 Concept: jepriCreations' $cursorSmallFont $cursorMuted 33 766 1055 24 $cursorLeft
$cursorGraphics.Dispose()
$cursorBitmap.Save($cursorOutput, [System.Drawing.Imaging.ImageFormat]::Png)
$cursorBitmap.Dispose()
foreach ($cursorResource in @($cursorText,$cursorMuted,$cursorAccent,$cursorLine,$cursorTitleFont,$cursorGroupFont,$cursorNameFont,$cursorLabelFont,$cursorSmallFont,$cursorCenter,$cursorLeft)) { $cursorResource.Dispose() }
Get-Item -LiteralPath $cursorOutput | Select-Object FullName,Length
}
