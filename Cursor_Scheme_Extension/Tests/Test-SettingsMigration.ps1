param([string]$OutputDirectory)

$ErrorActionPreference = 'Stop'
if (!$OutputDirectory) {
    $OutputDirectory = Join-Path (Split-Path (Split-Path $PSScriptRoot -Parent) -Parent) ('Builds\Cursor_Scheme_Extension\migration-' + [guid]::NewGuid())
}
$testPath = 'HKCU:\Software\DSN\WindhawkTests\' + [guid]::NewGuid()
$tool = Join-Path (Split-Path $PSScriptRoot -Parent) 'Tools\Migrate-Settings.ps1'
try {
    foreach ($value in 0, 1) {
        $path = Join-Path $testPath "legacy-$value"
        New-Item -Path $path -Force | Out-Null
        New-ItemProperty -LiteralPath $path -Name 'cursors.downSelection' -Value $value -PropertyType DWord | Out-Null
        $backup = Join-Path $OutputDirectory "legacy-$value.reg"
        $result = & $tool -SettingsPath $path -BackupPath $backup
        if (!$result.Changed -or (Get-ItemPropertyValue -LiteralPath $path -Name 'cursors.selectionArrows') -ne $value) {
            throw "Migration did not preserve value $value"
        }
        $hash = (Get-FileHash -LiteralPath $backup).Hash
        $result = & $tool -SettingsPath $path -BackupPath $backup
        if ($result.Changed -or (Get-FileHash -LiteralPath $backup).Hash -ne $hash) {
            throw 'Repeated migration changed settings or overwrote the backup'
        }
    }
    $path = Join-Path $testPath 'current'
    New-Item -Path $path -Force | Out-Null
    New-ItemProperty -LiteralPath $path -Name 'cursors.downSelection' -Value 1 -PropertyType DWord | Out-Null
    New-ItemProperty -LiteralPath $path -Name 'cursors.selectionArrows' -Value 0 -PropertyType DWord | Out-Null
    $result = & $tool -SettingsPath $path -BackupPath (Join-Path $OutputDirectory 'current.reg')
    if ($result.Changed -or (Get-ItemPropertyValue -LiteralPath $path -Name 'cursors.selectionArrows') -ne 0) {
        throw 'Migration overwrote an existing preference'
    }
    $path = Join-Path $testPath 'fresh'
    New-Item -Path $path -Force | Out-Null
    $result = & $tool -SettingsPath $path -BackupPath (Join-Path $OutputDirectory 'fresh.reg')
    if ($result.Changed -or (Get-Item -LiteralPath $path).GetValueNames().Count) {
        throw 'Migration changed a fresh settings key'
    }
    'settings-migration: PASS (disabled, enabled, repeated, existing, fresh)'
} finally {
    if (Test-Path -LiteralPath $testPath) { Remove-Item -LiteralPath $testPath -Recurse -Force }
}
