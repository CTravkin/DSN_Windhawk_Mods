# Cursor Scheme Extension

**English** · [Русский](README.ru.md) · [DSN Windhawk Mods](../README.md)

Cursor Scheme Extension uses the active Windows cursor scheme for native
autoscroll started by pressing the mouse wheel.

## Status

The mod recognizes native Windows autoscroll cursors, including Common Controls
Reader Mode. It identifies the shared Chromium and Electron cursors by image.
Classic desktop Office uses the same replacement mechanism for recognized
autoscroll, link, and text margin selection cursors.

Windhawk can load the mod in graphical desktop processes. Recognition is cached
per GUI thread, and rotated arrows are built in memory when needed.

Applications can draw a stationary starting marker themselves. Excel uses that
marker with recognized directional cursors from the Windows scheme.

## Features

- At the starting point, the pointer uses the Vertical resize, Horizontal
  resize, or Move role, according to the axes indicated by the application.
- During scrolling, the Alternate select (`Up`) arrow points in the active
  direction. Optional smoothing applies in all eight directions. Monochrome
  effects that invert the background keep their original pixel rendering.
- In Office, recognized hyperlink cursors use Link select and recognized text
  margin selection cursors mirror Normal select, including its hotspot.

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
| Replace cursors in Office | Off | Use the active Windows scheme for recognized cursors in classic desktop Office |
| Autoscroll in Chromium and Electron | On | Theme recognized Chromium and Electron autoscroll cursors |
| Smooth autoscroll arrows | On | Smooth edges with partially transparent pixels in all eight directions |

### Manual preview

The [Win32 preview](Tests/cursor-scheme-extension-preview.cpp) can display all
eleven system cursor states. Build it as `cursor-scheme-extension-preview.exe`
and open it with the mod enabled. Wheel-click **Vertical**, **Horizontal**, or
**Both axes**. Move away from the starting point and back. In **Both axes**,
try the four diagonals too. Press **F2** to try the Windows Reader Mode path.
