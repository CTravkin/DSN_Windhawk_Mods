# Quick Link Menu

**English** · [Русский](README.ru.md)

Quick Link Menu customizes the native Windows menu opened by **Win+X** or by
right-clicking the Start button. It organizes Windows Settings, classic Control
Panel, administration tools, and basic commands in four groups.

## Features

- Click a row or press **Enter** for its main action. Click the arrow or press
  **Right** for related shortcuts. The power row opens the native power submenu
  with **Lock** first.
- File Explorer opens its configured start page. Its submenu offers shortcuts
  to the current user's folders and other shell locations.
- Menu items use Segoe Fluent Icons in the native icon slots. Settings shortcuts
  follow the icons in Settings where the corresponding glyph is available.
- Shortcut captions follow the Windows display language. Settings shortcuts use
  the page titles, and classic tools use shell labels or localized Windows
  resources.

## Platforms

The mod targets Windows 11 x64. Native testing was performed on Windows 11
26H2, build 26300.10075. It is incompatible with Windows 10 Win+X Restorer.

## Usage

The switches in each Windhawk settings group follow the menu order.
Each row with a submenu groups **Show in menu** and its individual shortcuts
under the name of the main row.
**Search**, **Desktop**, and the **Documents** and **Pictures** shortcuts under
File Explorer start off. Items also depend on availability in Windows.

**Open submenus on hover** defaults to immediate opening from the row. You can
use the Windows delay or open immediately over the arrow. In the latter mode,
the arrow has a separate hover background and the row retains its native hover.

Turning off all power commands hides the power row. With all menu rows
disabled, the stock Win+X menu opens.

[Windhawk mod source](Mod/quick-link-menu.wh.cpp) ·
[DSN Windhawk Mods](../README.md)
