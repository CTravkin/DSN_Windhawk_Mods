# Refined Quick Settings and Sound

[Русский](docs/README.ru.md)

This "vanilla+" Windhawk mod reworks the Windows 11 "Quick Settings" and "Volume mixer" panels and makes them fit for daily use. It follows the Windows 11 design language, refining existing features and adding new ones: thin sliders, even row spacing at every display scale and a smooth frame transition to the Sound page.

Everything runs inside the shell's own panels (ShellHost): no extra windows, the system Acrylic, fonts, themes, lists, bindings and animations are kept, and customizations are restored when the mod is unloaded.

This edition is based on [HeyOkay's original mod](https://github.com/HeyOkay/Refined-Quick-Settings-and-Sound).

## Features

**Quick Settings**

- Settings opens Windows Settings. The Display settings and Night light buttons can be enabled separately. A button already shown by the brightness controls is shown there only.
- Night light uses the native quick action when available. If Windows cannot expose the action, the button opens the Night light settings page.
- **Display brightness** defaults to **Auto**. It keeps the Windows slider when available or adds one for a monitor with a usable DDC/CI range. **Disable** hides the brightness row.
- Set **Quick Settings tile rows** to 1–4. The default is two.

**Sound**

- The "Sound output" page is now simply "Sound".
- Change the output and input devices and the spatial sound provider from dropdown lists.
- Below them, a reworked Volume mixer for the apps. It is collapsible and remembers its last state.
- Mouse wheel on every slider; press an app's icon to mute and restore it, or its volume number to set it to 100.
- Hide apps from the mixer by name (mod setting).

**Taskbar**

- Click the volume icon to open Sound directly.
- The system's animated volume glyph and an Ethernet glyph for wired networks.

Mod ID: `dsn-refined-quick-settings-and-sound`. The version is in `mod/sound-native.wh.cpp`.

## Compatibility

Windows 11 24H2 or later with ShellHost Quick Settings, x86-64. The original mod was developed on build **26300**, primarily at **125%** display scale. Shell updates can change the XAML elements and symbols the mod uses. Added labels are available in English, Russian, Spanish, German, Brazilian Portuguese and Simplified Chinese. DDC/CI brightness requires DDC/CI to be enabled in the monitor's settings.

The mixer starts collapsed on the first Sound opening in each ShellHost process; subsequent openings remember its state. Disable other mods or styling rules that modify the same Quick Settings, Sound or taskbar volume elements.

Control Center discovery uses COM-interface symbol hooks in ControlCenter.dll and the public XAML visual tree; it does not open a XAML Diagnostics connection. Compatibility with other mods that edit the same controls still requires live testing. If the panel was already open when enabling the mod, close and reopen it; restart ShellHost if it is not picked up.

## Installation from source

1. Install [Windhawk](https://windhawk.net/).
2. Open [the standalone source](mod/dist/dsn-refined-quick-settings-and-sound.wh.cpp).
3. Disable the original Refined Quick Settings and Sound. Create a new mod in Windhawk, replace the template with the standalone source, then compile and enable it.
4. If the panels do not update, restart ShellHost. Windhawk logs contain diagnostic errors.

No prebuilt DLL is required. Open Quick Settings with Win+A and Sound with Win+Ctrl+V or the taskbar volume icon.

The repeatable mouse demo recorder is documented in [docs/PREVIEW.ru.md](docs/PREVIEW.ru.md).

## Building

Requires Python 3 and an installed Windhawk (`build.ps1` uses `C:/Program Files/Windhawk/Compiler` and Engine 1.7.3; adjust the paths for another installation).

```powershell
cd mod
.\build.ps1
```

`build.ps1` first runs `package.py`, which assembles the standalone source in `mod/dist/` from the files in `mod/`. Run `mod/test-model.ps1` to check property access and `tools/measure-panel-open.ps1` to measure panel opening time.

## Licenses

The mod is [GPL-3.0-only](LICENSE). The UI-thread dispatch technique follows m417z's Notification Center Styler (GPL-3.0); the hidden-apps parser is adapted from the Sound Output Direct and Taller mod. Lucide icons are ISC licensed, see [third-party notices](docs/THIRD-PARTY-NOTICES.md) and `mod/LUCIDE-LICENSE.txt`.
