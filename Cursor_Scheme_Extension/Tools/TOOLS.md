# Cursor Scheme Extension tools

| Tool | Purpose |
| --- | --- |
| `Extract-Comparison.cpp` | Extract cursor artwork for the comparison image |
| `New-Comparison.ps1` | Build the comparison image from cursor artwork |
| `Migrate-Settings.ps1` | Copy the selection-arrow preference from versions up to 1.0.2 to its new key, after exporting a verified registry backup |

Run `Migrate-Settings.ps1 -BackupPath <new-backup.reg>` before compiling the updated local mod in the system Windhawk installation. An elevated PowerShell session is required for the default machine-wide registry path. Use `-SettingsPath` for another local mod ID. An existing new preference takes precedence; the old value remains available for rollback.
