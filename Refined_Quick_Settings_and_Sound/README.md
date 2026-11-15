# Refined Quick Settings and Sound

[Русский](README.ru.md)

This "vanilla+" Windhawk mod reworks the Windows 11 "Quick Settings" and "Volume mixer" panels and makes them fit for daily use. It follows the Windows 11 design language with thin sliders, even row spacing and a smooth frame transition to the Sound page.

Everything runs inside the shell's own panels (ShellHost). The system Acrylic, fonts, themes, lists, bindings and animations are kept, and the original layout is restored when the mod is unloaded.

This edition is based on [HeyOkay's original mod](https://github.com/HeyOkay/Refined-Quick-Settings-and-Sound).

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="Assets/Previews/Dark_Preview.webp">
  <source media="(prefers-color-scheme: light)" srcset="Assets/Previews/Light_Preview.webp">
  <img alt="Quick Settings and Sound panels with the volume mixer in use" src="Assets/Previews/Dark_Preview.gif">
</picture>

## Features

**Quick Settings**

- Choose up to five **Additional shortcuts** to the right of **Settings**, including Notifications, Graphics, Advanced network settings, Power and Windows Update. Empty slots and duplicate shortcuts are skipped. The Display shortcut is also skipped when the brightness row already has a Display button.
- The DDC/CI brightness row has a shortcut to Display settings and a Night light button. The latter uses the native quick action when available and opens Night light settings otherwise.
- **Display brightness** defaults to **Auto**. It keeps the Windows slider when available or adds one for a monitor with a usable DDC/CI range. **Disable** hides the brightness row.
- Set **Rows of tiles** to 1–4. Pages follow the number of available tiles. The override requires the ControlCenter build from Windows 26300.9974.

**Sound**

- The "Sound output" page is now simply "Sound".
- Change the output and input devices and the spatial sound provider from dropdown lists.
- Below them is a reworked Volume mixer for the apps. It starts collapsed the first time Sound opens in each ShellHost process and remembers its state on subsequent openings.
- Volume sliders show the new value when dragged or scrolled. The default wheel step is 2 percentage points.
- Press an app's icon to mute and restore it, or its volume number to set it to 100.
- **Hide from the volume mixer** takes app names or executable names, one per row. Hidden apps keep playing audio.

**Taskbar**

- **Quick access to Sound** opens Sound with a click on the taskbar volume icon.
- **Middle-click to mute** toggles sound from the volume icon and preserves the volume level. Off by default.
- **More sound settings** in the volume icon menu opens the classic Sound Control Panel.
- **Device Manager** is available in the network and battery icon menus.
- **Alternative icons** includes the animated volume and wired network icons from the original mod. Each location has its own switch. Switching one off uses the Windows icon there.

## Platforms

Windows 11 24H2 or later with ShellHost Quick Settings, x86-64. The original mod was developed on build **26300**, primarily at **125%** display scale. Shell updates can change the XAML elements and symbols the mod uses. DDC/CI brightness requires a compatible monitor with DDC/CI enabled.

Added labels are available in English, Russian, Spanish, German, Brazilian Portuguese and Simplified Chinese.

## Operations

### Installation

1. Install [Windhawk](https://windhawk.net/) and open [the standalone source](Mod/refined-quick-settings-and-sound.wh.cpp).
2. Disable the original Refined Quick Settings and Sound and any mods or styles that change the same controls.
3. In Windhawk, choose **Create a New Mod** and replace the contents of `mod.wh.cpp` with the standalone source. Select **Compile Mod**, exit the editor and check that the mod is enabled.
4. If the panels do not update, close and reopen them. If needed, restart ShellHost and check the Windhawk log.

### Usage

Open Quick Settings with Win+A and Sound with Win+Ctrl+V. When **Quick access to Sound** is enabled, the taskbar volume icon opens Sound as well. Adjust the mod's options on its **Settings** tab in Windhawk.

### Build and verification

Run from this mod directory with Python 3 on `PATH` and Windhawk installed. `Build-Mod.ps1` uses `C:/Program Files/Windhawk/Compiler` and Engine 1.7.3.

```powershell
.\Tools\Build-Mod.ps1
.\Tests\Test-Model.ps1
```

`Build-Mod.ps1` first runs `package_mod.py`, which assembles the standalone source in `Mod/` from `Mod/Source/`. The DLL goes to `Builds/Refined_Quick_Settings_and_Sound/` at the repository root. `Test-Model.ps1` checks the mod's model logic. Use `Tools/Measure-PanelOpen.ps1` to measure panel opening time.

See the [preview recording guide](Preview.ru.md) to reproduce the demo.

## Licenses

The mod is [GPL-3.0-only](LICENSE). The UI-thread dispatch technique follows m417z's Notification Center Styler (GPL-3.0). The hidden-apps parser is adapted from Sound Output Direct and Taller (GPL-3.0-only). The `sliders-horizontal` and `ethernet-port` icons are from [Lucide](Mod/LUCIDE-LICENSE.txt) (ISC). See the [third-party notices](THIRD-PARTY-NOTICES.md) for details.
