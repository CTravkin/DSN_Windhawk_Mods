# Quick Link Menu

**English** · [Русский](README.ru.md)

Quick Link Menu customizes the native Windows 11 menu opened by **Win+X** or by
right-clicking the Start button.

## Features

### Customization and command availability

- **Menu contents.** Hide individual rows, submenu commands, or entire sections.
  Turning off a section preserves its settings. Turning off a row hides its submenu.
- **Automatic filtering.** The mod hides unavailable commands based on the Windows
  edition, installed components, hardware, and system policies. If a main action is
  unavailable, its available shortcuts remain. An empty menu falls back to standard Win+X.
- **User permissions.** An optional filter hides administrator commands that require
  credentials or whose elevation is blocked. Commands that only need UAC consent
  stay visible.
- **Submenu controls.** Submenus open immediately on hover by default. Other options
  include delayed hover, hovering over the arrow, and clicking the arrow. A hand
  cursor marks areas that run a command. This indicator can be turned off.
- **Terminal.** The main Terminal action can run as administrator.

### Menu sections

- **Settings.** Open Windows Settings categories or go directly to pages such
  as Display, Sound, VPN, Default apps, and Windows Update.
- **Control Panel.** Open classic categories and tools such as Device Manager,
  Network Connections, Power Options, and Programs and Features.
- **Administration.** Open Computer Management, Services, Event Viewer, Registry
  Editor, and Terminal. Related shortcuts include Task Scheduler, certificates,
  Windows PowerShell, and Command Prompt.
- **Basic commands.** Open Run, Task Manager, Search, and File Explorer, show
  the desktop, or access power and sign-in options. File Explorer opens its
  configured start page. Its submenu includes shortcuts such as This PC, Downloads,
  and the Recycle Bin. Search, Desktop, and the Documents and Pictures shortcuts in
  File Explorer's submenu are off by default.

### Quick access to power actions

The main power row shuts down Windows in one click by default. It can run another
action, such as Restart or Lock. Choose **None** to keep the power submenu on its own.

### Labels and icons

The menu uses Windows labels in the system display language and Segoe Fluent Icons.
Settings shortcuts use the matching Windows icons where available.

Windhawk settings are available in English, Russian, Spanish, German, Brazilian
Portuguese, and Simplified Chinese.

## Platforms

Requires [Windhawk](https://windhawk.net/) and Windows 11 x64 or ARM64.
Win+X interaction was tested on Windows 11 24H2 Home and Pro x64 (build
`26100.4349`) with Windhawk 1.7.3. An earlier version was also tested on
Windows 11 26H2 x64 (build `26300.10075`). ARM64 verification in GitHub Actions
covers native compilation and automated command availability tests.

### Compatibility

Quick Link Menu works with the standard Windows 11 Win+X menu. Mods that replace
it are incompatible. One example is [Windows 10 Win+X restorer on Win11 24H2](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/windows-10-winx-restorer.wh.cpp).

## Operations

### Installation

1. Open Windhawk and choose **Create a New Mod**.
2. Replace the contents of `mod.wh.cpp` with the complete [mod source](Mod/quick-link-menu.wh.cpp).
3. Select **Compile Mod** or press `Ctrl+B` with the Windhawk sidebar open. Exit
   the editor and check that **Quick Link Menu** is enabled on the home page.

To update a local installation, open the installed mod in the Windhawk editor,
replace its source, and compile again.

[Windhawk editing mode guide](https://github.com/ramensoftware/windhawk/wiki/Editing-mode-guidelines)

### Usage

Click a row's label or press **Enter** to run its main action. Click the arrow
or press **→** to open the submenu. Click the label if a row only has a submenu.

To change the menu, open **Quick Link Menu → Settings** in Windhawk. Section
switches are below their descriptions. Row and submenu switches follow the menu
order. Find submenu behavior and the administrator filter under **General settings**,
and Terminal and power actions under **Command settings**.

### Verification

Run both commands from the repository root with Node.js.

```powershell
node Quick_Link_Menu/Tools/package-mod.mjs --check
node --test Quick_Link_Menu/Tests/menu-settings.test.mjs
```

The first command checks that the standalone source matches its inputs. The
tests cover menu settings, defaults, and localization.

[DSN Windhawk Mods](../README.md)
