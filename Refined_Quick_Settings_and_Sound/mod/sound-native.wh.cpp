// ==WindhawkMod==
// @id refined-quick-settings-and-sound
// @name Refined Quick Settings and Sound
// @description Refines Windows 11 Quick Settings and Sound, with configurable brightness, tile rows and shortcuts
// @description:ru-RU Настройка быстрых настроек и звука Windows 11, включая яркость, число строк и кнопки
// @description:es-ES Personaliza la configuración rápida y el sonido de Windows 11, con brillo, filas y accesos directos configurables
// @description:de-DE Passt die Schnelleinstellungen und den Sound von Windows 11 an, einschließlich Helligkeit, Kachelzeilen und Verknüpfungen
// @description:pt-BR Personaliza as configurações rápidas e de som do Windows 11, incluindo brilho, linhas e atalhos
// @description:zh-CN 自定义 Windows 11 快速设置和声音，包括亮度、磁贴行数和快捷按钮
// @version 0.4.0
// @author HeyOkay, CTravkin
// @github https://github.com/HeyOkay
// @homepage https://github.com/HeyOkay/Refined-Quick-Settings-and-Sound
// @include explorer.exe
// @include ShellHost.exe
// @architecture x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lcomctl32 -lole32 -loleaut32 -lruntimeobject -liphlpapi -lshlwapi -lshell32 -lpowrprof -luuid
// @license GPL-3.0-only
// ==/WindhawkMod==
// Build: 159
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
- Set the volume change per wheel notch, default 2 percentage points. Press an app's icon to mute and restore it, or
  its volume number to set it to 100.
- Hide apps from the mixer by name or executable, one app per row.

**Taskbar**

- Enable Quick access to Sound to open Sound with a click on the taskbar volume icon.
- Choose where the alternative mixer, speaker, Ethernet and settings icons appear.

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
  $description: "Opens the Sound panel from the taskbar volume icon"
  $description:ru-RU: "Панель «Звук» открывается по нажатию на значок громкости"
  $description:es-ES: "Abre el panel Sonido desde el icono de volumen de la barra de tareas"
  $description:de-DE: "Öffnet das Soundfenster über das Lautstärkesymbol in der Taskleiste"
  $description:pt-BR: "Abre o painel Som pelo ícone de volume da barra de tarefas"
  $description:zh-CN: "单击任务栏上的音量图标可直接打开“声音”面板"
- volumeWheelStep: 2
  $name: Volume wheel step
  $name:ru-RU: Шаг громкости
  $name:es-ES: Paso de volumen con la rueda del ratón
  $name:de-DE: Lautstärkeschritt mit dem Mausrad
  $name:pt-BR: Passo do volume com a roda do mouse
  $name:zh-CN: 鼠标滚轮音量步长
  $description: Percentage points per notch over a volume slider, from 1 to 100
  $description:ru-RU: На сколько процентных пунктов меняется громкость за одно деление колеса, от 1 до 100
  $description:es-ES: Puntos porcentuales por paso de rueda sobre un control de volumen, de 1 a 100
  $description:de-DE: Prozentpunkte pro Mausradschritt über einem Lautstärkeregler, von 1 bis 100
  $description:pt-BR: Pontos percentuais por passo da roda sobre um controle de volume, de 1 a 100
  $description:zh-CN: 在音量滑块上滚动一格改变的百分点，范围为 1 到 100
- brightnessMode: auto
  $name: Display brightness
  $name:ru-RU: Яркость дисплея
  $name:es-ES: Brillo de la pantalla
  $name:de-DE: Anzeigehelligkeit
  $name:pt-BR: Brilho da tela
  $name:zh-CN: 显示亮度
  $description: In Auto mode, the slider appears when brightness can be adjusted
  $description:ru-RU: В режиме «Авто» ползунок виден, если яркость можно регулировать
  $description:es-ES: En modo automático, el control se muestra si el brillo se puede ajustar
  $description:de-DE: Im Modus Automatisch erscheint der Regler, wenn die Helligkeit einstellbar ist
  $description:pt-BR: No modo automático, o controle aparece quando é possível ajustar o brilho
  $description:zh-CN: 自动模式下，可调节亮度时显示滑块
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
  $name: Rows of tiles
  $name:ru-RU: Строки плиток
  $name:es-ES: Filas de iconos de configuración rápida
  $name:de-DE: Kachelzeilen in den Schnelleinstellungen
  $name:pt-BR: Linhas de blocos nas configurações rápidas
  $name:zh-CN: 快速设置磁贴行数
  $description: How many rows fit on each Quick Settings page
  $description:ru-RU: Сколько строк помещается на одной странице быстрых настроек
  $description:es-ES: Número de filas por página de Configuración rápida
  $description:de-DE: Anzahl der Kachelzeilen pro Seite der Schnelleinstellungen
  $description:pt-BR: Número de linhas por página das Configurações rápidas
  $description:zh-CN: 每页快速设置显示的磁贴行数
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
  $description: Shortcuts to the right of Settings, in the selected order
  $description:ru-RU: Порядок кнопок справа от «Настроек», пустые позиции и повторы пропускаются
  $description:es-ES: Accesos a la derecha de Configuración, en el orden seleccionado
  $description:de-DE: Verknüpfungen rechts neben Einstellungen in der gewählten Reihenfolge
  $description:pt-BR: Atalhos à direita de Configurações, na ordem selecionada
  $description:zh-CN: 按所选顺序显示在“设置”右侧的快捷方式
- alternativeIcons:
  - mixerPlaces: saved
    $name: "Mixer icon"
    $name:ru-RU: "Микшер"
    $name:es-ES: "Icono del mezclador"
    $name:de-DE: "Mixer-Symbol"
    $name:pt-BR: "Ícone do mixer"
    $name:zh-CN: "合成器图标"
    $options:
    - saved: "As before"
    - "0": "Windows icons everywhere"
    - "1": "Quick Settings"
    - "2": "Sound panel"
    - "3": "Everywhere"
    $options:ru-RU:
    - saved: "Как раньше"
    - "0": "Везде иконки Windows"
    - "1": "Быстрые настройки"
    - "2": "Панель «Звук»"
    - "3": "Везде"
    $options:es-ES:
    - saved: "Como antes"
    - "0": "Iconos de Windows en todas partes"
    - "1": "Configuración rápida"
    - "2": "Panel Sonido"
    - "3": "En todas partes"
    $options:de-DE:
    - saved: "Wie zuvor"
    - "0": "Überall Windows-Symbole"
    - "1": "Schnelleinstellungen"
    - "2": "Soundfenster"
    - "3": "Überall"
    $options:pt-BR:
    - saved: "Como antes"
    - "0": "Ícones do Windows em todos os locais"
    - "1": "Configurações rápidas"
    - "2": "Painel Som"
    - "3": "Em todos os locais"
    $options:zh-CN:
    - saved: "沿用原设置"
    - "0": "所有位置使用 Windows 图标"
    - "1": "快速设置"
    - "2": "“声音”面板"
    - "3": "所有位置"
  - volumePlaces: saved
    $name: "Speaker icon"
    $name:ru-RU: "Динамик"
    $name:es-ES: "Icono del altavoz"
    $name:de-DE: "Lautsprechersymbol"
    $name:pt-BR: "Ícone de alto-falante"
    $name:zh-CN: "扬声器图标"
    $options:
    - saved: "As before"
    - "0": "Windows icons everywhere"
    - "1": "Taskbar"
    - "2": "Quick Settings"
    - "4": "Sound panel"
    - "8": "Volume mixer"
    - "3": "Taskbar + Quick Settings"
    - "5": "Taskbar + Sound panel"
    - "6": "Quick Settings + Sound panel"
    - "9": "Taskbar + Volume mixer"
    - "a": "Quick Settings + Volume mixer"
    - "c": "Sound panel + Volume mixer"
    - "7": "Taskbar + Quick Settings + Sound panel"
    - "b": "Taskbar + Quick Settings + Volume mixer"
    - "d": "Taskbar + Sound panel + Volume mixer"
    - "e": "Quick Settings + Sound panel + Volume mixer"
    - "f": "Everywhere"
    $options:ru-RU:
    - saved: "Как раньше"
    - "0": "Везде иконки Windows"
    - "1": "Панель задач"
    - "2": "Быстрые настройки"
    - "4": "Панель «Звук»"
    - "8": "Микшер громкости"
    - "3": "Панель задач + Быстрые настройки"
    - "5": "Панель задач + Панель «Звук»"
    - "6": "Быстрые настройки + Панель «Звук»"
    - "9": "Панель задач + Микшер громкости"
    - "a": "Быстрые настройки + Микшер громкости"
    - "c": "Панель «Звук» + Микшер громкости"
    - "7": "Панель задач + Быстрые настройки + Панель «Звук»"
    - "b": "Панель задач + Быстрые настройки + Микшер громкости"
    - "d": "Панель задач + Панель «Звук» + Микшер громкости"
    - "e": "Быстрые настройки + Панель «Звук» + Микшер громкости"
    - "f": "Везде"
    $options:es-ES:
    - saved: "Como antes"
    - "0": "Iconos de Windows en todas partes"
    - "1": "Barra de tareas"
    - "2": "Configuración rápida"
    - "4": "Panel Sonido"
    - "8": "Mezclador de volumen"
    - "3": "Barra de tareas + Configuración rápida"
    - "5": "Barra de tareas + Panel Sonido"
    - "6": "Configuración rápida + Panel Sonido"
    - "9": "Barra de tareas + Mezclador de volumen"
    - "a": "Configuración rápida + Mezclador de volumen"
    - "c": "Panel Sonido + Mezclador de volumen"
    - "7": "Barra de tareas + Configuración rápida + Panel Sonido"
    - "b": "Barra de tareas + Configuración rápida + Mezclador de volumen"
    - "d": "Barra de tareas + Panel Sonido + Mezclador de volumen"
    - "e": "Configuración rápida + Panel Sonido + Mezclador de volumen"
    - "f": "En todas partes"
    $options:de-DE:
    - saved: "Wie zuvor"
    - "0": "Überall Windows-Symbole"
    - "1": "Taskleiste"
    - "2": "Schnelleinstellungen"
    - "4": "Soundfenster"
    - "8": "Lautstärkemixer"
    - "3": "Taskleiste + Schnelleinstellungen"
    - "5": "Taskleiste + Soundfenster"
    - "6": "Schnelleinstellungen + Soundfenster"
    - "9": "Taskleiste + Lautstärkemixer"
    - "a": "Schnelleinstellungen + Lautstärkemixer"
    - "c": "Soundfenster + Lautstärkemixer"
    - "7": "Taskleiste + Schnelleinstellungen + Soundfenster"
    - "b": "Taskleiste + Schnelleinstellungen + Lautstärkemixer"
    - "d": "Taskleiste + Soundfenster + Lautstärkemixer"
    - "e": "Schnelleinstellungen + Soundfenster + Lautstärkemixer"
    - "f": "Überall"
    $options:pt-BR:
    - saved: "Como antes"
    - "0": "Ícones do Windows em todos os locais"
    - "1": "Barra de tarefas"
    - "2": "Configurações rápidas"
    - "4": "Painel Som"
    - "8": "Mixer de volume"
    - "3": "Barra de tarefas + Configurações rápidas"
    - "5": "Barra de tarefas + Painel Som"
    - "6": "Configurações rápidas + Painel Som"
    - "9": "Barra de tarefas + Mixer de volume"
    - "a": "Configurações rápidas + Mixer de volume"
    - "c": "Painel Som + Mixer de volume"
    - "7": "Barra de tarefas + Configurações rápidas + Painel Som"
    - "b": "Barra de tarefas + Configurações rápidas + Mixer de volume"
    - "d": "Barra de tarefas + Painel Som + Mixer de volume"
    - "e": "Configurações rápidas + Painel Som + Mixer de volume"
    - "f": "Em todos os locais"
    $options:zh-CN:
    - saved: "沿用原设置"
    - "0": "所有位置使用 Windows 图标"
    - "1": "任务栏"
    - "2": "快速设置"
    - "4": "“声音”面板"
    - "8": "音量合成器"
    - "3": "任务栏、快速设置"
    - "5": "任务栏、“声音”面板"
    - "6": "快速设置、“声音”面板"
    - "9": "任务栏、音量合成器"
    - "a": "快速设置、音量合成器"
    - "c": "“声音”面板、音量合成器"
    - "7": "任务栏、快速设置、“声音”面板"
    - "b": "任务栏、快速设置、音量合成器"
    - "d": "任务栏、“声音”面板、音量合成器"
    - "e": "快速设置、“声音”面板、音量合成器"
    - "f": "所有位置"
  - ethernetPlaces: saved
    $name: "Ethernet icon"
    $name:ru-RU: "Ethernet"
    $name:es-ES: "Icono de Ethernet"
    $name:de-DE: "Ethernet-Symbol"
    $name:pt-BR: "Ícone de Ethernet"
    $name:zh-CN: "以太网图标"
    $options:
    - saved: "As before"
    - "0": "Windows icons everywhere"
    - "1": "Alternative icon"
    $options:ru-RU:
    - saved: "Как раньше"
    - "0": "Везде иконки Windows"
    - "1": "Альтернативная иконка"
    $options:es-ES:
    - saved: "Como antes"
    - "0": "Iconos de Windows en todas partes"
    - "1": "Icono alternativo"
    $options:de-DE:
    - saved: "Wie zuvor"
    - "0": "Überall Windows-Symbole"
    - "1": "Alternatives Symbol"
    $options:pt-BR:
    - saved: "Como antes"
    - "0": "Ícones do Windows em todos os locais"
    - "1": "Ícone alternativo"
    $options:zh-CN:
    - saved: "沿用原设置"
    - "0": "所有位置使用 Windows 图标"
    - "1": "备用图标"
  - gearPlaces: "1"
    $name: "Settings gear"
    $name:ru-RU: "Шестеренка"
    $name:es-ES: "Engranaje de configuración"
    $name:de-DE: "Einstellungszahnrad"
    $name:pt-BR: "Engrenagem de configurações"
    $name:zh-CN: "设置齿轮图标"
    $options:
    - "0": "Windows icons everywhere"
    - "1": "Sound panel"
    $options:ru-RU:
    - "0": "Везде иконки Windows"
    - "1": "Панель «Звук»"
    $options:es-ES:
    - "0": "Iconos de Windows en todas partes"
    - "1": "Panel Sonido"
    $options:de-DE:
    - "0": "Überall Windows-Symbole"
    - "1": "Soundfenster"
    $options:pt-BR:
    - "0": "Ícones do Windows em todos os locais"
    - "1": "Painel Som"
    $options:zh-CN:
    - "0": "所有位置使用 Windows 图标"
    - "1": "“声音”面板"
  $name: "Alternative icons"
  $name:ru-RU: "Альтернативные иконки"
  $name:es-ES: "Iconos alternativos"
  $name:de-DE: "Alternative Symbole"
  $name:pt-BR: "Ícones alternativos"
  $name:zh-CN: "备用图标"
  $description: "Choose where each alternative icon is used"
  $description:ru-RU: "Выбор мест, где отображаются альтернативные иконки"
  $description:es-ES: "Elige dónde se usa cada icono alternativo"
  $description:de-DE: "Legt fest, wo die alternativen Symbole erscheinen"
  $description:pt-BR: "Escolhe onde cada ícone alternativo aparece"
  $description:zh-CN: "选择各备用图标的显示位置"
- hiddenApplications: [""]
  $name: Hidden apps
  $name:ru-RU: Скрытые приложения
  $name:es-ES: Aplicaciones ocultas
  $name:de-DE: Ausgeblendete Apps
  $name:pt-BR: Aplicativos ocultos
  $name:zh-CN: 隐藏的应用
  $description: One app per row, by name or executable, for example Steam or steam.exe
  $description:ru-RU: По одному приложению на строку, например Steam или steam.exe
  $description:es-ES: Una aplicación por fila, por ejemplo Steam o steam.exe
  $description:de-DE: Eine App pro Zeile, zum Beispiel Steam oder steam.exe
  $description:pt-BR: Um aplicativo por linha, por exemplo Steam ou steam.exe
  $description:zh-CN: 每行一个应用，例如 Steam 或 steam.exe
*/
// ==/WindhawkModSettings==
#include "host.h"
#include "tray.h"
namespace native {
inline bool explorer = false;
inline void ReadHiddenApps() {
  hidden.clear();
  // Windhawk exposes indexed values but no array length. Allow empty rows
  // between names, including rows left blank in the settings editor.
  for (int i = 0, blanks = 0; blanks < 32; ++i) {
    auto name = WindhawkUtils::StringSetting::make(L"hiddenApplications[%d]", i);
    if (!*name.get()) {
      ++blanks;
      continue;
    }
    blanks = 0;
    sound::AddHiddenApp(hidden, name.get());
  }
  if (!hidden.empty()) {
    if (!Wh_GetIntValue(L"usesHiddenAppRows", 0))
      Wh_SetIntValue(L"usesHiddenAppRows", 1);
  } else if (!Wh_GetIntValue(L"usesHiddenAppRows", 0)) {
    auto legacy = WindhawkUtils::StringSetting::make(L"hiddenApps");
    hidden = sound::ParseHidden(legacy.get());
  }
}
inline void Settings() {
  ReadHiddenApps();
  volumeWheelStep = sound::VolumeWheelStep(Wh_GetIntSetting(L"volumeWheelStep"));
  auto brightness = WindhawkUtils::StringSetting::make(L"brightnessMode");
  brightnessEnabled = std::wstring_view(brightness.get()) != L"disabled";
  auto rows = WindhawkUtils::StringSetting::make(L"quickActionRows");
  std::wstring_view rowCount(rows.get());
  quickActionRows = rowCount.size() == 1 && rowCount.front() >= L'1' && rowCount.front() <= L'4'
                        ? rowCount.front() - L'0' : 2;
  using S = sound::QuickShortcut;
  auto first = WindhawkUtils::StringSetting::make(L"quickSettings.shortcut1");
  const auto oldIcon = [&](PCWSTR key) { return !*first.get() || Wh_GetIntSetting(key) != 0; };
  auto mixer = WindhawkUtils::StringSetting::make(L"alternativeIcons.mixerPlaces");
  auto volume = WindhawkUtils::StringSetting::make(L"alternativeIcons.volumePlaces");
  auto ethernet = WindhawkUtils::StringSetting::make(L"alternativeIcons.ethernetPlaces");
  auto gear = WindhawkUtils::StringSetting::make(L"alternativeIcons.gearPlaces");
  unsigned mixerPlaces = sound::IconPlaces(mixer.get(), 3,
      oldIcon(L"alternativeIcons.quickMixer") | (oldIcon(L"alternativeIcons.soundMixer") << 1));
  unsigned volumePlaces = sound::IconPlaces(volume.get(), 15,
      oldIcon(L"alternativeIcons.trayVolume") | (oldIcon(L"alternativeIcons.quickVolume") << 1) |
      (oldIcon(L"alternativeIcons.soundVolume") << 2) | (oldIcon(L"alternativeIcons.appUnmute") << 3));
  altQuickMixer = (mixerPlaces & 1) != 0;
  altSoundMixer = (mixerPlaces & 2) != 0;
  altTrayVolume = (volumePlaces & 1) != 0;
  altQuickVolume = (volumePlaces & 2) != 0;
  altSoundVolume = (volumePlaces & 4) != 0;
  altAppUnmute = (volumePlaces & 8) != 0;
  altTrayEthernet = sound::IconPlaces(ethernet.get(), 1, oldIcon(L"alternativeIcons.trayEthernet")) != 0;
  altSoundGear = sound::IconPlaces(gear.get(), 1, 1) != 0;
  if (*first.get()) {
    for (int i = 0; i < int(quickShortcuts.size()); ++i) {
      auto slot = WindhawkUtils::StringSetting::make(L"quickSettings.shortcut%d", i + 1);
      quickShortcuts[i] = sound::ParseQuickShortcut(slot.get());
    }
    quickSoundAccess = Wh_GetIntSetting(L"quickSoundAccess") != 0;
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
