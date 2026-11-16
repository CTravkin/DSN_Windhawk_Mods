# Cursor Scheme Extension

**English** · [Русский](README.ru.md) · [DSN Windhawk Mods](../README.md)

Cursor Scheme Extension uses the active Windows cursor scheme for native
autoscroll started by pressing the mouse wheel.

## Status

This prototype runs in the preview window, Excel, and Word. It targets the
shared autoscroll cursors provided by Windows User32. An application can draw
its own autoscroll pointer or stationary origin marker.

## Features

- At the starting point, the pointer uses the Vertical resize, Horizontal
  resize, or Move role, according to the axes indicated by the application.
- During scrolling, the Alternate select (`Up`) arrow points in the active
  direction. The mod rotates the arrow in memory and reads the current scheme
  when autoscroll starts.

## Platforms

Windows 11 x64 with [Windhawk](https://windhawk.net/).

## Operations

### Installation

1. In Windhawk, select **Create a New Mod**.
2. Replace `mod.wh.cpp` with the complete [mod source](Mod/cursor-scheme-extension.wh.cpp).
3. Select **Compile Mod**, exit the editor, and enable **Cursor Scheme Extension**.

### Usage

Press the mouse wheel inside a scrollable area in an application that uses native
autoscroll. Change the cursor scheme or its individual pointer roles in Windows
as usual.

### Manual preview

The [Win32 preview](Tests/cursor-scheme-extension-preview.cpp) can display all
eleven system cursor states. Build it as `cursor-scheme-extension-preview.exe`
and open it with the mod enabled. Wheel-click **Vertical**, **Horizontal**, or
**Both axes**. Move away from the starting point and back. In **Both axes**,
try the four diagonals too. Press **F2** to try the Windows Reader Mode path.
