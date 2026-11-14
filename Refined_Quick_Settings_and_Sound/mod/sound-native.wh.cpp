// ==WindhawkMod==
// @id dsn-refined-quick-settings-and-sound
// @name Refined Quick Settings and Sound (DSN)
// @description Refines Windows 11 Quick Settings and Sound, with configurable brightness, tile rows and shortcuts
// @description:ru-RU Настройка быстрых настроек и звука Windows 11, включая яркость, число строк и кнопки
// @description:es-ES Personaliza la configuración rápida y el sonido de Windows 11, con brillo, filas y accesos directos configurables
// @description:de-DE Passt die Schnelleinstellungen und den Sound von Windows 11 an, einschließlich Helligkeit, Kachelzeilen und Verknüpfungen
// @description:pt-BR Personaliza as configurações rápidas e de som do Windows 11, incluindo brilho, linhas e atalhos
// @description:zh-CN 自定义 Windows 11 快速设置和声音，包括亮度、磁贴行数和快捷按钮
// @version 0.2.0
// @author Constantine Travkin, based on HeyOkay's mod
// @github https://github.com/CTravkin
// @homepage https://github.com/CTravkin/DSN_Windhawk_Mods
// @include explorer.exe
// @include ShellHost.exe
// @architecture x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lcomctl32 -lole32 -loleaut32 -lruntimeobject -liphlpapi -lshlwapi -lshell32 -lpowrprof -luuid
// @license GPL-3.0-only
// ==/WindhawkMod==
// Build: 155
// Modified by Constantine Travkin on 2026-11-14.
// Based on HeyOkay's v0.1.154 (source blob 7229ac098ad0e8732905b831259d2aa99984bb31).
// Upstream: https://github.com/HeyOkay/Refined-Quick-Settings-and-Sound
// ==WindhawkModReadme==
/*
# Refined Quick Settings and Sound

This "vanilla+" Windhawk mod reworks the Windows 11 "Quick Settings" and
"Volume mixer" panels and makes them fit for daily use. It follows the
Windows 11 design language, refining existing features and adding new ones:
thin sliders, even row spacing at every display scale and a smooth frame
transition to the Sound page.

Everything runs inside the shell's own panels (ShellHost): no extra windows,
the system Acrylic, fonts, themes, lists, bindings and animations are kept, and
customizations are restored when the mod is unloaded.

## Features

**Quick Settings**

- Display brightness defaults to Auto and can be disabled. Auto keeps the Windows slider or adds one for a display with a usable DDC/CI range.
- The Settings row opens Windows Settings. It can also show Display settings and Night light when those buttons do not already appear in the brightness controls.
- Quick Settings can show one to four rows of tiles. The default is two.

**Sound**

- The "Sound output" page is now simply "Sound".
- Change the output and input devices and the spatial sound provider from
  dropdown lists.
- Below them, a reworked Volume mixer for the apps. It is collapsible and
  remembers its last state.
- Mouse wheel on every slider; press an app's icon to mute and restore it, or
  its volume number to set it to 100.
- Hide apps from the mixer by name (setting below).

**Taskbar**

- Click the volume icon to open Sound directly.
- The system's animated volume glyph and an Ethernet glyph for wired networks.

## Compatibility

- Windows 11 24H2 or later (Quick Settings hosted by ShellHost), x86-64
  (ARM64 is untested).
  Development testing: build 26300, primarily at 125% scale.
- Other Windows builds and display scales need validation. The mod depends on
  internal shell XAML elements and symbols which can change with Windows updates.
- DDC/CI brightness requires a compatible monitor with DDC/CI enabled.
- Added labels are available in English, Russian, Spanish, German, Brazilian Portuguese and Simplified Chinese.
- The first Sound opening in each ShellHost process starts with the mixer
  collapsed; subsequent openings remember its state.

Control Center discovery uses COM-interface symbol hooks in ControlCenter.dll and the public XAML visual tree; it does not open a XAML Diagnostics connection. Compatibility with other mods that edit the same controls still requires live testing. If the panel was already open when enabling the mod, close and reopen it; restart ShellHost if it is not picked up.

## Notes

Disable other mods that change the Quick Settings, the Sound page or the
taskbar volume icon before enabling this one. Errors are written to the
Windhawk log.

## Credits and license

GPL-3.0-only. The UI-thread dispatch technique follows
[m417z's Notification Center Styler](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/windows-11-notification-center-styler.wh.cpp)
(GPL-3.0). ControlCenter.dll discovery uses the COM produce-interface pattern
documented by [Per-monitor brightness](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/per-monitor-brightness.wh.cpp).
The hidden-applications parser is adapted from the local
Sound Output Direct and Taller mod (GPL-3.0-only).
The sliders-horizontal and ethernet-port icons are from
[Lucide](https://lucide.dev/) (ISC); the license notice is included below
in this source file.
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- brightnessMode: auto
  $name: Display brightness
  $name:ru-RU: Яркость дисплея
  $name:es-ES: Brillo de la pantalla
  $name:de-DE: Anzeigehelligkeit
  $name:pt-BR: Brilho da tela
  $name:zh-CN: 显示亮度
  $description: Auto checks whether brightness can be adjusted. Disable hides the brightness row.
  $description:ru-RU: Авто проверяет доступность регулировки яркости. Отключить скрывает строку яркости
  $description:es-ES: Automático comprueba si se puede ajustar el brillo. Desactivar oculta la fila de brillo
  $description:de-DE: Automatisch prüft, ob sich die Helligkeit anpassen lässt. Deaktivieren blendet die Helligkeitszeile aus
  $description:pt-BR: Automático verifica se o brilho pode ser ajustado. Desativar oculta a linha de brilho
  $description:zh-CN: 自动检查亮度是否可调。禁用会隐藏亮度行
  $options:
  - auto: Auto
  - disabled: Disable
  $options:ru-RU:
  - auto: Авто
  - disabled: Отключить
  $options:es-ES:
  - auto: Automático
  - disabled: Desactivar
  $options:de-DE:
  - auto: Automatisch
  - disabled: Deaktivieren
  $options:pt-BR:
  - auto: Automático
  - disabled: Desativar
  $options:zh-CN:
  - auto: 自动
  - disabled: 禁用
- quickActionRows: 2
  $name: Quick Settings tile rows
  $name:ru-RU: Строки плиток быстрых настроек
  $name:es-ES: Filas de iconos de configuración rápida
  $name:de-DE: Kachelzeilen in den Schnelleinstellungen
  $name:pt-BR: Linhas de blocos nas configurações rápidas
  $name:zh-CN: 快速设置磁贴行数
  $min: 1
  $max: 4
  $format: slider
- showNightLight: true
  $name: Night light button
  $name:ru-RU: Кнопка «Ночной свет»
  $name:es-ES: Botón de luz nocturna
  $name:de-DE: Schaltfläche für den Nachtmodus
  $name:pt-BR: Botão de luz noturna
  $name:zh-CN: 夜间模式按钮
- showDisplaySettings: true
  $name: Display settings button
  $name:ru-RU: Кнопка «Параметры дисплея»
  $name:es-ES: Botón de configuración de pantalla
  $name:de-DE: Schaltfläche für die Anzeigeeinstellungen
  $name:pt-BR: Botão de configurações de exibição
  $name:zh-CN: 显示设置按钮
- hiddenApps: ""
  $name: Hidden applications
  $name:ru-RU: Скрытые приложения
  $name:es-ES: Aplicaciones ocultas
  $name:de-DE: Ausgeblendete Apps
  $name:pt-BR: Aplicativos ocultos
  $name:zh-CN: 隐藏的应用
  $description: Parts of application names separated by semicolons. Hidden applications can still play sound.
  $description:ru-RU: Части названий приложений через точку с запятой. Скрытые приложения продолжают воспроизводить звук
  $description:es-ES: Partes de los nombres de las aplicaciones separadas por punto y coma. Las aplicaciones ocultas pueden seguir reproduciendo sonido
  $description:de-DE: Teile von App-Namen, getrennt durch Semikolons. Ausgeblendete Apps können weiterhin Ton wiedergeben
  $description:pt-BR: Partes dos nomes dos aplicativos separadas por ponto e vírgula. Os aplicativos ocultos ainda podem reproduzir som
  $description:zh-CN: 应用名称的一部分，用分号分隔。隐藏的应用仍可播放声音
*/
// ==/WindhawkModSettings==
#include "host.h"
#include "tray.h"
namespace native {
inline bool explorer = false;
inline void Settings() {
  auto setting = WindhawkUtils::StringSetting::make(L"hiddenApps");
  hidden = sound::ParseHidden(setting.get());
  auto brightness = WindhawkUtils::StringSetting::make(L"brightnessMode");
  brightnessEnabled = std::wstring_view(brightness.get()) != L"disabled";
  quickActionRows = Wh_GetIntSetting(L"quickActionRows");
  showNightLight = Wh_GetIntSetting(L"showNightLight") != 0;
  showDisplaySettings = Wh_GetIntSetting(L"showDisplaySettings") != 0;
}
} // namespace native
BOOL Wh_ModInit() {
  using namespace native;
  GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                         GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                     reinterpret_cast<LPCWSTR>(&Wh_ModInit), &module);
  WCHAR path[32768]{};
  GetModuleFileNameW(nullptr, path, 32768);
  explorer = !_wcsicmp(PathFindFileNameW(path), L"explorer.exe");
  stopping = false;
  Settings();
  if (explorer) {
    if (!tray::Initialize(module)) {
      tray::Shutdown();
      return FALSE;
    }
    return TRUE;
  }
  return InitializeHost();
}
void Wh_ModAfterInit() {
  if (native::explorer) {
    native::tray::AttachExisting();
  } else
    native::AfterHostInit();
}
void Wh_ModUninit() {
  native::stopping = true;
  if (native::explorer) {
    native::tray::Shutdown();
  } else
    native::StopHost();
}
BOOL Wh_ModSettingsChanged(BOOL *reload) {
  *reload = TRUE;
  return TRUE;
}
