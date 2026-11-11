# Explorer Item Pins

**English** · [Русский](README.ru.md)

Explorer Item Pins is a Windhawk mod in development for pinning files and folders to the top of their parent folder in Windows File Explorer.

## Status

The current snapshot contains the pin store and item matching. Explorer commands, ordering, and the pin indicator are still in development.

## Data exchange

Pins are stored per Windows account in `%LOCALAPPDATA%\Explorer Item Pins\pins.json`. Each folder has its own list. Replacing a file at the same name keeps the pin. File identity can resolve an unambiguous rename within that folder. Copying or moving an item to another folder leaves its destination pin state up to that folder.

## Operations

### Verification

Run the storage tests on Windows with PowerShell and the compiler bundled with Windhawk 1.7.3.

```powershell
pwsh -NoProfile -File .\tests\Test-PinStore.ps1
```

The tests use a temporary database under `build\`. They check Unicode names, persistence, changes to files on the local volume, invalid data, and concurrent writers.
