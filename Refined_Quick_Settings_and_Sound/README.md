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
- **Display brightness** uses the Windows slider in the default **Auto** mode and falls back to DDC/CI on compatible monitors with DDC/CI enabled. **Disable** hides the brightness row.
- The mod places Night light and Display settings beside the brightness slider. If the native slider cannot be moved, Windows keeps its original row. Night light uses the native quick action when available and opens Night light settings otherwise.
- On devices with a battery, the battery button moves into the **Settings** row.

- **Rows of tiles** sets 1–4 rows per page with Windows pagination. If the mod cannot recognize the tile grid layout, Windows uses its default row count.
- **Tile shape** rounds the outer edges of Quick Settings tiles in **Pills** mode.

**Sound**

- The "Sound output" page is now simply "Sound".
- Change the output and input devices and the spatial sound provider from dropdown lists.
- Below them is a reworked Volume mixer for the apps. It starts collapsed the first time Sound opens in each ShellHost process and remembers its state on subsequent openings.
- **Microphone volume** defaults to **Off**. **Auto** adds a volume slider above system volume when the selected microphone has an adjustable level. The button on the left mutes and unmutes the microphone without changing its level.
- **Microphone settings shortcut** shows the volume percentage by default. **Device settings** replaces it with a button for the selected microphone's Windows settings.
- **Sound settings shortcut** puts the Windows Sound settings button beside system volume by default. **None (Percentage)** shows the volume percentage. **Device settings** opens the selected output device's settings. Both modes move the Sound settings button to the header.
- **Spatial sound** controls whether the spatial audio menu appears. It is on by default.
- **Sound field shape** rounds the closed output, input and spatial sound selectors in **Pills** mode.
- Volume sliders show the new value when dragged or scrolled. The default wheel step is 2 percentage points.
- Press a volume number to set that slider to 100. The app icon mutes or restores the app.
- **Hide from the volume mixer** takes app names or executable names, one per row. Hidden apps keep playing audio.

**Taskbar**

- **Quick access to Sound** opens Sound with a click on the taskbar volume icon.
- **Middle-click to mute** toggles sound from the volume icon and preserves the volume level. Off by default.
- **More sound settings** in the volume icon menu opens the classic Sound Control Panel.
- **Device Manager** is available in the network and battery icon menus.
- **Alternative icons** includes the animated volume and wired network icons from the original mod. Each location has its own switch. Switching one off uses the Windows icon there.

The interface is available in English, Russian, Spanish, German, Brazilian Portuguese and Simplified Chinese.

## Platforms

Requires Windows 11 24H2 or later (x64) and [Windhawk](https://windhawk.net/).

## Operations

### Installation

#### Windhawk catalog (recommended)

In Windhawk, open **Explore**, search for **Refined Quick Settings and Sound**, open its page, and select **Install**.

[Mod page in the Windhawk catalog](https://windhawk.net/mods/refined-quick-settings-and-sound)

#### From source (manual)

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
