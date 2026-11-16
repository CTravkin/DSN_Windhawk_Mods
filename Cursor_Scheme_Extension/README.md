# Cursor Scheme Extension

**English** · [Русский](README.ru.md) · [DSN Windhawk Mods](../README.md)

Cursor Scheme Extension applies the active Windows cursor scheme to recognized
application pointers.

## Status

The mod recognizes native Windows autoscroll cursors, including Common Controls
Reader Mode. It identifies the shared Chromium and Electron cursors by image.
Classic desktop Office uses the same replacement mechanism for its recognized
pointers.

Windhawk can load the mod in graphical desktop processes. Recognition is cached
per GUI thread, and rotated arrows are built in memory when needed.

Excel draws the stationary marker at the click position. The mod applies the
Windows scheme to the pointer at the start and during autoscroll.

## Features

- At the starting point, the pointer uses the Vertical resize, Horizontal
  resize, or Move role, according to the axes indicated by the application.
- During scrolling, the Alternate select (`Up`) arrow points in the active
  direction. Optional smoothing applies in all eight directions. Monochrome
  effects that invert the background keep their original pixel rendering.
- Panel, row, and column dividers use the corresponding Resize roles.
- In Office, the link pointer uses Link select, margin selection mirrors Normal
  select, and the downward selection arrow rotates Alternate select.
- In Chromium and Electron, cell selection uses Precision select and the
  vertical text pointer rotates Text select. Transforms preserve the hotspot.

## Platforms

Windows 11 with [Windhawk](https://windhawk.net/). Builds are available for x86,
x64, and ARM64. Runtime checks use Windows 11 x64.

## Operations

### Installation

1. In Windhawk, select **Create a New Mod**.
2. Replace `mod.wh.cpp` with the complete [mod source](Mod/cursor-scheme-extension.wh.cpp).
3. Select **Compile Mod**, exit the editor, and enable **Cursor Scheme Extension**.

### Usage

Choose application families under **Where to replace pointers** and individual
types under **Which pointers to replace**. Chromium and Electron and all pointer
types are on by default. Classic Office is opt-in. Windows and Common Controls
autoscroll follow the two autoscroll type switches.

**Smooth autoscroll arrows** is on by default and applies to the eight
directional arrows. Change the cursor scheme and pointer roles in Windows.

When upgrading from 0.6.x in the Windhawk editor, review the two application
switches in their new group.

### Manual preview

The [Win32 preview](Tests/cursor-scheme-extension-preview.cpp) can display all
eleven system cursor states. Build it as `cursor-scheme-extension-preview.exe`
and open it with the mod enabled. Wheel-click **Vertical**, **Horizontal**, or
**Both axes**. Move away from the starting point and back. In **Both axes**,
try the four diagonals too. Press **F2** to try the Windows Reader Mode path.
