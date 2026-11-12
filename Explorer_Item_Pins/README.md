# Explorer Item Pins

**English** · [Русский](README.ru.md) · [DSN Windhawk Mods](../README.md)

Explorer Item Pins is a Windhawk mod in development for pinning files and folders to the top of their parent folder in Windows File Explorer.

## Features

- Pin or unpin a selected file or folder from the context menu or command bar.
- Pins stay at the top in either sort direction. When grouping is on, they appear in a separate group at the top.
- The context menu command, command bar button, and pin indicator can be switched off independently in Windhawk.

## Platforms

Tested on Windows 11 26H2 x64 (build `26300.9974`). Support for newer Explorer builds depends on available symbols and compatible internal interfaces.

## Data exchange

Pins are stored per Windows account in `%LOCALAPPDATA%\Explorer Item Pins\pins.json`. Each folder has its own list. Replacing a file at the same name keeps the pin. File identity can resolve an unambiguous rename within that folder. Copying or moving an item to another folder leaves its destination pin state up to that folder.

## Operations

### Verification

Run the tests from this mod directory on Windows with PowerShell and the compiler bundled with Windhawk 1.7.3.

```powershell
pwsh -NoProfile -File .\Tests\Test-PinStore.ps1
pwsh -NoProfile -File .\Tests\Test-ExplorerCommands.ps1
```

The tests cover storage, concurrent access, pin operations, sorting, grouping, and indicator rendering in a native Explorer view hosted in the test process.
