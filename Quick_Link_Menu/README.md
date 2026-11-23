# Quick Link Menu

**English** · [Русский](README.ru.md)

Quick Link Menu customizes the native Windows 11 menu opened by **Win+X** or by
right-clicking the Start button. It adds shortcuts to system settings and tools
and lets you choose which commands appear.

## Features

- **Settings.** Open Windows Settings categories or go directly to pages such
  as Display, Sound, VPN, Default apps, and Windows Update.
- **Control Panel.** Open classic categories and tools such as Device Manager,
  Network Connections, Power Options, and Programs and Features.
- **Administration.** Open Computer Management, Services, Event Viewer, Registry
  Editor, and Terminal. Related shortcuts include Task Scheduler, certificates,
  Windows PowerShell, and Command Prompt.
- **Basic commands.** Open Run, Task Manager, Search, and File Explorer, show
  the desktop, or access power and sign-in options. File Explorer opens its
  configured start page. Its submenu can include This PC, Downloads, the Recycle
  Bin, and other folder shortcuts.
- **Windows labels and icons.** Menu labels follow the Windows display language.
  Menu items use Segoe Fluent Icons, including the matching Settings icons
  where available. Windhawk settings are available in English, Russian, Spanish,
  German, Brazilian Portuguese, and Simplified Chinese.

## Platforms

Requires [Windhawk](https://windhawk.net/) and Windows 11 x64. Tested on
Windows 11 24H2 Home and Pro (build `26100.4349`) with Windhawk 1.7.3. An
earlier version was also tested on Windows 11 26H2 (build `26300.10075`). The
mod is incompatible with Windows 10 Win+X Restorer.

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
or press **→** to open related shortcuts. A row with only a submenu opens it
when you click the label. A row with only a main action has no arrow.

Open the mod's **Settings** in Windhawk to change the menu.

- **Menu contents.** The switches follow the menu order. The switch under each
  section description hides the whole section and preserves its settings. Each
  row has a switch below its label, followed by the switches for its submenu
  items. Turning off a row hides it with its submenu. Search, Desktop, and
  the Documents and Pictures shortcuts under File Explorer are off by default.
- **General settings.** Submenus open immediately on hover by default. They
  can also open after a delay, on arrow hover, or on arrow click. **Command
  indication** uses a hand cursor over areas that run a command and is on by
  default. **Hide administrator commands** is off by default. When enabled,
  it hides known administrator commands that require credentials or whose
  elevation is blocked. Commands that only need UAC consent stay visible.
- **Command settings.** The main Terminal action can run as administrator.
  The main power action defaults to **Shut down**. Select another action or
  choose **None** to keep the power submenu with no main action.

Command availability can depend on the Windows edition, installed components,
and hardware. When a main action is unavailable, its available shortcuts stay
in the submenu. If no commands remain, Windows opens its standard Win+X menu.

### Verification

Run both commands from the repository root with Node.js.

```powershell
node Quick_Link_Menu/Tools/package-mod.mjs --check
node --test Quick_Link_Menu/Tests/menu-settings.test.mjs
```

The first command checks that the standalone source matches its inputs. The
tests cover menu settings, defaults, and localization.

[DSN Windhawk Mods](../README.md)
