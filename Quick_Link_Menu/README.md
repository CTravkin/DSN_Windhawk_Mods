# Quick Link Menu

**English** · [Русский](README.ru.md)

Quick Link Menu customizes the native Windows menu opened by **Win+X** or by
right-clicking the Start button. It organizes Windows Settings, classic Control
Panel, administration tools, and basic commands in four groups.

## Features

- Click a row or press **Enter** for its main action. Click the arrow or press
  **Right** for related shortcuts.
- File Explorer opens its configured start page. Its submenu offers shortcuts
  to the current user's folders and other shell locations.
- Menu items use Segoe Fluent Icons in the native icon slots. Settings shortcuts
  follow the icons in Settings where the corresponding glyph is available.
- Shortcut captions follow the Windows display language. Settings shortcuts use
  the page titles, and classic tools use shell labels or localized Windows
  resources.

## Platforms

The mod targets Windows 11 x64. Win+X was verified on Windows 11 24H2 Home and
Pro (build 26100.4349) and Windows 11 26H2 (build 26300.10075). The mod is
incompatible with Windows 10 Win+X Restorer.

## Usage

The switches in each Windhawk settings group follow the menu order.
The main switch appears directly under the row's name, with any related
shortcuts grouped below it.
**Search**, **Desktop**, and the **Documents** and **Pictures** shortcuts under
File Explorer start off. Items also depend on availability in Windows.

In **General settings**, submenus open immediately on hover by default. You can
use a delay, hover over the arrow, or click the arrow.

**Hide administrator commands** is off by default. When enabled, it hides
known administrator commands if the current session needs credentials or policy
prevents elevation. Commands that only need UAC consent are still shown.

**Command indication** is on by default. The hand marks areas
that run a command. If a main destination is unavailable, its available
shortcuts stay in a submenu. A row with a main action and no available shortcuts
has no submenu arrow.

In **Command settings**, you can make administrator Terminal the main Terminal
action and select a main power command. The default is **Shut down**. Choosing
**None** keeps the general power submenu.

Turning off the power row hides it with its submenu. With all menu rows disabled,
the stock Win+X menu opens.

[Windhawk mod source](Mod/quick-link-menu.wh.cpp) ·
[DSN Windhawk Mods](../README.md)
