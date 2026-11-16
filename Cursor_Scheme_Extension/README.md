# Cursor Scheme Extension

**English** · [Русский](README.ru.md) · [DSN Windhawk Mods](../README.md)

Cursor Scheme Extension uses the active Windows cursor scheme for native
autoscroll started by pressing the mouse wheel.

## Status

The preview supports the native autoscroll cursors from Windows. The optional
Office mode also recognizes the same pointer shapes in classic desktop Office
apps, independent of Office cursor resource numbers. Resource matching has been
checked with Excel from Office LTSC 2024 x64. Applications can also draw a
separate stationary origin marker.

## Features

- At the starting point, the pointer uses the Vertical resize, Horizontal
  resize, or Move role, according to the axes indicated by the application.
- During scrolling, the Alternate select (`Up`) arrow points in the active
  direction. The mod rotates the arrow in memory, smooths diagonal edges, and
  reads the current scheme when autoscroll starts.

## Platforms

Windows 11 with [Windhawk](https://windhawk.net/). Builds are available for x86,
x64, and ARM64. Runtime checks use Windows 11 x64.

## Operations

### Installation

1. In Windhawk, select **Create a New Mod**.
2. Replace `mod.wh.cpp` with the complete [mod source](Mod/cursor-scheme-extension.wh.cpp).
3. Select **Compile Mod**, exit the editor, and enable **Cursor Scheme Extension**.

### Usage

Press the mouse wheel inside a scrollable area in an application that uses native
autoscroll. Change the cursor scheme or its individual pointer roles in Windows
as usual.

| Windhawk setting | Default | Effect |
| --- | --- | --- |
| Replace cursors in Office | Off | Use the active Windows scheme for recognized autoscroll pointers in classic desktop Office apps |
| Smooth diagonal arrows | On | Use partially transparent edge pixels when rotating diagonally. Turning this off uses the original pixels for the rotation |

### Manual preview

The [Win32 preview](Tests/cursor-scheme-extension-preview.cpp) can display all
eleven system cursor states. Build it as `cursor-scheme-extension-preview.exe`
and open it with the mod enabled. Wheel-click **Vertical**, **Horizontal**, or
**Both axes**. Move away from the starting point and back. In **Both axes**,
try the four diagonals too. Press **F2** to try the Windows Reader Mode path.
