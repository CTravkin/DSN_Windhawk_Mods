# Explorer Item Pins

**English** · [Русский](README.ru.md)

Explorer Item Pins is a Windhawk mod in development for pinning files and folders to the top of their parent folder in Windows File Explorer.

## Status

The current snapshot contains the pin store and pin/unpin operations for a single selection in a native Explorer view. The context menu, command bar, ordering, and pin indicator are still in development.

## Data exchange

Pins are stored per Windows account in `%LOCALAPPDATA%\Explorer Item Pins\pins.json`. Each folder has its own list. Replacing a file at the same name keeps the pin. File identity can resolve an unambiguous rename within that folder. Copying or moving an item to another folder leaves its destination pin state up to that folder.

## Operations

### Verification

Run the tests from this mod directory on Windows with PowerShell and the compiler bundled with Windhawk 1.7.3.

```powershell
pwsh -NoProfile -File .\tests\Test-PinStore.ps1
pwsh -NoProfile -File .\tests\Test-ExplorerCommands.ps1
```

The tests use temporary files under `build\`. They check storage, concurrent access, and pin/unpin operations against a native Explorer view hosted in the test process.
