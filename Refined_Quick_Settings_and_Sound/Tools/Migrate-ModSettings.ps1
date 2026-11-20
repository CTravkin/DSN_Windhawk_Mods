param(
    [Parameter(Mandatory)]
    [string] $InputPath
)

$ErrorActionPreference = 'Stop'
$configuration = Get-Content -LiteralPath $InputPath -Raw -Encoding UTF8 | ConvertFrom-Json
if ($configuration -isnot [pscustomobject]) {
    throw 'Expected the formatted Windhawk settings JSON object for this mod.'
}

$settings = [ordered]@{}
foreach ($property in $configuration.PSObject.Properties) {
    $settings[$property.Name] = $property.Value
}

function Move-Setting($previous, $current, $initial, [scriptblock] $convert) {
    if (!$settings.Contains($previous)) { return }
    $present = $settings.Contains($current)
    $emptyString = $initial -is [string] -and [string]::IsNullOrEmpty([string] $settings[$current])
    if (!$present -or $emptyString -or $settings[$current] -ceq $initial) {
        $value = $settings[$previous]
        if ($convert) { $value = & $convert $value }
        $settings[$current] = $value
    }
    $settings.Remove($previous)
}

Move-Setting 'quickSoundAccess' 'taskbar.openSoundFromVolumeIcon' 1
Move-Setting 'trayMiddleClickMute' 'taskbar.middleClickMute' 0
Move-Setting 'volumeWheelStep' 'sound.volumeWheelStep' 2
Move-Setting 'microphoneVolume' 'sound.microphoneVolumeMode' 'disabled'
Move-Setting 'soundSettingsPosition' 'sound.systemVolumeRight' 'soundSettings' {
    param($position)
    if ($position -ceq 'header') { 'deviceSettings' } else { 'soundSettings' }
}
Move-Setting 'spatialSoundMenu' 'sound.spatialSoundMenu' 'enabled'
Move-Setting 'appearance.soundPickers' 'sound.selectorShape' 'default'
Move-Setting 'brightnessMode' 'quickSettings.brightnessMode' 'auto'
if (![string]::IsNullOrEmpty([string] $settings['appearance.quickActionRows'])) {
    $settings.Remove('quickActionRows')
} else {
    $settings.Remove('appearance.quickActionRows')
}
Move-Setting 'appearance.quickActionRows' 'quickSettings.tileRows' '2'
Move-Setting 'quickActionRows' 'quickSettings.tileRows' '2'
Move-Setting 'appearance.quickTiles' 'quickSettings.tileShape' 'default'
for ($index = 1; $index -le 5; ++$index) {
    $initial = if ($index -eq 1) { 'display' } else { 'none' }
    Move-Setting "quickSettings.shortcut$index" "quickSettings.additionalShortcuts.shortcut$index" $initial
}

function Test-ReadableHiddenApps([string] $prefix) {
    $blanks = 0
    for ($index = 0; $blanks -lt 32; ++$index) {
        $value = [string] $settings["$prefix[$index]"]
        if ([string]::IsNullOrEmpty($value)) {
            ++$blanks
        } elseif ($value -match '[^ \t\r\n]') {
            return $true
        } else {
            $blanks = 0
        }
    }
    return $false
}

$oldApps = @($settings.Keys | Where-Object { $_ -cmatch '^hiddenApplications\[\d+\]$' })
$groupedApps = @($settings.Keys | Where-Object { $_ -cmatch '^sound\.hiddenApplications\[\d+\]$' })
if ($oldApps.Count -and !(Test-ReadableHiddenApps 'sound.hiddenApplications') -and
    (!$groupedApps.Count -or (Test-ReadableHiddenApps 'hiddenApplications'))) {
    # Treat the list as a unit so previously ignored rows cannot become active.
    foreach ($key in $groupedApps) { $settings.Remove($key) }
    foreach ($key in $oldApps) { $settings["sound.$key"] = $settings[$key] }
}
foreach ($key in $oldApps) { $settings.Remove($key) }

ConvertTo-Json -InputObject $settings -Depth 32
