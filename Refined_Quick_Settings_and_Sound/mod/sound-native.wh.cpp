// ==WindhawkMod==
// @id refined-quick-settings-and-sound
// @name Refined Quick Settings and Sound
// @description Refines Windows 11 Quick Settings and Sound, with configurable brightness, tile rows and shortcuts
// @description:ru-RU Настройка быстрых настроек и звука Windows 11, включая яркость, число строк и кнопки
// @description:es-ES Personaliza la configuración rápida y el sonido de Windows 11, con brillo, filas y accesos directos configurables
// @description:de-DE Passt die Schnelleinstellungen und den Sound von Windows 11 an, einschließlich Helligkeit, Kachelzeilen und Verknüpfungen
// @description:pt-BR Personaliza as configurações rápidas e de som do Windows 11, incluindo brilho, linhas e atalhos
// @description:zh-CN 自定义 Windows 11 快速设置和声音，包括亮度、磁贴行数和快捷按钮
// @version 0.3.0
// @author HeyOkay, CTravkin
// @github https://github.com/HeyOkay
// @homepage https://github.com/HeyOkay/Refined-Quick-Settings-and-Sound
// @include explorer.exe
// @include ShellHost.exe
// @architecture x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lcomctl32 -lole32 -loleaut32 -lruntimeobject -liphlpapi -lshlwapi -lshell32 -lpowrprof -luuid
// @license GPL-3.0-only
// ==/WindhawkMod==
// Build: 158
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
- The Settings row opens Windows Settings and can show up to five ordered shortcuts. Empty slots, repeat choices and actions already visible in the brightness controls are skipped.
- Quick Settings can show one to four rows of tiles. Pages follow the number of available tiles. The row override is currently supported on Windows build 26300.9974.

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

- Enable Quick access to Sound to open Sound with a click on the taskbar volume icon.
- Alternative mixer, volume and Ethernet icons can be enabled individually.

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
- quickSoundAccess: true
  $name: "Quick access to Sound"
  $name:ru-RU: "Быстрый доступ к настройкам звука"
  $name:es-ES: "Acceso rápido al sonido"
  $name:de-DE: "Schnellzugriff auf Sound"
  $name:pt-BR: "Acesso rápido ao som"
  $name:zh-CN: "快速访问声音设置"
  $description: "Click the taskbar volume icon to open Sound directly."
  $description:ru-RU: "Нажатие на значок громкости на панели задач сразу открывает панель «Звук»"
  $description:es-ES: "Abre directamente Sonido al pulsar el icono de volumen de la barra de tareas."
  $description:de-DE: "Ein Klick auf das Lautstärkesymbol in der Taskleiste öffnet direkt das Soundfenster."
  $description:pt-BR: "Clicar no ícone de volume na barra de tarefas abre diretamente o painel Som."
  $description:zh-CN: "单击任务栏上的音量图标可直接打开“声音”面板"
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
- quickActionRows: "2"
  $name: Quick Settings tile rows
  $name:ru-RU: Строки плиток быстрых настроек
  $name:es-ES: Filas de iconos de configuración rápida
  $name:de-DE: Kachelzeilen in den Schnelleinstellungen
  $name:pt-BR: Linhas de blocos nas configurações rápidas
  $name:zh-CN: 快速设置磁贴行数
  $options:
  - "1": "1"
  - "2": "2"
  - "3": "3"
  - "4": "4"
- quickSettings:
  - shortcut1: display
    $name: "Shortcut 1"
    $name:ru-RU: "Переход 1"
    $name:es-ES: "Acceso directo 1"
    $name:de-DE: "Verknüpfung 1"
    $name:pt-BR: "Atalho 1"
    $name:zh-CN: "快捷方式 1"
    $options:
    - none: "None"
    - display: "Display settings"
    - night: "Night light"
    - network: "Network & internet"
    - bluetooth: "Bluetooth & devices"
    - apps: "Installed apps"
    $options:ru-RU:
    - none: "Нет"
    - display: "Параметры дисплея"
    - night: "Ночной свет"
    - network: "Сеть и Интернет"
    - bluetooth: "Bluetooth и устройства"
    - apps: "Установленные приложения"
    $options:es-ES:
    - none: "Ninguno"
    - display: "Configuración de pantalla"
    - night: "Luz nocturna"
    - network: "Red e Internet"
    - bluetooth: "Bluetooth y dispositivos"
    - apps: "Aplicaciones instaladas"
    $options:de-DE:
    - none: "Keine"
    - display: "Anzeigeeinstellungen"
    - night: "Nachtmodus"
    - network: "Netzwerk und Internet"
    - bluetooth: "Bluetooth und Geräte"
    - apps: "Installierte Apps"
    $options:pt-BR:
    - none: "Nenhum"
    - display: "Configurações de exibição"
    - night: "Luz noturna"
    - network: "Rede e Internet"
    - bluetooth: "Bluetooth e dispositivos"
    - apps: "Aplicativos instalados"
    $options:zh-CN:
    - none: "无"
    - display: "显示设置"
    - night: "夜间模式"
    - network: "网络和 Internet"
    - bluetooth: "蓝牙和其他设备"
    - apps: "已安装的应用"
  - shortcut2: night
    $name: "Shortcut 2"
    $name:ru-RU: "Переход 2"
    $name:es-ES: "Acceso directo 2"
    $name:de-DE: "Verknüpfung 2"
    $name:pt-BR: "Atalho 2"
    $name:zh-CN: "快捷方式 2"
    $options:
    - none: "None"
    - display: "Display settings"
    - night: "Night light"
    - network: "Network & internet"
    - bluetooth: "Bluetooth & devices"
    - apps: "Installed apps"
    $options:ru-RU:
    - none: "Нет"
    - display: "Параметры дисплея"
    - night: "Ночной свет"
    - network: "Сеть и Интернет"
    - bluetooth: "Bluetooth и устройства"
    - apps: "Установленные приложения"
    $options:es-ES:
    - none: "Ninguno"
    - display: "Configuración de pantalla"
    - night: "Luz nocturna"
    - network: "Red e Internet"
    - bluetooth: "Bluetooth y dispositivos"
    - apps: "Aplicaciones instaladas"
    $options:de-DE:
    - none: "Keine"
    - display: "Anzeigeeinstellungen"
    - night: "Nachtmodus"
    - network: "Netzwerk und Internet"
    - bluetooth: "Bluetooth und Geräte"
    - apps: "Installierte Apps"
    $options:pt-BR:
    - none: "Nenhum"
    - display: "Configurações de exibição"
    - night: "Luz noturna"
    - network: "Rede e Internet"
    - bluetooth: "Bluetooth e dispositivos"
    - apps: "Aplicativos instalados"
    $options:zh-CN:
    - none: "无"
    - display: "显示设置"
    - night: "夜间模式"
    - network: "网络和 Internet"
    - bluetooth: "蓝牙和其他设备"
    - apps: "已安装的应用"
  - shortcut3: none
    $name: "Shortcut 3"
    $name:ru-RU: "Переход 3"
    $name:es-ES: "Acceso directo 3"
    $name:de-DE: "Verknüpfung 3"
    $name:pt-BR: "Atalho 3"
    $name:zh-CN: "快捷方式 3"
    $options:
    - none: "None"
    - display: "Display settings"
    - night: "Night light"
    - network: "Network & internet"
    - bluetooth: "Bluetooth & devices"
    - apps: "Installed apps"
    $options:ru-RU:
    - none: "Нет"
    - display: "Параметры дисплея"
    - night: "Ночной свет"
    - network: "Сеть и Интернет"
    - bluetooth: "Bluetooth и устройства"
    - apps: "Установленные приложения"
    $options:es-ES:
    - none: "Ninguno"
    - display: "Configuración de pantalla"
    - night: "Luz nocturna"
    - network: "Red e Internet"
    - bluetooth: "Bluetooth y dispositivos"
    - apps: "Aplicaciones instaladas"
    $options:de-DE:
    - none: "Keine"
    - display: "Anzeigeeinstellungen"
    - night: "Nachtmodus"
    - network: "Netzwerk und Internet"
    - bluetooth: "Bluetooth und Geräte"
    - apps: "Installierte Apps"
    $options:pt-BR:
    - none: "Nenhum"
    - display: "Configurações de exibição"
    - night: "Luz noturna"
    - network: "Rede e Internet"
    - bluetooth: "Bluetooth e dispositivos"
    - apps: "Aplicativos instalados"
    $options:zh-CN:
    - none: "无"
    - display: "显示设置"
    - night: "夜间模式"
    - network: "网络和 Internet"
    - bluetooth: "蓝牙和其他设备"
    - apps: "已安装的应用"
  - shortcut4: none
    $name: "Shortcut 4"
    $name:ru-RU: "Переход 4"
    $name:es-ES: "Acceso directo 4"
    $name:de-DE: "Verknüpfung 4"
    $name:pt-BR: "Atalho 4"
    $name:zh-CN: "快捷方式 4"
    $options:
    - none: "None"
    - display: "Display settings"
    - night: "Night light"
    - network: "Network & internet"
    - bluetooth: "Bluetooth & devices"
    - apps: "Installed apps"
    $options:ru-RU:
    - none: "Нет"
    - display: "Параметры дисплея"
    - night: "Ночной свет"
    - network: "Сеть и Интернет"
    - bluetooth: "Bluetooth и устройства"
    - apps: "Установленные приложения"
    $options:es-ES:
    - none: "Ninguno"
    - display: "Configuración de pantalla"
    - night: "Luz nocturna"
    - network: "Red e Internet"
    - bluetooth: "Bluetooth y dispositivos"
    - apps: "Aplicaciones instaladas"
    $options:de-DE:
    - none: "Keine"
    - display: "Anzeigeeinstellungen"
    - night: "Nachtmodus"
    - network: "Netzwerk und Internet"
    - bluetooth: "Bluetooth und Geräte"
    - apps: "Installierte Apps"
    $options:pt-BR:
    - none: "Nenhum"
    - display: "Configurações de exibição"
    - night: "Luz noturna"
    - network: "Rede e Internet"
    - bluetooth: "Bluetooth e dispositivos"
    - apps: "Aplicativos instalados"
    $options:zh-CN:
    - none: "无"
    - display: "显示设置"
    - night: "夜间模式"
    - network: "网络和 Internet"
    - bluetooth: "蓝牙和其他设备"
    - apps: "已安装的应用"
  - shortcut5: none
    $name: "Shortcut 5"
    $name:ru-RU: "Переход 5"
    $name:es-ES: "Acceso directo 5"
    $name:de-DE: "Verknüpfung 5"
    $name:pt-BR: "Atalho 5"
    $name:zh-CN: "快捷方式 5"
    $options:
    - none: "None"
    - display: "Display settings"
    - night: "Night light"
    - network: "Network & internet"
    - bluetooth: "Bluetooth & devices"
    - apps: "Installed apps"
    $options:ru-RU:
    - none: "Нет"
    - display: "Параметры дисплея"
    - night: "Ночной свет"
    - network: "Сеть и Интернет"
    - bluetooth: "Bluetooth и устройства"
    - apps: "Установленные приложения"
    $options:es-ES:
    - none: "Ninguno"
    - display: "Configuración de pantalla"
    - night: "Luz nocturna"
    - network: "Red e Internet"
    - bluetooth: "Bluetooth y dispositivos"
    - apps: "Aplicaciones instaladas"
    $options:de-DE:
    - none: "Keine"
    - display: "Anzeigeeinstellungen"
    - night: "Nachtmodus"
    - network: "Netzwerk und Internet"
    - bluetooth: "Bluetooth und Geräte"
    - apps: "Installierte Apps"
    $options:pt-BR:
    - none: "Nenhum"
    - display: "Configurações de exibição"
    - night: "Luz noturna"
    - network: "Rede e Internet"
    - bluetooth: "Bluetooth e dispositivos"
    - apps: "Aplicativos instalados"
    $options:zh-CN:
    - none: "无"
    - display: "显示设置"
    - night: "夜间模式"
    - network: "网络和 Internet"
    - bluetooth: "蓝牙和其他设备"
    - apps: "已安装的应用"
  $name: "Quick Settings"
  $name:ru-RU: "Быстрые настройки"
  $name:es-ES: "Configuración rápida"
  $name:de-DE: "Schnelleinstellungen"
  $name:pt-BR: "Configurações rápidas"
  $name:zh-CN: "快速设置"
- alternativeIcons:
  - quickMixer: true
    $name: "Mixer button in Quick Settings"
    $name:ru-RU: "Кнопка микшера в быстрых настройках"
    $name:es-ES: "Botón del mezclador en Configuración rápida"
    $name:de-DE: "Lautstärkemixer-Schaltfläche in den Schnelleinstellungen"
    $name:pt-BR: "Botão do mixer nas Configurações rápidas"
    $name:zh-CN: "快速设置中的音量合成器按钮"
  - soundMixer: true
    $name: "Mixer settings button in Sound"
    $name:ru-RU: "Кнопка параметров микшера в панели «Звук»"
    $name:es-ES: "Botón de configuración del mezclador en Sonido"
    $name:de-DE: "Mixer-Einstellungen im Soundfenster"
    $name:pt-BR: "Botão de configurações do mixer no painel Som"
    $name:zh-CN: "声音面板中的音量合成器设置按钮"
  - trayVolume: true
    $name: "Volume icon on the taskbar"
    $name:ru-RU: "Значок громкости на панели задач"
    $name:es-ES: "Icono de volumen en la barra de tareas"
    $name:de-DE: "Lautstärkesymbol in der Taskleiste"
    $name:pt-BR: "Ícone de volume na barra de tarefas"
    $name:zh-CN: "任务栏音量图标"
  - trayEthernet: true
    $name: "Ethernet icon on the taskbar"
    $name:ru-RU: "Значок Ethernet на панели задач"
    $name:es-ES: "Icono de Ethernet en la barra de tareas"
    $name:de-DE: "Ethernet-Symbol in der Taskleiste"
    $name:pt-BR: "Ícone de Ethernet na barra de tarefas"
    $name:zh-CN: "任务栏以太网图标"
  - quickVolume: true
    $name: "Volume icon in Quick Settings"
    $name:ru-RU: "Значок громкости в быстрых настройках"
    $name:es-ES: "Icono de volumen en Configuración rápida"
    $name:de-DE: "Lautstärkesymbol in den Schnelleinstellungen"
    $name:pt-BR: "Ícone de volume nas Configurações rápidas"
    $name:zh-CN: "快速设置中的音量图标"
  - soundVolume: true
    $name: "System volume icon in Sound"
    $name:ru-RU: "Значок общей громкости в панели «Звук»"
    $name:es-ES: "Icono de volumen del sistema en Sonido"
    $name:de-DE: "Systemlautstärkesymbol im Soundfenster"
    $name:pt-BR: "Ícone de volume do sistema no painel Som"
    $name:zh-CN: "声音面板中的系统音量图标"
  - appUnmute: true
    $name: "App unmute icon in the mixer"
    $name:ru-RU: "Значок включения звука приложения в микшере"
    $name:es-ES: "Icono para reactivar el sonido de una aplicación en el mezclador"
    $name:de-DE: "Symbol zum Aufheben der App-Stummschaltung im Mixer"
    $name:pt-BR: "Ícone para ativar o som de um aplicativo no mixer"
    $name:zh-CN: "音量合成器中的应用取消静音图标"
  $name: "Alternative icons"
  $name:ru-RU: "Альтернативные иконки"
  $name:es-ES: "Iconos alternativos"
  $name:de-DE: "Alternative Symbole"
  $name:pt-BR: "Ícones alternativos"
  $name:zh-CN: "备用图标"
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
  auto rows = WindhawkUtils::StringSetting::make(L"quickActionRows");
  std::wstring_view rowCount(rows.get());
  quickActionRows = rowCount.size() == 1 && rowCount.front() >= L'1' && rowCount.front() <= L'4'
                        ? rowCount.front() - L'0' : 2;
  using S = sound::QuickShortcut;
  auto first = WindhawkUtils::StringSetting::make(L"quickSettings.shortcut1");
  if (*first.get()) {
    for (int i = 0; i < int(quickShortcuts.size()); ++i) {
      auto slot = WindhawkUtils::StringSetting::make(L"quickSettings.shortcut%d", i + 1);
      quickShortcuts[i] = sound::ParseQuickShortcut(slot.get());
    }
    quickSoundAccess = Wh_GetIntSetting(L"quickSoundAccess") != 0;
    altQuickMixer = Wh_GetIntSetting(L"alternativeIcons.quickMixer") != 0;
    altSoundMixer = Wh_GetIntSetting(L"alternativeIcons.soundMixer") != 0;
    altTrayVolume = Wh_GetIntSetting(L"alternativeIcons.trayVolume") != 0;
    altTrayEthernet = Wh_GetIntSetting(L"alternativeIcons.trayEthernet") != 0;
    altQuickVolume = Wh_GetIntSetting(L"alternativeIcons.quickVolume") != 0;
    altSoundVolume = Wh_GetIntSetting(L"alternativeIcons.soundVolume") != 0;
    altAppUnmute = Wh_GetIntSetting(L"alternativeIcons.appUnmute") != 0;
  } else {
    quickShortcuts = {Wh_GetIntSetting(L"showDisplaySettings") ? S::Display : S::None,
                      Wh_GetIntSetting(L"showNightLight") ? S::Night : S::None};
  }
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
