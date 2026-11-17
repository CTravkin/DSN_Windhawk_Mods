# Cursor Scheme Extension

**English** · [Русский](README.ru.md) · [DSN Windhawk Mods](../README.md)

Cursor Scheme Extension replaces built-in application pointers with pointers
from the selected Windows scheme.

## Status

The mod recognizes native Windows and Common Controls autoscroll cursors,
Windows Forms pointers, and shared Chromium assets in Electron and compatible
embedded runtimes. Gecko, Qt, and classic desktop Office have separate controls.

Windhawk can load the mod in graphical desktop processes. Recognition is cached
per GUI thread, and rotated arrows are built in memory when needed.

Published cursor handles remain valid until the mod restores the process's
input queues. The cache holds up to 128 instances across active and stopped
sessions. While full, further requests use the application's original cursor.
An unresponsive input thread restores its cursor when it resumes. Callback code
can remain resident until its owning thread exits, including when a removed
hook is still returning. If Windows cannot enumerate input owners, their cursor
handles remain valid until the process exits.
If an untracked restore window cannot be destroyed, its callback code also
remains resident until the process exits.

Excel draws the stationary marker at the click position. The mod applies the
Windows scheme to the pointer at the start and during autoscroll. Firefox
draws its autoscroll marker separately from the pointer. That marker is outside
the current coverage.

## Features

- At the starting point, the pointer uses the Vertical resize, Horizontal
  resize, or Move role, according to the axes indicated by the application.
- During scrolling, the Alternate select (`Up`) arrow points in the active
  direction. Optional smoothing applies in all eight directions. Monochrome
  effects that invert the background keep their original pixel rendering.
  Animated scheme cursors retain their frame order and timing.
- Recognized drag pointers for cells, ranges, and objects use Move. Dividers
  and corner handles use the horizontal, vertical, or diagonal Resize roles.
  Windows and Chromium already use the active scheme for standard Move and
  Resize pointers.
- In Office, the pointing hand uses Link select and margin selection mirrors
  Normal select. Arrows for selecting rows, columns, or an entire table rotate
  Alternate select in the direction of the original arrow.
- Cell selection in Excel, Chromium, and Gecko uses Precision select.
  The vertical text pointer rotates Text select. Transforms preserve the hotspot.
- Gecko column and row dividers, Qt splitters, and Windows Forms splitters
  use the corresponding Resize roles. Windows Forms autoscroll follows the
  Windows option.

## Platforms

Windows 11 with [Windhawk](https://windhawk.net/). Builds are available for x86,
x64, and ARM64. Runtime checks cover x86 and x64 on Windows 11 x64. ARM64 has
been compiled without a runtime check. Microsoft 365 and Qt WebEngine have
not been verified.

## Operations

### Installation

1. In Windhawk, select **Create a New Mod**.
2. Replace `mod.wh.cpp` with the complete [mod source](Mod/cursor-scheme-extension.wh.cpp).
3. Select **Compile Mod**, exit the editor, and enable **Cursor Scheme Extension**.

### Usage

Choose application families under **Applications and processes** and individual
shapes under **Pointer types**. Windows, Chromium, Gecko, Qt, and all pointer
types are on by default. Office covers desktop applications and is opt-in.

Add one executable name or full path per row under **Application exclusions**,
below the family switches. A name matches any location. A full path matches a
specific installation.

The **Smoothing** group has separate switches for autoscroll activation,
all eight autoscroll arrows, and selection arrows. All three are on by default.
Change the pointer scheme and individual roles in Windows.

### Manual preview

The [Win32 preview](Tests/cursor-scheme-extension-preview.cpp) can display all
eleven system cursor states. Build it as `cursor-scheme-extension-preview.exe`
and open it with the mod enabled. Wheel-click **Vertical**, **Horizontal**, or
**Both axes**. Move away from the starting point and back. In **Both axes**,
try the four diagonals too. Press **F2** to try the Windows Reader Mode path.

### Lifecycle tests

Run [Run-LifecycleTests.ps1](Tests/Run-LifecycleTests.ps1) in PowerShell to check
reentrant callbacks, shared input queues, handle reuse, and restoration on x86
and x64. The tests use isolated processes and preserve the installed settings.
