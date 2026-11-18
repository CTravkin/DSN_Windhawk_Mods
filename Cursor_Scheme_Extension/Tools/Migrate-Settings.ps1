param(
    [string]$SettingsPath = 'HKLM:\SOFTWARE\Windhawk\Engine\Mods\local@cursor-scheme-extension\Settings',
    [Parameter(Mandatory)][string]$BackupPath
)

$ErrorActionPreference = 'Stop'
$settingsKey = Get-Item -LiteralPath $SettingsPath
$names = $settingsKey.GetValueNames()
$legacyName = 'cursors.downSelection'
$currentName = 'cursors.selectionArrows'
if ($names -contains $currentName -or $names -notcontains $legacyName) {
    [pscustomobject]@{ Changed = $false }
    return
}
if ($settingsKey.GetValueKind($legacyName) -ne [Microsoft.Win32.RegistryValueKind]::DWord) {
    throw 'The existing selection setting is not a DWORD'
}
$value = $settingsKey.GetValue($legacyName)
if ($value -notin 0, 1) { throw 'The existing selection setting is not a boolean' }
if (Test-Path -LiteralPath $BackupPath) { throw 'The settings backup already exists' }
$backupDirectory = Split-Path -Parent ([IO.Path]::GetFullPath($BackupPath))
New-Item -ItemType Directory -Path $backupDirectory -Force | Out-Null
& reg.exe export $settingsKey.Name $BackupPath /y | Out-Null
if ($LASTEXITCODE -ne 0 -or !(Test-Path -LiteralPath $BackupPath)) {
    throw 'The settings backup failed'
}
$backupText = Get-Content -LiteralPath $BackupPath -Raw
if (!$backupText.Contains('"cursors.downSelection"=dword:' + $value.ToString('x8'))) {
    throw 'The settings backup does not contain the original value'
}
New-ItemProperty -LiteralPath $SettingsPath -Name $currentName -Value $value -PropertyType DWord | Out-Null
if ((Get-ItemPropertyValue -LiteralPath $SettingsPath -Name $currentName) -ne $value) {
    throw 'The migrated selection setting does not match the original value'
}
# Retain the old value so reverting to 1.0.2 also preserves the preference.
[pscustomobject]@{ Changed = $true; SelectionArrows = $value; BackupPath = $BackupPath }
