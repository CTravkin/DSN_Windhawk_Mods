// ==WindhawkMod==
// @id              quick-link-menu
// @name            Quick Link Menu
// @description     Customize and extend the Win+X menu (right-click the Start button)
// @description:ru-RU Настройка и расширение меню Win+X (правый клик по кнопке «Пуск»)
// @description:es-ES Personaliza y amplía el menú Win+X (clic derecho en el botón Inicio)
// @description:de-DE Das Win+X-Menü anpassen und erweitern (Rechtsklick auf die Startschaltfläche)
// @description:pt-BR Personalize e amplie o menu Win+X (clique direito no botão Iniciar)
// @description:zh-CN 自定义并扩展 Win+X 菜单（右键单击“开始”按钮）
// @version         0.2.7
// @author          CTravkin
// @github          https://github.com/CTravkin
// @homepage        https://ctravkin.ru
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lruntimeobject -lole32 -loleaut32 -lshell32 -lshlwapi -ladvapi32 -liphlpapi -luuid
// ==/WindhawkMod==
// Build: 13

// ==WindhawkModReadme==
/*
# Quick Link Menu

Customize the native Windows 11 menu opened by **Win+X** or by right-clicking
the Start button.

Quick Link Menu organizes shortcuts into Windows Settings, Control Panel,
Administration, and Basic commands. The row opens its main destination.
A submenu offers related shortcuts and can be opened with the arrow or **Right**.
**Enter** runs the main action. The power row opens the Windows power submenu,
with **Lock** as its first command.

In the mod settings, the related shortcuts are listed with their main row.
Choose which rows and submenu items appear, and change how
submenus open on hover at the top of the settings page. Disabling all entries
restores the stock Win+X menu.

Windows 11 x64. Incompatible with Windows 10 Win+X Restorer.

[Project documentation](https://github.com/CTravkin/DSN_Windhawk_Mods/tree/main/Quick_Link_Menu)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- submenuHover: immediate
  $name: "Open submenus on hover"
  $name:ru-RU: "Раскрытие подменю при наведении"
  $name:es-ES: "Apertura de submenús al pasar el puntero"
  $name:de-DE: "Untermenüs beim Darüberfahren öffnen"
  $name:pt-BR: "Abrir submenus ao passar o ponteiro"
  $name:zh-CN: "悬停时打开子菜单"
  $options:
  - immediate: "Immediately"
  - delayed: "With the Windows delay"
  - arrow: "When hovering over the arrow"
  $options:ru-RU:
  - immediate: "Сразу"
  - delayed: "С задержкой Windows"
  - arrow: "При наведении на стрелку"
  $options:es-ES:
  - immediate: "Inmediatamente"
  - delayed: "Con el retardo de Windows"
  - arrow: "Al pasar el puntero sobre la flecha"
  $options:de-DE:
  - immediate: "Sofort"
  - delayed: "Mit Windows-Verzögerung"
  - arrow: "Beim Darüberfahren über den Pfeil"
  $options:pt-BR:
  - immediate: "Imediatamente"
  - delayed: "Com o atraso do Windows"
  - arrow: "Ao passar o ponteiro sobre a seta"
  $options:zh-CN:
  - immediate: "立即打开"
  - delayed: "使用 Windows 延迟"
  - arrow: "悬停在箭头上时打开"
- windowsSettings:
  - system:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - display: true
      $name: "Display"
      $name:ru-RU: "Дисплей"
      $name:es-ES: "Pantalla"
      $name:de-DE: "Bildschirm"
      $name:pt-BR: "Tela"
      $name:zh-CN: "屏幕"
    - sound: true
      $name: "Sound"
      $name:ru-RU: "Звук"
      $name:es-ES: "Sonido"
      $name:de-DE: "Sound"
      $name:pt-BR: "Som"
      $name:zh-CN: "声音"
    - power: true
      $name: "Power"
      $name:ru-RU: "Питание"
      $name:es-ES: "Inicio/Apagado"
      $name:de-DE: "Leistung"
      $name:pt-BR: "Desligar "
      $name:zh-CN: "电源"
    - storage: true
      $name: "Storage"
      $name:ru-RU: "Память"
      $name:es-ES: "Almacenamiento"
      $name:de-DE: "Speicher"
      $name:pt-BR: "Armazenamento"
      $name:zh-CN: "存储"
    - advanced: true
      $name: "Advanced"
      $name:ru-RU: "Дополнительно"
      $name:es-ES: "Avanzado"
      $name:de-DE: "Erweitert"
      $name:pt-BR: "Avançado"
      $name:zh-CN: "高级"
    - about: true
      $name: "About"
      $name:ru-RU: "О системе"
      $name:es-ES: "Información"
      $name:de-DE: "Info"
      $name:pt-BR: "Sobre"
      $name:zh-CN: "系统信息"
    $name: "System"
    $name:ru-RU: "Система"
    $name:es-ES: "Sistema"
    $name:de-DE: "System"
    $name:pt-BR: "Sistema"
    $name:zh-CN: "系统"
  - devices:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - devices: true
      $name: "Devices"
      $name:ru-RU: "Устройства"
      $name:es-ES: "Dispositivos"
      $name:de-DE: "Geräte"
      $name:pt-BR: "Dispositivos"
      $name:zh-CN: "设备"
    - printers: true
      $name: "Printers & scanners"
      $name:ru-RU: "Принтеры и сканеры"
      $name:es-ES: "Impresoras y escáneres"
      $name:de-DE: "Drucker und Scanner"
      $name:pt-BR: "Impressoras e scanners"
      $name:zh-CN: "打印机和扫描仪"
    - cameras: true
      $name: "Cameras"
      $name:ru-RU: "Камеры"
      $name:es-ES: "Cámaras"
      $name:de-DE: "Kameras"
      $name:pt-BR: "Câmeras"
      $name:zh-CN: "摄像头"
    - mouse: true
      $name: "Mouse"
      $name:ru-RU: "Мышь"
      $name:es-ES: "Mouse"
      $name:de-DE: "Maus"
      $name:pt-BR: "Mouse"
      $name:zh-CN: "鼠标"
    - keyboard: true
      $name: "Keyboard"
      $name:ru-RU: "Клавиатура"
      $name:es-ES: "Teclado"
      $name:de-DE: "Tastatur"
      $name:pt-BR: "Teclado"
      $name:zh-CN: "键盘"
    $name: "Bluetooth & devices"
    $name:ru-RU: "Bluetooth и устройства"
    $name:es-ES: "Bluetooth y dispositivos"
    $name:de-DE: "Bluetooth und Geräte"
    $name:pt-BR: "Bluetooth e dispositivos"
    $name:zh-CN: "蓝牙和其他设备"
  - network:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - ethernet: true
      $name: "Ethernet"
      $name:ru-RU: "Ethernet"
      $name:es-ES: "Ethernet"
      $name:de-DE: "Ethernet"
      $name:pt-BR: "Ethernet"
      $name:zh-CN: "以太网"
    - wifi: true
      $name: "Wi-Fi"
      $name:ru-RU: "Wi-Fi"
      $name:es-ES: "Wi-Fi"
      $name:de-DE: "WLAN"
      $name:pt-BR: "Wi-Fi"
      $name:zh-CN: "WLAN"
    - hotspot: true
      $name: "Mobile hotspot"
      $name:ru-RU: "Мобильный хот-спот"
      $name:es-ES: "Zona con cobertura inalámbrica móvil"
      $name:de-DE: "Mobiler Hotspot"
      $name:pt-BR: "Hotspot móvel"
      $name:zh-CN: "移动热点"
    - vpn: true
      $name: "VPN"
      $name:ru-RU: "VPN"
      $name:es-ES: "VPN"
      $name:de-DE: "VPN"
      $name:pt-BR: "VPN"
      $name:zh-CN: "VPN"
    - proxy: true
      $name: "Proxy"
      $name:ru-RU: "Прокси-сервер"
      $name:es-ES: "Proxy"
      $name:de-DE: "Proxy"
      $name:pt-BR: "Proxy"
      $name:zh-CN: "代理"
    - advancedNetwork: true
      $name: "Advanced network settings"
      $name:ru-RU: "Дополнительные сетевые параметры"
      $name:es-ES: "Configuración de red avanzada"
      $name:de-DE: "Erweiterte Netzwerkeinstellungen"
      $name:pt-BR: "Configurações avançadas de rede"
      $name:zh-CN: "高级网络设置"
    $name: "Network & internet"
    $name:ru-RU: "Сеть и Интернет"
    $name:es-ES: "Red e Internet"
    $name:de-DE: "Netzwerk und Internet"
    $name:pt-BR: "Rede e Internet"
    $name:zh-CN: "网络和 Internet"
  - personalization:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - background: true
      $name: "Background"
      $name:ru-RU: "Фон"
      $name:es-ES: "Fondo"
      $name:de-DE: "Hintergrund"
      $name:pt-BR: "Tela de fundo"
      $name:zh-CN: "背景"
    - colors: true
      $name: "Colors"
      $name:ru-RU: "Цвета"
      $name:es-ES: "Colores"
      $name:de-DE: "Farben"
      $name:pt-BR: "Cores"
      $name:zh-CN: "颜色"
    - themes: true
      $name: "Themes"
      $name:ru-RU: "Темы"
      $name:es-ES: "Temas"
      $name:de-DE: "Designs"
      $name:pt-BR: "Temas"
      $name:zh-CN: "主题"
    - lockScreen: true
      $name: "Lock screen"
      $name:ru-RU: "Экран блокировки"
      $name:es-ES: "Pantalla de bloqueo"
      $name:de-DE: "Sperrbildschirm"
      $name:pt-BR: "Tela de bloqueio"
      $name:zh-CN: "锁屏界面"
    - start: true
      $name: "Start"
      $name:ru-RU: "Пуск"
      $name:es-ES: "Inicio"
      $name:de-DE: "Start"
      $name:pt-BR: "Iniciar"
      $name:zh-CN: "开始"
    - taskbar: true
      $name: "Taskbar"
      $name:ru-RU: "Панель задач"
      $name:es-ES: "Barra de tareas"
      $name:de-DE: "Taskleiste"
      $name:pt-BR: "Barra de Tarefas"
      $name:zh-CN: "任务栏"
    $name: "Personalization"
    $name:ru-RU: "Персонализация"
    $name:es-ES: "Personalización"
    $name:de-DE: "Personalisierung"
    $name:pt-BR: "Personalização"
    $name:zh-CN: "个性化"
  - apps:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - installed: true
      $name: "Installed apps"
      $name:ru-RU: "Установленные приложения"
      $name:es-ES: "Aplicaciones instaladas"
      $name:de-DE: "Installierte Apps"
      $name:pt-BR: "Aplicativos instalados"
      $name:zh-CN: "安装的应用"
    - defaults: true
      $name: "Default apps"
      $name:ru-RU: "Приложения по умолчанию"
      $name:es-ES: "Aplicaciones predeterminadas"
      $name:de-DE: "Standard-Apps"
      $name:pt-BR: "Aplicativos padrão"
      $name:zh-CN: "默认应用"
    - startup: true
      $name: "Startup"
      $name:ru-RU: "Автозагрузка"
      $name:es-ES: "Inicio"
      $name:de-DE: "Autostart"
      $name:pt-BR: "Inicialização"
      $name:zh-CN: "启动"
    $name: "Apps"
    $name:ru-RU: "Приложения"
    $name:es-ES: "Aplicaciones"
    $name:de-DE: "Apps"
    $name:pt-BR: "Aplicativos"
    $name:zh-CN: "应用"
  - accounts:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - signIn: true
      $name: "Sign-in options"
      $name:ru-RU: "Варианты входа"
      $name:es-ES: "Opciones de inicio de sesión"
      $name:de-DE: "Anmeldeoptionen"
      $name:pt-BR: "Opções de entrada"
      $name:zh-CN: "登录选项"
    - backup: true
      $name: "Windows backup"
      $name:ru-RU: "Резервное копирование Windows"
      $name:es-ES: "Copia de seguridad en Windows"
      $name:de-DE: "Windows-Sicherung"
      $name:pt-BR: "Backup do Windows"
      $name:zh-CN: "Windows 备份"
    - passkeys: true
      $name: "Passkeys"
      $name:ru-RU: "Ключи доступа"
      $name:es-ES: "Claves de acceso"
      $name:de-DE: "Hauptschlüssel"
      $name:pt-BR: "Chaves de acesso"
      $name:zh-CN: "通行密钥"
    $name: "Accounts"
    $name:ru-RU: "Учетные записи"
    $name:es-ES: "Cuentas"
    $name:de-DE: "Konten"
    $name:pt-BR: "Contas"
    $name:zh-CN: "帐户"
  - timeLanguage:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - dateTime: true
      $name: "Date & time"
      $name:ru-RU: "Дата и время"
      $name:es-ES: "Fecha y hora"
      $name:de-DE: "Datum und Zeit"
      $name:pt-BR: "Data e hora"
      $name:zh-CN: "日期和时间"
    - language: true
      $name: "Language & region"
      $name:ru-RU: "Язык и регион"
      $name:es-ES: "Idioma y región"
      $name:de-DE: "Sprache und Region"
      $name:pt-BR: "Idioma e região"
      $name:zh-CN: "语言和区域"
    - typing: true
      $name: "Typing"
      $name:ru-RU: "Ввод"
      $name:es-ES: "Escritura"
      $name:de-DE: "Eingabe"
      $name:pt-BR: "Digitação"
      $name:zh-CN: "输入"
    $name: "Time & language"
    $name:ru-RU: "Время и язык"
    $name:es-ES: "Hora e idioma"
    $name:de-DE: "Zeit und Sprache"
    $name:pt-BR: "Hora e idioma"
    $name:zh-CN: "时间和语言"
  - gaming: true
    $name: "Gaming"
    $name:ru-RU: "Игры"
    $name:es-ES: "Juegos"
    $name:de-DE: "Spielen"
    $name:pt-BR: "Jogos"
    $name:zh-CN: "游戏"
  - accessibility: true
    $name: "Accessibility"
    $name:ru-RU: "Специальные возможности"
    $name:es-ES: "Accesibilidad"
    $name:de-DE: "Barrierefreiheit"
    $name:pt-BR: "Acessibilidade"
    $name:zh-CN: "辅助功能"
  - privacy: true
    $name: "Privacy & security"
    $name:ru-RU: "Конфиденциальность и защита"
    $name:es-ES: "Privacidad y seguridad"
    $name:de-DE: "Datenschutz und Sicherheit"
    $name:pt-BR: "Privacidade e segurança"
    $name:zh-CN: "隐私和安全性"
  - update: true
    $name: "Windows Update"
    $name:ru-RU: "Центр обновления Windows"
    $name:es-ES: "Windows Update"
    $name:de-DE: "Windows Update"
    $name:pt-BR: "Windows Update"
    $name:zh-CN: "Windows 更新"
  $name: "Settings"
  $name:ru-RU: "Параметры"
  $name:es-ES: "Configuración"
  $name:de-DE: "Einstellungen"
  $name:pt-BR: "Configurações"
  $name:zh-CN: "设置"
  $description: "Windows Settings categories and pages"
  $description:ru-RU: "Разделы и страницы приложения «Параметры»"
  $description:es-ES: "Categorías y páginas de Configuración de Windows"
  $description:de-DE: "Kategorien und Seiten der Windows-Einstellungen"
  $description:pt-BR: "Categorias e páginas das Configurações do Windows"
  $description:zh-CN: "Windows 设置的类别和页面"
- controlPanel:
  - security:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - maintenance: true
      $name: "Security and Maintenance"
      $name:ru-RU: "Центр безопасности и обслуживания"
      $name:es-ES: "Seguridad y mantenimiento"
      $name:de-DE: "Sicherheit und Wartung"
      $name:pt-BR: "Segurança e Manutenção"
      $name:zh-CN: "安全和维护"
    - firewall: true
      $name: "Windows Defender Firewall"
      $name:ru-RU: "Брандмауэр Защитника Windows"
      $name:es-ES: "Firewall de Windows Defender"
      $name:de-DE: "Windows Defender Firewall"
      $name:pt-BR: "Windows Defender Firewall"
      $name:zh-CN: "Windows Defender 防火墙"
    - powerOptions: true
      $name: "Power Options"
      $name:ru-RU: "Электропитание"
      $name:es-ES: "Opciones de energía"
      $name:de-DE: "Energieoptionen"
      $name:pt-BR: "Opções de Energia"
      $name:zh-CN: "电源选项"
    - bitlocker: true
      $name: "BitLocker Drive Encryption"
      $name:ru-RU: "Шифрование диска BitLocker"
      $name:es-ES: "Cifrado de unidad BitLocker"
      $name:de-DE: "BitLocker-Laufwerkverschlüsselung"
      $name:pt-BR: "Criptografia de Unidade de Disco BitLocker"
      $name:zh-CN: "BitLocker 驱动器加密"
    $name: "System and Security"
    $name:ru-RU: "Система и безопасность"
    $name:es-ES: "Sistema y seguridad"
    $name:de-DE: "System und Sicherheit"
    $name:pt-BR: "Sistema e Segurança"
    $name:zh-CN: "系统和安全"
  - network:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - sharing: true
      $name: "Network and Sharing Center"
      $name:ru-RU: "Центр управления сетями и общим доступом"
      $name:es-ES: "Centro de redes y recursos compartidos"
      $name:de-DE: "Netzwerk- und Freigabecenter"
      $name:pt-BR: "Central de Rede e Compartilhamento"
      $name:zh-CN: "网络和共享中心"
    - connections: true
      $name: "Network Connections"
      $name:ru-RU: "Сетевые подключения"
      $name:es-ES: "Conexiones de red"
      $name:de-DE: "Netzwerkverbindungen"
      $name:pt-BR: "Conexões de Rede"
      $name:zh-CN: "网络连接(W)"
    - internetOptions: true
      $name: "Internet Options"
      $name:ru-RU: "Свойства браузера"
      $name:es-ES: "Opciones de Internet"
      $name:de-DE: "Internetoptionen"
      $name:pt-BR: "Opções da Internet"
      $name:zh-CN: "Internet 选项"
    $name: "Network and Internet"
    $name:ru-RU: "Сеть и Интернет"
    $name:es-ES: "Redes e Internet"
    $name:de-DE: "Netzwerk und Internet"
    $name:pt-BR: "Rede e Internet"
    $name:zh-CN: "网络和 Internet"
  - hardware:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - deviceManager: true
      $name: "Device Manager"
      $name:ru-RU: "Диспетчер устройств"
      $name:es-ES: "Administrador de dispositivos"
      $name:de-DE: "Geräte-Manager"
      $name:pt-BR: "Gerenciador de Dispositivos"
      $name:zh-CN: "设备管理器(M)"
    - sound: true
      $name: "Sound"
      $name:ru-RU: "Звук"
      $name:es-ES: "Sonido"
      $name:de-DE: "Sound"
      $name:pt-BR: "Som"
      $name:zh-CN: "声音"
    - mouse: true
      $name: "Mouse"
      $name:ru-RU: "Мышь"
      $name:es-ES: "Mouse"
      $name:de-DE: "Maus"
      $name:pt-BR: "Mouse"
      $name:zh-CN: "鼠标"
    - keyboard: true
      $name: "Keyboard"
      $name:ru-RU: "Клавиатура"
      $name:es-ES: "Teclado"
      $name:de-DE: "Tastatur"
      $name:pt-BR: "Teclado"
      $name:zh-CN: "键盘"
    - controllers: true
      $name: "Game Controllers"
      $name:ru-RU: "Игровые устройства"
      $name:es-ES: "Dispositivos de juego"
      $name:de-DE: "Gamecontroller"
      $name:pt-BR: "Controladores de Jogo"
      $name:zh-CN: "游戏控制器"
    $name: "Hardware and Sound"
    $name:ru-RU: "Оборудование и звук"
    $name:es-ES: "Hardware y sonido"
    $name:de-DE: "Hardware und Sound"
    $name:pt-BR: "Hardware e Sons"
    $name:zh-CN: "硬件和声音"
  - programs:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - programsFeatures: true
      $name: "Programs and Features"
      $name:ru-RU: "Программы и компоненты"
      $name:es-ES: "Programas y características"
      $name:de-DE: "Programme und Features"
      $name:pt-BR: "Programas e Recursos"
      $name:zh-CN: "程序和功能"
    - windowsFeatures: true
      $name: "Turn Windows features on or off"
      $name:ru-RU: "Включение или отключение компонентов Windows"
      $name:es-ES: "Activar o desactivar las características de Windows"
      $name:de-DE: "Windows-Features aktivieren oder deaktivieren"
      $name:pt-BR: "Ativar ou desativar recursos do Windows"
      $name:zh-CN: "启用或关闭 Windows 功能"
    $name: "Programs"
    $name:ru-RU: "Программы"
    $name:es-ES: "Programas"
    $name:de-DE: "Programme"
    $name:pt-BR: "Programas"
    $name:zh-CN: "程序"
  - userAccounts:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - credentials: true
      $name: "Credential Manager"
      $name:ru-RU: "Диспетчер учетных данных"
      $name:es-ES: "Administrador de credenciales"
      $name:de-DE: "Anmeldeinformationsverwaltung"
      $name:pt-BR: "Gerenciador de Credenciais"
      $name:zh-CN: "凭据管理器"
    - uac: true
      $name: "Change User Account Control settings"
      $name:ru-RU: "Изменение параметров контроля учетных записей"
      $name:es-ES: "Cambiar configuración de Control de cuentas de usuario"
      $name:de-DE: "Einstellungen der Benutzerkontensteuerung ändern"
      $name:pt-BR: "Alterar configurações de Controle de Conta de Usuário"
      $name:zh-CN: "更改用户帐户控制设置"
    $name: "User Accounts"
    $name:ru-RU: "Учетные записи пользователей"
    $name:es-ES: "Cuentas de usuario"
    $name:de-DE: "Benutzerkonten"
    $name:pt-BR: "Contas de Usuário"
    $name:zh-CN: "用户帐户"
  - appearance:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - explorerOptions: true
      $name: "File Explorer Options"
      $name:ru-RU: "Параметры Проводника"
      $name:es-ES: "Opciones del Explorador de archivos"
      $name:de-DE: "Explorer-Optionen"
      $name:pt-BR: "Opções do Explorador de Arquivos"
      $name:zh-CN: "文件资源管理器选项"
    - desktopIcons: true
      $name: "Desktop Icon Settings"
      $name:ru-RU: "Параметры значков рабочего стола"
      $name:es-ES: "Configuración de iconos de escritorio"
      $name:de-DE: "Desktopsymboleinstellungen"
      $name:pt-BR: "Configurações dos Ícones da Área de Trabalho"
      $name:zh-CN: "桌面图标设置"
    - colorManagement: true
      $name: "Color Management"
      $name:ru-RU: "Управление цветом"
      $name:es-ES: "Administración del color"
      $name:de-DE: "Farbverwaltung"
      $name:pt-BR: "Gerenciamento de Cores"
      $name:zh-CN: "颜色管理"
    $name: "Appearance and Personalization"
    $name:ru-RU: "Оформление и персонализация"
    $name:es-ES: "Apariencia y personalización"
    $name:de-DE: "Darstellung und Anpassung"
    $name:pt-BR: "Aparência e Personalização"
    $name:zh-CN: "外观和个性化"
  - clockRegion:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - dateTime: true
      $name: "Date and Time"
      $name:ru-RU: "Дата и время"
      $name:es-ES: "Fecha y hora"
      $name:de-DE: "Datum und Uhrzeit"
      $name:pt-BR: "Data e Hora"
      $name:zh-CN: "日期和时间"
    - region: true
      $name: "Region"
      $name:ru-RU: "Региональные стандарты"
      $name:es-ES: "Región"
      $name:de-DE: "Region"
      $name:pt-BR: "Região"
      $name:zh-CN: "区域"
    $name: "Clock and Region"
    $name:ru-RU: "Часы и регион"
    $name:es-ES: "Reloj y región"
    $name:de-DE: "Zeit und Region"
    $name:pt-BR: "Relógio e Região"
    $name:zh-CN: "时钟和区域"
  - accessibility: true
    $name: "Ease of Access"
    $name:ru-RU: "Специальные возможности"
    $name:es-ES: "Accesibilidad"
    $name:de-DE: "Erleichterte Bedienung"
    $name:pt-BR: "Facilidade de Acesso"
    $name:zh-CN: "轻松使用"
  $name: "Control Panel"
  $name:ru-RU: "Панель управления"
  $name:es-ES: "Panel de control"
  $name:de-DE: "Systemsteuerung"
  $name:pt-BR: "Painel de Controle"
  $name:zh-CN: "控制面板"
  $description: "Classic Control Panel categories and windows"
  $description:ru-RU: "Категории и классические окна Панели управления"
  $description:es-ES: "Categorías y ventanas clásicas del Panel de control"
  $description:de-DE: "Kategorien und klassische Fenster der Systemsteuerung"
  $description:pt-BR: "Categorias e janelas clássicas do Painel de Controle"
  $description:zh-CN: "控制面板类别和经典窗口"
- administration:
  - tools:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - userCertificates: true
      $name: "Certificates - Current User"
      $name:ru-RU: "Сертификаты - текущий пользователь"
      $name:es-ES: "Certificados: usuario actual"
      $name:de-DE: "Zertifikate - Aktueller Benutzer"
      $name:pt-BR: "Certificados - Usuário Atual"
      $name:zh-CN: "证书 - 当前用户"
    - machineCertificates: true
      $name: "Certificates (Local Computer)"
      $name:ru-RU: "Сертификаты (локальный компьютер)"
      $name:es-ES: "Certificados (equipo local)"
      $name:de-DE: "Zertifikate (Lokaler Computer)"
      $name:pt-BR: "Certificados (computador local)"
      $name:zh-CN: "证书(本地计算机)"
    - firewallAdvanced: true
      $name: "Windows Defender Firewall with Advanced Security"
      $name:ru-RU: "Монитор брандмауэра Защитника Windows в режиме повышенной безопасности"
      $name:es-ES: "Windows Defender Firewall con seguridad avanzada"
      $name:de-DE: "Windows Defender Firewall mit erweiterter Sicherheit"
      $name:pt-BR: "Windows Defender Firewall com Segurança Avançada"
      $name:zh-CN: "高级安全 Windows Defender 防火墙"
    $name: "Windows Tools"
    $name:ru-RU: "Инструменты Windows"
    $name:es-ES: "Herramientas de Windows"
    $name:de-DE: "Windows-Tools"
    $name:pt-BR: "Ferramentas do Windows"
    $name:zh-CN: "Windows 工具"
  - computerManagement:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - sharedFolders: true
      $name: "Shared Folders"
      $name:ru-RU: "Общие папки"
      $name:es-ES: "Carpetas compartidas"
      $name:de-DE: "Freigegebene Ordner"
      $name:pt-BR: "Pastas compartilhadas"
      $name:zh-CN: "共享文件夹"
    - localUsers: true
      $name: "Local Users and Groups"
      $name:ru-RU: "Локальные пользователи и группы"
      $name:es-ES: "Usuarios y grupos locales"
      $name:de-DE: "Lokale Benutzer und Gruppen"
      $name:pt-BR: "Usuários e Grupos Locais"
      $name:zh-CN: "本地用户和组"
    - wmi: true
      $name: "WMI Control"
      $name:ru-RU: "Управляющий элемент WMI"
      $name:es-ES: "Control WMI"
      $name:de-DE: "WMI-Steuerung"
      $name:pt-BR: "Controle WMI"
      $name:zh-CN: "WMI 控件"
    $name: "Computer Management"
    $name:ru-RU: "Управление компьютером"
    $name:es-ES: "Administración de equipos"
    $name:de-DE: "Computerverwaltung"
    $name:pt-BR: "Gerenciamento do Computador"
    $name:zh-CN: "计算机管理(G)"
  - diskManagement:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - optimize: true
      $name: "Defragment and Optimize Drives"
      $name:ru-RU: "Оптимизация дисков"
      $name:es-ES: "Desfragmentar y optimizar unidades"
      $name:de-DE: "Laufwerke defragmentieren und optimieren"
      $name:pt-BR: "Desfragmentar e Otimizar Unidades"
      $name:zh-CN: "碎片整理和优化驱动器"
    - cleanup: true
      $name: "Disk Cleanup"
      $name:ru-RU: "Очистка диска"
      $name:es-ES: "Liberador de espacio en disco"
      $name:de-DE: "Datenträgerbereinigung"
      $name:pt-BR: "Limpeza de Disco"
      $name:zh-CN: "磁盘清理"
    - recoveryDrive: true
      $name: "Recovery Drive"
      $name:ru-RU: "Диск восстановления"
      $name:es-ES: "Unidad de recuperación"
      $name:de-DE: "Wiederherstellungslaufwerk"
      $name:pt-BR: "Unidade de Recuperação"
      $name:zh-CN: "恢复驱动器"
    $name: "Disk Management"
    $name:ru-RU: "Управление дисками"
    $name:es-ES: "Administración de discos"
    $name:de-DE: "Datenträgerverwaltung"
    $name:pt-BR: "Gerenciamento de Disco"
    $name:zh-CN: "磁盘管理(K)"
  - services:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - taskScheduler: true
      $name: "Task Scheduler"
      $name:ru-RU: "Планировщик задач"
      $name:es-ES: "Programador de tareas"
      $name:de-DE: "Aufgabenplanung"
      $name:pt-BR: "Agendador de Tarefas"
      $name:zh-CN: "任务计划程序"
    - componentServices: true
      $name: "Component Services"
      $name:ru-RU: "Службы компонентов"
      $name:es-ES: "Servicios de componentes"
      $name:de-DE: "Komponentendienste"
      $name:pt-BR: "Serviços de Componentes"
      $name:zh-CN: "组件服务"
    $name: "Services"
    $name:ru-RU: "Службы"
    $name:es-ES: "Servicios"
    $name:de-DE: "Dienste"
    $name:pt-BR: "Serviços"
    $name:zh-CN: "服务"
  - eventViewer:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - performance: true
      $name: "Performance Monitor"
      $name:ru-RU: "Системный монитор"
      $name:es-ES: "Monitor de rendimiento"
      $name:de-DE: "Leistungsüberwachung"
      $name:pt-BR: "Monitor de Desempenho"
      $name:zh-CN: "性能监视器"
    - resources: true
      $name: "Resource Monitor"
      $name:ru-RU: "Монитор ресурсов"
      $name:es-ES: "Monitor de recursos"
      $name:de-DE: "Ressourcenmonitor"
      $name:pt-BR: "Monitor de Recursos"
      $name:zh-CN: "资源监视器"
    - reliability: true
      $name: "Reliability Monitor"
      $name:ru-RU: "Монитор стабильности системы"
      $name:es-ES: "Monitor de confiabilidad"
      $name:de-DE: "Zuverlässigkeitsüberwachung"
      $name:pt-BR: "Monitor de Confiabilidade"
      $name:zh-CN: "可靠性监视程序"
    - systemInfo: true
      $name: "System Information"
      $name:ru-RU: "Сведения о системе"
      $name:es-ES: "Información del sistema"
      $name:de-DE: "Systeminformationen"
      $name:pt-BR: "Informações do sistema"
      $name:zh-CN: "系统信息"
    - memoryDiagnostic: true
      $name: "Windows Memory Diagnostic"
      $name:ru-RU: "Средство проверки памяти Windows"
      $name:es-ES: "Diagnóstico de memoria de Windows"
      $name:de-DE: "Windows-Speicherdiagnose"
      $name:pt-BR: "Diagnóstico de Memória do Windows"
      $name:zh-CN: "Windows 内存诊断"
    $name: "Event Viewer"
    $name:ru-RU: "Просмотр событий"
    $name:es-ES: "Visor de eventos"
    $name:de-DE: "Ereignisanzeige"
    $name:pt-BR: "Visualizador de Eventos"
    $name:zh-CN: "事件查看器(V)"
  - registry:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - systemConfiguration: true
      $name: "System Configuration"
      $name:ru-RU: "Конфигурация системы"
      $name:es-ES: "Configuración del sistema"
      $name:de-DE: "Systemkonfiguration"
      $name:pt-BR: "Configuração do Sistema"
      $name:zh-CN: "系统配置"
    - groupPolicy: true
      $name: "Local Group Policy Editor"
      $name:ru-RU: "Редактор локальной групповой политики"
      $name:es-ES: "Editor de directivas de grupo local"
      $name:de-DE: "Editor für lokale Gruppenrichtlinien"
      $name:pt-BR: "Editor de Política de Grupo Local"
      $name:zh-CN: "本地组策略编辑器"
    - localSecurity: true
      $name: "Local Security Policy"
      $name:ru-RU: "Локальная политика безопасности"
      $name:es-ES: "Directiva de seguridad local"
      $name:de-DE: "Lokale Sicherheitsrichtlinie"
      $name:pt-BR: "Política de Segurança Local"
      $name:zh-CN: "本地安全策略"
    $name: "Registry Editor"
    $name:ru-RU: "Редактор реестра"
    $name:es-ES: "Editor del Registro"
    $name:de-DE: "Registrierungs-Editor"
    $name:pt-BR: "Editor do Registro"
    $name:zh-CN: "注册表编辑器"
  - terminal:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - terminalAdmin: true
      $name: "Terminal (Admin)"
      $name:ru-RU: "Терминал (Администратор)"
      $name:es-ES: "Terminal (Administrador)"
      $name:de-DE: "Terminal (Administrator)"
      $name:pt-BR: "Terminal (Administrador)"
      $name:zh-CN: "终端管理员(A)"
    - powershell: true
      $name: "Windows PowerShell"
      $name:ru-RU: "Windows PowerShell"
      $name:es-ES: "Windows PowerShell"
      $name:de-DE: "Windows PowerShell"
      $name:pt-BR: "Windows PowerShell"
      $name:zh-CN: "Windows PowerShell(I)"
    - commandPrompt: true
      $name: "Command Prompt"
      $name:ru-RU: "Командная строка"
      $name:es-ES: "Símbolo del sistema"
      $name:de-DE: "Eingabeaufforderung"
      $name:pt-BR: "Prompt de Comando"
      $name:zh-CN: "命令提示符(C)"
    $name: "Terminal"
    $name:ru-RU: "Терминал"
    $name:es-ES: "Terminal"
    $name:de-DE: "Terminal"
    $name:pt-BR: "Terminal"
    $name:zh-CN: "终端(I)"
  $name: "Administration"
  $name:ru-RU: "Администрирование"
  $name:es-ES: "Administración"
  $name:de-DE: "Verwaltung"
  $name:pt-BR: "Administração"
  $name:zh-CN: "管理"
  $description: "System consoles, diagnostics, and terminals"
  $description:ru-RU: "Системные оснастки, диагностика и терминалы"
  $description:es-ES: "Consolas del sistema, diagnóstico y terminales"
  $description:de-DE: "Systemkonsolen, Diagnose und Terminals"
  $description:pt-BR: "Consoles do sistema, diagnóstico e terminais"
  $description:zh-CN: "系统控制台、诊断和终端"
- commands:
  - run: true
    $name: "Run"
    $name:ru-RU: "Выполнить"
    $name:es-ES: "Ejecutar"
    $name:de-DE: "Ausführen"
    $name:pt-BR: "Executar"
    $name:zh-CN: "运行(R)"
  - taskManager: true
    $name: "Task Manager"
    $name:ru-RU: "Диспетчер задач"
    $name:es-ES: "Administrador de tareas"
    $name:de-DE: "Task-Manager"
    $name:pt-BR: "Gerenciador de Tarefas"
    $name:zh-CN: "任务管理器(T)"
  - explorer:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - desktop: true
      $name: "Desktop"
      $name:ru-RU: "Рабочий стол"
      $name:es-ES: "Escritorio"
      $name:de-DE: "Desktop"
      $name:pt-BR: "Área de Trabalho"
      $name:zh-CN: "桌面"
    - computer: true
      $name: "This PC"
      $name:ru-RU: "Этот компьютер"
      $name:es-ES: "Este equipo"
      $name:de-DE: "Dieser PC"
      $name:pt-BR: "Este Computador"
      $name:zh-CN: "此电脑"
    - personal: true
      $name: "Personal Folder"
      $name:ru-RU: "Личная папка"
      $name:es-ES: "Carpeta personal"
      $name:de-DE: "Persönlicher Ordner"
      $name:pt-BR: "Pasta Pessoal"
      $name:zh-CN: "个人文件夹"
    - documents: false
      $name: "Documents"
      $name:ru-RU: "Документы"
      $name:es-ES: "Documentos"
      $name:de-DE: "Dokumente"
      $name:pt-BR: "Documentos"
      $name:zh-CN: "文档"
    - pictures: false
      $name: "Pictures"
      $name:ru-RU: "Изображения"
      $name:es-ES: "Imágenes"
      $name:de-DE: "Bilder"
      $name:pt-BR: "Imagens"
      $name:zh-CN: "图片"
    - downloads: true
      $name: "Downloads"
      $name:ru-RU: "Загрузки"
      $name:es-ES: "Descargas"
      $name:de-DE: "Downloads"
      $name:pt-BR: "Downloads"
      $name:zh-CN: "下载"
    - recycleBin: true
      $name: "Recycle Bin"
      $name:ru-RU: "Корзина"
      $name:es-ES: "Papelera de reciclaje"
      $name:de-DE: "Papierkorb"
      $name:pt-BR: "Lixeira"
      $name:zh-CN: "回收站"
    $name: "File Explorer"
    $name:ru-RU: "Проводник"
    $name:es-ES: "Explorador de archivos"
    $name:de-DE: "Explorer"
    $name:pt-BR: "Explorador de Arquivos"
    $name:zh-CN: "文件资源管理器(E)"
  - find: false
    $name: "Search"
    $name:ru-RU: "Найти"
    $name:es-ES: "Buscar"
    $name:de-DE: "Suchen"
    $name:pt-BR: "Pesquisar"
    $name:zh-CN: "搜索(S)"
  - desktop: false
    $name: "Desktop"
    $name:ru-RU: "Рабочий стол"
    $name:es-ES: "Escritorio"
    $name:de-DE: "Desktop"
    $name:pt-BR: "Área de Trabalho"
    $name:zh-CN: "桌面(D)"
  - power:
    - enabled: true
      $name: "Show in menu"
      $name:ru-RU: "Показывать в меню"
      $name:es-ES: "Mostrar en el menú"
      $name:de-DE: "Im Menü anzeigen"
      $name:pt-BR: "Mostrar no menu"
      $name:zh-CN: "在菜单中显示"
    - lock: true
      $name: "Lock"
      $name:ru-RU: "Заблокировать"
      $name:es-ES: "Bloquear"
      $name:de-DE: "Sperren"
      $name:pt-BR: "Bloquear"
      $name:zh-CN: "锁定"
    - signOut: true
      $name: "Sign out"
      $name:ru-RU: "Выход"
      $name:es-ES: "Cerrar sesión"
      $name:de-DE: "Abmelden"
      $name:pt-BR: "Sair"
      $name:zh-CN: "注销"
    - sleep: true
      $name: "Sleep"
      $name:ru-RU: "Спящий режим"
      $name:es-ES: "Suspender"
      $name:de-DE: "Energie sparen"
      $name:pt-BR: "Suspender"
      $name:zh-CN: "睡眠"
    - hibernate: true
      $name: "Hibernate"
      $name:ru-RU: "Гибернация"
      $name:es-ES: "Hibernar"
      $name:de-DE: "Ruhezustand"
      $name:pt-BR: "Hibernar"
      $name:zh-CN: "休眠"
    - shutdown: true
      $name: "Shut down"
      $name:ru-RU: "Завершение работы"
      $name:es-ES: "Apagar"
      $name:de-DE: "Herunterfahren"
      $name:pt-BR: "Desligar"
      $name:zh-CN: "关机"
    - restart: true
      $name: "Restart"
      $name:ru-RU: "Перезагрузка"
      $name:es-ES: "Reiniciar"
      $name:de-DE: "Neu starten"
      $name:pt-BR: "Reiniciar"
      $name:zh-CN: "重启"
    $name: "Shut down or sign out"
    $name:ru-RU: "Завершение работы или выход из системы"
    $name:es-ES: "Apagar o cerrar sesión"
    $name:de-DE: "Herunterfahren oder abmelden"
    $name:pt-BR: "Desligar ou sair"
    $name:zh-CN: "关机或注销(U)"
  $name: "Basic commands"
  $name:ru-RU: "Основные команды"
  $name:es-ES: "Comandos básicos"
  $name:de-DE: "Grundbefehle"
  $name:pt-BR: "Comandos básicos"
  $name:zh-CN: "基本命令"
  $description: "File Explorer, system actions, and power"
  $description:ru-RU: "Проводник, системные действия и питание"
  $description:es-ES: "Explorador de archivos, acciones del sistema y energía"
  $description:de-DE: "Datei-Explorer, Systemaktionen und Energie"
  $description:pt-BR: "Explorador de Arquivos, ações do sistema e energia"
  $description:zh-CN: "文件资源管理器、系统操作和电源"
*/
// ==/WindhawkModSettings==

#include <winsock2.h>
#include <shellapi.h>
#include <ipifcons.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <netioapi.h>
#include <uiautomationclient.h>
#include <windhawk_utils.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Automation.Provider.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace X = winrt::Windows::UI::Xaml;
namespace C = X::Controls;
namespace P = C::Primitives;
namespace I = X::Input;
namespace AP = X::Automation::Peers;
namespace AV = X::Automation::Provider;
using Key = winrt::Windows::System::VirtualKey;
using PointerType = winrt::Windows::Devices::Input::PointerDeviceType;

namespace {

struct CatalogItem {
    PCWSTR key;
    int group;
    int parent;
    bool enabledByDefault;
    wchar_t glyph;
    PCWSTR action;
    PCWSTR resource;
    std::array<PCWSTR, 6> names;
};

constexpr CatalogItem catalog[] = {
    {L"windowsSettings.system.enabled", 0, -1, true, 0xe770, L"uri:system", L"settings:SettingsPageGroupPCSystem",
     {L"System", L"Система", L"Sistema", L"System", L"Sistema", L"系统"}},
    {L"windowsSettings.system.display", 0, 0, true, 0xe770, L"uri:display", L"settings:SettingsPagePCSystemDisplay",
     {L"Display", L"Дисплей", L"Pantalla", L"Bildschirm", L"Tela", L"屏幕"}},
    {L"windowsSettings.system.sound", 0, 0, true, 0xe767, L"uri:sound", L"settings:SettingsPageAudio",
     {L"Sound", L"Звук", L"Sonido", L"Sound", L"Som", L"声音"}},
    {L"windowsSettings.system.power", 0, 0, true, 0xe7e8, L"uri:powersleep", L"settings:SettingsPagePowerAndBattery",
     {L"Power", L"Питание", L"Inicio/Apagado", L"Leistung", L"Desligar ", L"电源"}},
    {L"windowsSettings.system.storage", 0, 0, true, 0xeda2, L"uri:storagesense", L"settings:SettingsPageStorageSenseStorageOverview",
     {L"Storage", L"Память", L"Almacenamiento", L"Speicher", L"Armazenamento", L"存储"}},
    {L"windowsSettings.system.advanced", 0, 0, true, 0xec7a, L"uri:developers", L"settings:SettingsPageAdvancedSystemSettings",
     {L"Advanced", L"Дополнительно", L"Avanzado", L"Erweitert", L"Avançado", L"高级"}},
    {L"windowsSettings.system.about", 0, 0, true, 0xe946, L"uri:about", L"settings:SettingsPageAbout",
     {L"About", L"О системе", L"Información", L"Info", L"Sobre", L"系统信息"}},
    {L"windowsSettings.devices.enabled", 0, -1, true, 0xe772, L"uri:devices", L"settings:SettingsPageGroupDevices",
     {L"Bluetooth & devices", L"Bluetooth и устройства", L"Bluetooth y dispositivos", L"Bluetooth und Geräte", L"Bluetooth e dispositivos", L"蓝牙和其他设备"}},
    {L"windowsSettings.devices.devices", 0, 7, true, 0xe772, L"uri:connecteddevices", L"settings:SettingsPageDevices",
     {L"Devices", L"Устройства", L"Dispositivos", L"Geräte", L"Dispositivos", L"设备"}},
    {L"windowsSettings.devices.printers", 0, 7, true, 0xe749, L"uri:printers", L"settings:SettingsPageDevicesPrinters",
     {L"Printers & scanners", L"Принтеры и сканеры", L"Impresoras y escáneres", L"Drucker und Scanner", L"Impressoras e scanners", L"打印机和扫描仪"}},
    {L"windowsSettings.devices.cameras", 0, 7, true, 0xe722, L"uri:camera", L"settings:SettingsPageDevicesCameras",
     {L"Cameras", L"Камеры", L"Cámaras", L"Kameras", L"Câmeras", L"摄像头"}},
    {L"windowsSettings.devices.mouse", 0, 7, true, 0xe962, L"uri:mousetouchpad", L"settings:SettingsPagePCSystemDeviceSettings",
     {L"Mouse", L"Мышь", L"Mouse", L"Maus", L"Mouse", L"鼠标"}},
    {L"windowsSettings.devices.keyboard", 0, 7, true, 0xe765, L"uri:devices-keyboard", L"settings:SettingsPageDevicesKeyboard",
     {L"Keyboard", L"Клавиатура", L"Teclado", L"Tastatur", L"Teclado", L"键盘"}},
    {L"windowsSettings.network.enabled", 0, -1, true, 0xe701, L"uri:network-status", L"settings:SettingsPageGroupNetwork",
     {L"Network & internet", L"Сеть и Интернет", L"Red e Internet", L"Netzwerk und Internet", L"Rede e Internet", L"网络和 Internet"}},
    {L"windowsSettings.network.ethernet", 0, 13, true, 0xe839, L"uri:network-ethernet", L"settings:SettingsPageNetworkEthernet",
     {L"Ethernet", L"Ethernet", L"Ethernet", L"Ethernet", L"Ethernet", L"以太网"}},
    {L"windowsSettings.network.wifi", 0, 13, true, 0xe701, L"uri:network-wifi", L"settings:SettingsPageNetworkWiFi",
     {L"Wi-Fi", L"Wi-Fi", L"Wi-Fi", L"WLAN", L"Wi-Fi", L"WLAN"}},
    {L"windowsSettings.network.hotspot", 0, 13, true, 0xe704, L"uri:network-mobilehotspot", L"settings:SettingsPageNetworkMobileHotspot",
     {L"Mobile hotspot", L"Мобильный хот-спот", L"Zona con cobertura inalámbrica móvil", L"Mobiler Hotspot", L"Hotspot móvel", L"移动热点"}},
    {L"windowsSettings.network.vpn", 0, 13, true, 0xe705, L"uri:network-vpn", L"settings:SettingsPageNetworkVPN",
     {L"VPN", L"VPN", L"VPN", L"VPN", L"VPN", L"VPN"}},
    {L"windowsSettings.network.proxy", 0, 13, true, 0xe968, L"uri:network-proxy", L"settings:SettingsPageNetworkProxy",
     {L"Proxy", L"Прокси-сервер", L"Proxy", L"Proxy", L"Proxy", L"代理"}},
    {L"windowsSettings.network.advancedNetwork", 0, 13, true, 0xeda3, L"uri:network-advancedsettings", L"settings:SettingsPageNetworkManageAdapterOptions",
     {L"Advanced network settings", L"Дополнительные сетевые параметры", L"Configuración de red avanzada", L"Erweiterte Netzwerkeinstellungen", L"Configurações avançadas de rede", L"高级网络设置"}},
    {L"windowsSettings.personalization.enabled", 0, -1, true, 0xe771, L"uri:personalization", L"settings:SettingsPageGroupPersonalization",
     {L"Personalization", L"Персонализация", L"Personalización", L"Personalisierung", L"Personalização", L"个性化"}},
    {L"windowsSettings.personalization.background", 0, 20, true, 0xeb9f, L"uri:personalization-background", L"settings:SettingsPageBackground",
     {L"Background", L"Фон", L"Fondo", L"Hintergrund", L"Tela de fundo", L"背景"}},
    {L"windowsSettings.personalization.colors", 0, 20, true, 0xe2b1, L"uri:personalization-colors", L"settings:SettingsPageColors",
     {L"Colors", L"Цвета", L"Colores", L"Farben", L"Cores", L"颜色"}},
    {L"windowsSettings.personalization.themes", 0, 20, true, 0xe771, L"uri:themes", L"settings:SettingsPageThemes",
     {L"Themes", L"Темы", L"Temas", L"Designs", L"Temas", L"主题"}},
    {L"windowsSettings.personalization.lockScreen", 0, 20, true, 0xee3f, L"uri:lockscreen", L"settings:SettingsPageLockScreen",
     {L"Lock screen", L"Экран блокировки", L"Pantalla de bloqueo", L"Sperrbildschirm", L"Tela de bloqueio", L"锁屏界面"}},
    {L"windowsSettings.personalization.start", 0, 20, true, 0xe8fc, L"uri:personalization-start", L"settings:SettingsPageStart",
     {L"Start", L"Пуск", L"Inicio", L"Start", L"Iniciar", L"开始"}},
    {L"windowsSettings.personalization.taskbar", 0, 20, true, 0xe90e, L"uri:taskbar", L"settings:SettingsPageTaskbar",
     {L"Taskbar", L"Панель задач", L"Barra de tareas", L"Taskleiste", L"Barra de Tarefas", L"任务栏"}},
    {L"windowsSettings.apps.enabled", 0, -1, true, 0xe71d, L"category:installed-apps;SystemSettings_StorageSense_AppSizesListFilter_DisplayStringValue", L"settings:SettingsPageGroupApps",
     {L"Apps", L"Приложения", L"Aplicaciones", L"Apps", L"Aplicativos", L"应用"}},
    {L"windowsSettings.apps.installed", 0, 27, true, 0xe71d, L"uri:installed-apps", L"settings:SettingsPageInstalledApps",
     {L"Installed apps", L"Установленные приложения", L"Aplicaciones instaladas", L"Installierte Apps", L"Aplicativos instalados", L"安装的应用"}},
    {L"windowsSettings.apps.defaults", 0, 27, true, 0xe73a, L"uri:defaultapps", L"settings:SettingsPageAppsDefaults",
     {L"Default apps", L"Приложения по умолчанию", L"Aplicaciones predeterminadas", L"Standard-Apps", L"Aplicativos padrão", L"默认应用"}},
    {L"windowsSettings.apps.startup", 0, 27, true, 0xe7b5, L"uri:startupapps", L"settings:SettingsPageStartup",
     {L"Startup", L"Автозагрузка", L"Inicio", L"Autostart", L"Inicialização", L"启动"}},
    {L"windowsSettings.accounts.enabled", 0, -1, true, 0xe77b, L"uri:accounts", L"settings:SettingsPageGroupAccounts",
     {L"Accounts", L"Учетные записи", L"Cuentas", L"Konten", L"Contas", L"帐户"}},
    {L"windowsSettings.accounts.signIn", 0, 31, true, 0xe8d7, L"uri:signinoptions", L"settings:SettingsPageSignInOptions",
     {L"Sign-in options", L"Варианты входа", L"Opciones de inicio de sesión", L"Anmeldeoptionen", L"Opções de entrada", L"登录选项"}},
    {L"windowsSettings.accounts.backup", 0, 31, true, 0xe777, L"uri:backup", L"settings:SettingsPageAccountsSync",
     {L"Windows backup", L"Резервное копирование Windows", L"Copia de seguridad en Windows", L"Windows-Sicherung", L"Backup do Windows", L"Windows 备份"}},
    {L"windowsSettings.accounts.passkeys", 0, 31, true, 0xee7e, L"uri:savedpasskeys", L"settings:SettingsPagePasskeysManagement",
     {L"Passkeys", L"Ключи доступа", L"Claves de acceso", L"Hauptschlüssel", L"Chaves de acesso", L"通行密钥"}},
    {L"windowsSettings.timeLanguage.enabled", 0, -1, true, 0xe775, L"category:dateandtime;SystemSettings_DateTime_IsTimeSetAutomaticallyEnabled_ToggleSwitch", L"settings:SettingsPageGroupTimeRegion",
     {L"Time & language", L"Время и язык", L"Hora e idioma", L"Zeit und Sprache", L"Hora e idioma", L"时间和语言"}},
    {L"windowsSettings.timeLanguage.dateTime", 0, 35, true, 0xec92, L"uri:dateandtime", L"settings:SettingsPageTimeRegionDateTime",
     {L"Date & time", L"Дата и время", L"Fecha y hora", L"Datum und Zeit", L"Data e hora", L"日期和时间"}},
    {L"windowsSettings.timeLanguage.language", 0, 35, true, 0xf2b7, L"uri:regionlanguage", L"settings:SettingsPageTimeRegionLanguage",
     {L"Language & region", L"Язык и регион", L"Idioma y región", L"Sprache und Region", L"Idioma e região", L"语言和区域"}},
    {L"windowsSettings.timeLanguage.typing", 0, 35, true, 0xe765, L"uri:typing", L"settings:SettingsPageTimeRegionSpelling",
     {L"Typing", L"Ввод", L"Escritura", L"Eingabe", L"Digitação", L"输入"}},
    {L"windowsSettings.gaming", 0, -1, true, 0xe7fc, L"category:gaming-gamemode;SystemSettings_Gaming_GameMode_Toggle_ToggleSwitch", L"settings:SettingsPageGroupGaming2",
     {L"Gaming", L"Игры", L"Juegos", L"Spielen", L"Jogos", L"游戏"}},
    {L"windowsSettings.accessibility", 0, -1, true, 0xe776, L"uri:easeofaccess", L"settings:SettingsPageGroupEaseOfAccess",
     {L"Accessibility", L"Специальные возможности", L"Accesibilidad", L"Barrierefreiheit", L"Acessibilidade", L"辅助功能"}},
    {L"windowsSettings.privacy", 0, -1, true, 0xea18, L"uri:privacy", L"settings:SettingsPageGroupPrivacy",
     {L"Privacy & security", L"Конфиденциальность и защита", L"Privacidad y seguridad", L"Datenschutz und Sicherheit", L"Privacidade e segurança", L"隐私和安全性"}},
    {L"windowsSettings.update", 0, -1, true, 0xe895, L"uri:windowsupdate", L"settings:SettingsPageGroupRestore",
     {L"Windows Update", L"Центр обновления Windows", L"Windows Update", L"Windows Update", L"Windows Update", L"Windows 更新"}},
    {L"controlPanel.security.enabled", 1, -1, true, 0xea18, L"panel:5", L"shell32.dll,-32014",
     {L"System and Security", L"Система и безопасность", L"Sistema y seguridad", L"System und Sicherheit", L"Sistema e Segurança", L"系统和安全"}},
    {L"controlPanel.security.maintenance", 1, 43, true, 0xf760, L"control:Microsoft.ActionCenter", L"shell:{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}",
     {L"Security and Maintenance", L"Центр безопасности и обслуживания", L"Seguridad y mantenimiento", L"Sicherheit und Wartung", L"Segurança e Manutenção", L"安全和维护"}},
    {L"controlPanel.security.firewall", 1, 43, true, 0xe83d, L"exe:Firewall.cpl", L"shell:{4026492F-2F69-46B8-B9BF-5654FC07E423}",
     {L"Windows Defender Firewall", L"Брандмауэр Защитника Windows", L"Firewall de Windows Defender", L"Windows Defender Firewall", L"Windows Defender Firewall", L"Windows Defender 防火墙"}},
    {L"controlPanel.security.powerOptions", 1, 43, true, 0xe7e8, L"exe:powercfg.cpl", L"shell:{025A5937-A6BE-4686-A844-36FE4BEC8B6D}",
     {L"Power Options", L"Электропитание", L"Opciones de energía", L"Energieoptionen", L"Opções de Energia", L"电源选项"}},
    {L"controlPanel.security.bitlocker", 1, 43, true, 0xe72e, L"control:Microsoft.BitLockerDriveEncryption", L"shell:{D9EF8727-CAC2-4E60-809E-86F80A666C91}",
     {L"BitLocker Drive Encryption", L"Шифрование диска BitLocker", L"Cifrado de unidad BitLocker", L"BitLocker-Laufwerkverschlüsselung", L"Criptografia de Unidade de Disco BitLocker", L"BitLocker 驱动器加密"}},
    {L"controlPanel.network.enabled", 1, -1, true, 0xe774, L"panel:3", L"shell32.dll,-32010",
     {L"Network and Internet", L"Сеть и Интернет", L"Redes e Internet", L"Netzwerk und Internet", L"Rede e Internet", L"网络和 Internet"}},
    {L"controlPanel.network.sharing", 1, 48, true, 0xf193, L"control:Microsoft.NetworkAndSharingCenter", L"shell:{8E908FC9-BECC-40F6-915B-F4CA0E70D03D}",
     {L"Network and Sharing Center", L"Центр управления сетями и общим доступом", L"Centro de redes y recursos compartidos", L"Netzwerk- und Freigabecenter", L"Central de Rede e Compartilhamento", L"网络和共享中心"}},
    {L"controlPanel.network.connections", 1, 48, true, 0xe839, L"exe:ncpa.cpl", L"twinui.pcshell.dll,-10912",
     {L"Network Connections", L"Сетевые подключения", L"Conexiones de red", L"Netzwerkverbindungen", L"Conexões de Rede", L"网络连接(W)"}},
    {L"controlPanel.network.internetOptions", 1, 48, true, 0xe774, L"exe:inetcpl.cpl", L"shell:{A3DD4F92-658A-410F-84FD-6FBBBEF2FFFE}",
     {L"Internet Options", L"Свойства браузера", L"Opciones de Internet", L"Internetoptionen", L"Opções da Internet", L"Internet 选项"}},
    {L"controlPanel.hardware.enabled", 1, -1, true, 0xe7f3, L"panel:2", L"shell32.dll,-32008",
     {L"Hardware and Sound", L"Оборудование и звук", L"Hardware y sonido", L"Hardware und Sound", L"Hardware e Sons", L"硬件和声音"}},
    {L"controlPanel.hardware.deviceManager", 1, 52, true, 0xe772, L"exe:mmc.exe;devmgmt.msc", L"twinui.pcshell.dll,-10916",
     {L"Device Manager", L"Диспетчер устройств", L"Administrador de dispositivos", L"Geräte-Manager", L"Gerenciador de Dispositivos", L"设备管理器(M)"}},
    {L"controlPanel.hardware.sound", 1, 52, true, 0xe767, L"exe:mmsys.cpl", L"shell:{F2DDFC82-8F12-4CDD-B7DC-D4FE1425AA4D}",
     {L"Sound", L"Звук", L"Sonido", L"Sound", L"Som", L"声音"}},
    {L"controlPanel.hardware.mouse", 1, 52, true, 0xe962, L"exe:main.cpl", L"shell:{6C8EEC18-8D75-41B2-A177-8831D59D2D50}",
     {L"Mouse", L"Мышь", L"Mouse", L"Maus", L"Mouse", L"鼠标"}},
    {L"controlPanel.hardware.keyboard", 1, 52, true, 0xe765, L"control:keyboard", L"shell:{725BE8F7-668E-4C7B-8F90-46BDB0936430}",
     {L"Keyboard", L"Клавиатура", L"Teclado", L"Tastatur", L"Teclado", L"键盘"}},
    {L"controlPanel.hardware.controllers", 1, 52, true, 0xe7fc, L"exe:joy.cpl", L"joy.cpl,-1076",
     {L"Game Controllers", L"Игровые устройства", L"Dispositivos de juego", L"Gamecontroller", L"Controladores de Jogo", L"游戏控制器"}},
    {L"controlPanel.programs.enabled", 1, -1, true, 0xe71d, L"panel:8", L"shell32.dll,-32006",
     {L"Programs", L"Программы", L"Programas", L"Programme", L"Programas", L"程序"}},
    {L"controlPanel.programs.programsFeatures", 1, 58, true, 0xe71d, L"exe:appwiz.cpl", L"shell:{7B81BE6A-CE2B-4676-A29E-EB907A5126C5}",
     {L"Programs and Features", L"Программы и компоненты", L"Programas y características", L"Programme und Features", L"Programas e Recursos", L"程序和功能"}},
    {L"controlPanel.programs.windowsFeatures", 1, 58, true, 0xe950, L"exe:OptionalFeatures.exe", L"appwiz.cpl,-188",
     {L"Turn Windows features on or off", L"Включение или отключение компонентов Windows", L"Activar o desactivar las características de Windows", L"Windows-Features aktivieren oder deaktivieren", L"Ativar ou desativar recursos do Windows", L"启用或关闭 Windows 功能"}},
    {L"controlPanel.userAccounts.enabled", 1, -1, true, 0xe716, L"panel:9", L"shell32.dll,-32002",
     {L"User Accounts", L"Учетные записи пользователей", L"Cuentas de usuario", L"Benutzerkonten", L"Contas de Usuário", L"用户帐户"}},
    {L"controlPanel.userAccounts.credentials", 1, 61, true, 0xe8d7, L"control:Microsoft.CredentialManager", L"shell:{1206F5F1-0569-412C-8FEC-3204630DFB70}",
     {L"Credential Manager", L"Диспетчер учетных данных", L"Administrador de credenciales", L"Anmeldeinformationsverwaltung", L"Gerenciador de Credenciais", L"凭据管理器"}},
    {L"controlPanel.userAccounts.uac", 1, 61, true, 0xe7ef, L"exe:UserAccountControlSettings.exe", L"shell32.dll,-24317",
     {L"Change User Account Control settings", L"Изменение параметров контроля учетных записей", L"Cambiar configuración de Control de cuentas de usuario", L"Einstellungen der Benutzerkontensteuerung ändern", L"Alterar configurações de Controle de Conta de Usuário", L"更改用户帐户控制设置"}},
    {L"controlPanel.appearance.enabled", 1, -1, true, 0xe771, L"panel:1", L"shell32.dll,-32018",
     {L"Appearance and Personalization", L"Оформление и персонализация", L"Apariencia y personalización", L"Darstellung und Anpassung", L"Aparência e Personalização", L"外观和个性化"}},
    {L"controlPanel.appearance.explorerOptions", 1, 64, true, 0xe8b7, L"control:folders", L"shell:{6DFD7C5C-2451-11D3-A299-00C04F8EF6AF}",
     {L"File Explorer Options", L"Параметры Проводника", L"Opciones del Explorador de archivos", L"Explorer-Optionen", L"Opções do Explorador de Arquivos", L"文件资源管理器选项"}},
    {L"controlPanel.appearance.desktopIcons", 1, 64, true, 0xe7fb, L"exe:rundll32.exe;shell32.dll,Control_RunDLL desk.cpl,,0", L"desk.cpl,-112",
     {L"Desktop Icon Settings", L"Параметры значков рабочего стола", L"Configuración de iconos de escritorio", L"Desktopsymboleinstellungen", L"Configurações dos Ícones da Área de Trabalho", L"桌面图标设置"}},
    {L"controlPanel.appearance.colorManagement", 1, 64, true, 0xe790, L"exe:colorcpl.exe", L"shell:{B2C761C6-29BC-4F19-9251-E6195265BAF1}",
     {L"Color Management", L"Управление цветом", L"Administración del color", L"Farbverwaltung", L"Gerenciamento de Cores", L"颜色管理"}},
    {L"controlPanel.clockRegion.enabled", 1, -1, true, 0xe917, L"panel:6", L"shell32.dll,-32026",
     {L"Clock and Region", L"Часы и регион", L"Reloj y región", L"Zeit und Region", L"Relógio e Região", L"时钟和区域"}},
    {L"controlPanel.clockRegion.dateTime", 1, 68, true, 0xe823, L"exe:timedate.cpl", L"shell:{E2E7934B-DCE5-43C4-9576-7FE4F75E7480}",
     {L"Date and Time", L"Дата и время", L"Fecha y hora", L"Datum und Uhrzeit", L"Data e Hora", L"日期和时间"}},
    {L"controlPanel.clockRegion.region", 1, 68, true, 0xe774, L"exe:intl.cpl", L"shell:{62D8ED13-C9D0-4CE8-A914-47DD628FB1B0}",
     {L"Region", L"Региональные стандарты", L"Región", L"Region", L"Região", L"区域"}},
    {L"controlPanel.accessibility", 1, -1, true, 0xe776, L"panel:7", L"shell32.dll,-32000",
     {L"Ease of Access", L"Специальные возможности", L"Accesibilidad", L"Erleichterte Bedienung", L"Facilidade de Acesso", L"轻松使用"}},
    {L"administration.tools.enabled", 2, -1, true, 0xe90f, L"shell:{D20EA4E1-3957-11d2-A40B-0C5020524153}", L"tools:Windows Tools",
     {L"Windows Tools", L"Инструменты Windows", L"Herramientas de Windows", L"Windows-Tools", L"Ferramentas do Windows", L"Windows 工具"}},
    {L"administration.tools.userCertificates", 2, 72, true, 0xeb95, L"exe:mmc.exe;certmgr.msc", L"certmgr.dll,-246",
     {L"Certificates - Current User", L"Сертификаты - текущий пользователь", L"Certificados: usuario actual", L"Zertifikate - Aktueller Benutzer", L"Certificados - Usuário Atual", L"证书 - 当前用户"}},
    {L"administration.tools.machineCertificates", 2, 72, true, 0x0, L"exe:mmc.exe;certlm.msc", L"certmgr.dll,-193",
     {L"Certificates (Local Computer)", L"Сертификаты (локальный компьютер)", L"Certificados (equipo local)", L"Zertifikate (Lokaler Computer)", L"Certificados (computador local)", L"证书(本地计算机)"}},
    {L"administration.tools.firewallAdvanced", 2, 72, true, 0xe83d, L"exe:mmc.exe;WF.msc", L"tools:Windows Defender Firewall with Advanced Security",
     {L"Windows Defender Firewall with Advanced Security", L"Монитор брандмауэра Защитника Windows в режиме повышенной безопасности", L"Windows Defender Firewall con seguridad avanzada", L"Windows Defender Firewall mit erweiterter Sicherheit", L"Windows Defender Firewall com Segurança Avançada", L"高级安全 Windows Defender 防火墙"}},
    {L"administration.computerManagement.enabled", 2, -1, true, 0xe770, L"native:10918", L"twinui.pcshell.dll,-10918",
     {L"Computer Management", L"Управление компьютером", L"Administración de equipos", L"Computerverwaltung", L"Gerenciamento do Computador", L"计算机管理(G)"}},
    {L"administration.computerManagement.sharedFolders", 2, 76, true, 0xe8b7, L"exe:mmc.exe;fsmgmt.msc", L"filemgmt.dll,-3500",
     {L"Shared Folders", L"Общие папки", L"Carpetas compartidas", L"Freigegebene Ordner", L"Pastas compartilhadas", L"共享文件夹"}},
    {L"administration.computerManagement.localUsers", 2, 76, true, 0xe716, L"exe:mmc.exe;lusrmgr.msc", L"localsec.dll,-5026",
     {L"Local Users and Groups", L"Локальные пользователи и группы", L"Usuarios y grupos locales", L"Lokale Benutzer und Gruppen", L"Usuários e Grupos Locais", L"本地用户和组"}},
    {L"administration.computerManagement.wmi", 2, 76, true, 0x0, L"exe:mmc.exe;WmiMgmt.msc", L"wbem\\wbemcntl.dll,-143",
     {L"WMI Control", L"Управляющий элемент WMI", L"Control WMI", L"WMI-Steuerung", L"Controle WMI", L"WMI 控件"}},
    {L"administration.diskManagement.enabled", 2, -1, true, 0xeda2, L"native:10917", L"twinui.pcshell.dll,-10917",
     {L"Disk Management", L"Управление дисками", L"Administración de discos", L"Datenträgerverwaltung", L"Gerenciamento de Disco", L"磁盘管理(K)"}},
    {L"administration.diskManagement.optimize", 2, 80, true, 0xeda2, L"exe:dfrgui.exe", L"tools:dfrgui",
     {L"Defragment and Optimize Drives", L"Оптимизация дисков", L"Desfragmentar y optimizar unidades", L"Laufwerke defragmentieren und optimieren", L"Desfragmentar e Otimizar Unidades", L"碎片整理和优化驱动器"}},
    {L"administration.diskManagement.cleanup", 2, 80, true, 0xe74d, L"exe:cleanmgr.exe", L"tools:Disk Cleanup",
     {L"Disk Cleanup", L"Очистка диска", L"Liberador de espacio en disco", L"Datenträgerbereinigung", L"Limpeza de Disco", L"磁盘清理"}},
    {L"administration.diskManagement.recoveryDrive", 2, 80, true, 0xe88e, L"exe:RecoveryDrive.exe", L"tools:RecoveryDrive",
     {L"Recovery Drive", L"Диск восстановления", L"Unidad de recuperación", L"Wiederherstellungslaufwerk", L"Unidade de Recuperação", L"恢复驱动器"}},
    {L"administration.services.enabled", 2, -1, true, 0xe713, L"exe:mmc.exe;services.msc", L"tools:services",
     {L"Services", L"Службы", L"Servicios", L"Dienste", L"Serviços", L"服务"}},
    {L"administration.services.taskScheduler", 2, 84, true, 0xe787, L"exe:mmc.exe;taskschd.msc", L"tools:Task Scheduler",
     {L"Task Scheduler", L"Планировщик задач", L"Programador de tareas", L"Aufgabenplanung", L"Agendador de Tarefas", L"任务计划程序"}},
    {L"administration.services.componentServices", 2, 84, true, 0xe950, L"exe:mmc.exe;comexp.msc", L"tools:Component Services",
     {L"Component Services", L"Службы компонентов", L"Servicios de componentes", L"Komponentendienste", L"Serviços de Componentes", L"组件服务"}},
    {L"administration.eventViewer.enabled", 2, -1, true, 0xf0e3, L"native:10914", L"twinui.pcshell.dll,-10914",
     {L"Event Viewer", L"Просмотр событий", L"Visor de eventos", L"Ereignisanzeige", L"Visualizador de Eventos", L"事件查看器(V)"}},
    {L"administration.eventViewer.performance", 2, 87, true, 0xe9d9, L"exe:perfmon.exe", L"tools:Performance Monitor",
     {L"Performance Monitor", L"Системный монитор", L"Monitor de rendimiento", L"Leistungsüberwachung", L"Monitor de Desempenho", L"性能监视器"}},
    {L"administration.eventViewer.resources", 2, 87, true, 0xec49, L"exe:resmon.exe", L"tools:Resource Monitor",
     {L"Resource Monitor", L"Монитор ресурсов", L"Monitor de recursos", L"Ressourcenmonitor", L"Monitor de Recursos", L"资源监视器"}},
    {L"administration.eventViewer.reliability", 2, 87, true, 0xe81c, L"exe:perfmon.exe;/rel", L"werconcpl.dll,-408",
     {L"Reliability Monitor", L"Монитор стабильности системы", L"Monitor de confiabilidad", L"Zuverlässigkeitsüberwachung", L"Monitor de Confiabilidade", L"可靠性监视程序"}},
    {L"administration.eventViewer.systemInfo", 2, 87, true, 0xe946, L"exe:msinfo32.exe", L"tools:System Information",
     {L"System Information", L"Сведения о системе", L"Información del sistema", L"Systeminformationen", L"Informações do sistema", L"系统信息"}},
    {L"administration.eventViewer.memoryDiagnostic", 2, 87, true, 0xeea0, L"exe:MdSched.exe", L"tools:Memory Diagnostics Tool",
     {L"Windows Memory Diagnostic", L"Средство проверки памяти Windows", L"Diagnóstico de memoria de Windows", L"Windows-Speicherdiagnose", L"Diagnóstico de Memória do Windows", L"Windows 内存诊断"}},
    {L"administration.registry.enabled", 2, -1, true, 0x0, L"exe:regedit.exe", L"tools:Registry Editor",
     {L"Registry Editor", L"Редактор реестра", L"Editor del Registro", L"Registrierungs-Editor", L"Editor do Registro", L"注册表编辑器"}},
    {L"administration.registry.systemConfiguration", 2, 93, true, 0xec7a, L"exe:msconfig.exe", L"tools:System Configuration",
     {L"System Configuration", L"Конфигурация системы", L"Configuración del sistema", L"Systemkonfiguration", L"Configuração do Sistema", L"系统配置"}},
    {L"administration.registry.groupPolicy", 2, 93, true, 0xe8a6, L"exe:mmc.exe;gpedit.msc", L"gpedit.dll,-337",
     {L"Local Group Policy Editor", L"Редактор локальной групповой политики", L"Editor de directivas de grupo local", L"Editor für lokale Gruppenrichtlinien", L"Editor de Política de Grupo Local", L"本地组策略编辑器"}},
    {L"administration.registry.localSecurity", 2, 93, true, 0xe72e, L"exe:mmc.exe;secpol.msc", L"tools:Security Configuration Management",
     {L"Local Security Policy", L"Локальная политика безопасности", L"Directiva de seguridad local", L"Lokale Sicherheitsrichtlinie", L"Política de Segurança Local", L"本地安全策略"}},
    {L"administration.terminal.enabled", 2, -1, true, 0xe756, L"native:10960", L"",
     {L"Terminal", L"Терминал", L"Terminal", L"Terminal", L"Terminal", L"终端(I)"}},
    {L"administration.terminal.terminalAdmin", 2, 97, true, 0xe7ef, L"native:10961", L"",
     {L"Terminal (Admin)", L"Терминал (Администратор)", L"Terminal (Administrador)", L"Terminal (Administrator)", L"Terminal (Administrador)", L"终端管理员(A)"}},
    {L"administration.terminal.powershell", 2, 97, true, 0x0, L"exe:WindowsPowerShell/v1.0/powershell.exe", L"twinui.pcshell.dll,-10928",
     {L"Windows PowerShell", L"Windows PowerShell", L"Windows PowerShell", L"Windows PowerShell", L"Windows PowerShell", L"Windows PowerShell(I)"}},
    {L"administration.terminal.commandPrompt", 2, 97, true, 0xe756, L"exe:cmd.exe", L"twinui.pcshell.dll,-10919",
     {L"Command Prompt", L"Командная строка", L"Símbolo del sistema", L"Eingabeaufforderung", L"Prompt de Comando", L"命令提示符(C)"}},
    {L"commands.run", 3, -1, true, 0xe8ad, L"native:10925", L"",
     {L"Run", L"Выполнить", L"Ejecutar", L"Ausführen", L"Executar", L"运行(R)"}},
    {L"commands.taskManager", 3, -1, true, 0xf120, L"native:10921", L"",
     {L"Task Manager", L"Диспетчер задач", L"Administrador de tareas", L"Task-Manager", L"Gerenciador de Tarefas", L"任务管理器(T)"}},
    {L"commands.explorer.enabled", 3, -1, true, 0xec51, L"native:10923", L"",
     {L"File Explorer", L"Проводник", L"Explorador de archivos", L"Explorer", L"Explorador de Arquivos", L"文件资源管理器(E)"}},
    {L"commands.explorer.desktop", 3, 103, true, 0xe7f4, L"folder:B4BFCC3A-DB2C-424C-B029-7FE99A87C641", L"shell32.dll,-21769",
     {L"Desktop", L"Рабочий стол", L"Escritorio", L"Desktop", L"Área de Trabalho", L"桌面"}},
    {L"commands.explorer.computer", 3, 103, true, 0xe770, L"folder:0AC0837C-BBF8-452A-850D-79D08E667CA7", L"windows.storage.dll,-9216",
     {L"This PC", L"Этот компьютер", L"Este equipo", L"Dieser PC", L"Este Computador", L"此电脑"}},
    {L"commands.explorer.personal", 3, 103, true, 0xec25, L"folder:F3CE0F7C-4901-4ACC-8648-D5D44B04EF8F", L"shell32.dll,-22076",
     {L"Personal Folder", L"Личная папка", L"Carpeta personal", L"Persönlicher Ordner", L"Pasta Pessoal", L"个人文件夹"}},
    {L"commands.explorer.documents", 3, 103, false, 0xe8a5, L"folder:FDD39AD0-238F-46AF-ADB4-6C85480369C7", L"windows.storage.dll,-21770",
     {L"Documents", L"Документы", L"Documentos", L"Dokumente", L"Documentos", L"文档"}},
    {L"commands.explorer.pictures", 3, 103, false, 0xe91b, L"folder:33E28130-4E1E-4676-835A-98395C3BC3BB", L"windows.storage.dll,-21779",
     {L"Pictures", L"Изображения", L"Imágenes", L"Bilder", L"Imagens", L"图片"}},
    {L"commands.explorer.downloads", 3, 103, true, 0xe896, L"folder:374DE290-123F-4565-9164-39C4925E467B", L"windows.storage.dll,-21798",
     {L"Downloads", L"Загрузки", L"Descargas", L"Downloads", L"Downloads", L"下载"}},
    {L"commands.explorer.recycleBin", 3, 103, true, 0xe74d, L"folder:B7534046-3ECB-4C18-BE4E-64CD4CB7D6AC", L"shell32.dll,-8964",
     {L"Recycle Bin", L"Корзина", L"Papelera de reciclaje", L"Papierkorb", L"Lixeira", L"回收站"}},
    {L"commands.find", 3, -1, false, 0xe721, L"native:10924", L"",
     {L"Search", L"Найти", L"Buscar", L"Suchen", L"Pesquisar", L"搜索(S)"}},
    {L"commands.desktop", 3, -1, false, 0xe7fb, L"native:10926", L"",
     {L"Desktop", L"Рабочий стол", L"Escritorio", L"Desktop", L"Área de Trabalho", L"桌面(D)"}},
    {L"commands.power.enabled", 3, -1, true, 0xe7e8, L"power:menu", L"",
     {L"Shut down or sign out", L"Завершение работы или выход из системы", L"Apagar o cerrar sesión", L"Herunterfahren oder abmelden", L"Desligar ou sair", L"关机或注销(U)"}},
    {L"commands.power.lock", 3, 113, true, 0xe72e, L"lock:session", L"pri:windows.ui.logon/resources/Lock/Text",
     {L"Lock", L"Заблокировать", L"Bloquear", L"Sperren", L"Bloquear", L"锁定"}},
    {L"commands.power.signOut", 3, 113, true, 0xf3b1, L"power:signOut", L"",
     {L"Sign out", L"Выход", L"Cerrar sesión", L"Abmelden", L"Sair", L"注销"}},
    {L"commands.power.sleep", 3, 113, true, 0xe708, L"power:sleep", L"",
     {L"Sleep", L"Спящий режим", L"Suspender", L"Energie sparen", L"Suspender", L"睡眠"}},
    {L"commands.power.hibernate", 3, 113, true, 0xe823, L"power:hibernate", L"",
     {L"Hibernate", L"Гибернация", L"Hibernar", L"Ruhezustand", L"Hibernar", L"休眠"}},
    {L"commands.power.shutdown", 3, 113, true, 0xe7e8, L"power:shutdown", L"",
     {L"Shut down", L"Завершение работы", L"Apagar", L"Herunterfahren", L"Desligar", L"关机"}},
    {L"commands.power.restart", 3, 113, true, 0xe777, L"power:restart", L"",
     {L"Restart", L"Перезагрузка", L"Reiniciar", L"Neu starten", L"Reiniciar", L"重启"}},
};

enum class HoverMode { Immediate, Delayed, Arrow };
enum class HoverZone { Outside, Body, Arrow };

struct Settings {
    HoverMode mode{};
    std::array<bool, std::size(catalog)> enabled{};
};

struct MenuSession;

struct Destination {
    CatalogItem const* item{};
    C::MenuFlyoutItem original{nullptr};
};

struct RowInteraction {
    MenuSession* owner{};
    Destination destination;
    C::MenuFlyoutSubItem split{nullptr};
    HoverMode mode{};
    C::Grid layout{nullptr};
    X::FrameworkElement chevron{nullptr};
    C::Canvas arrowLayer{nullptr};
    C::Border arrowBackground{nullptr};
    X::VisualStateGroup commonStates{nullptr};
    AV::IExpandCollapseProvider expand{nullptr};
    X::DispatcherTimer hoverUpdate{nullptr};
    HoverZone zone{HoverZone::Outside};
    bool openOnUpdate{};
    std::optional<uint32_t> pressedPointer;
    X::UIElement::PreviewKeyDown_revoker keyDown;
    X::UIElement::PointerPressed_revoker pointerPressed;
    X::UIElement::PointerReleased_revoker pointerReleased;
    X::UIElement::PointerMoved_revoker pointerMoved;
    X::UIElement::PointerEntered_revoker pointerEntered;
    X::UIElement::PointerExited_revoker pointerExited;
    X::UIElement::PointerCanceled_revoker pointerCanceled;
    X::VisualStateGroup::CurrentStateChanged_revoker visualStateChanged;
    X::DispatcherTimer::Tick_revoker tick;
};

struct MenuSession {
    C::MenuFlyout menu{nullptr};
    std::vector<C::MenuFlyoutItemBase> original;
    C::MenuFlyoutSubItem power{nullptr};
    std::vector<C::MenuFlyoutItemBase> originalPower;
    winrt::Windows::Foundation::IInspectable oldPowerIcon{nullptr};
    bool powerChanged{};
    bool menuChanged{};
    std::vector<std::unique_ptr<RowInteraction>> rows;
    std::vector<C::MenuFlyoutItem::Click_revoker> clicks;
    P::FlyoutBase::Opened_revoker opened;
    P::FlyoutBase::Closed_revoker closed;
};

[[clang::no_destroy]] std::unique_ptr<MenuSession> session;
[[clang::no_destroy]] std::array<std::optional<std::wstring>, std::size(catalog)> captionCache;
std::mutex settingsLock;
Settings settings;
std::atomic<bool> unloading;
std::atomic<unsigned> settingsNavigation;
HWND trayWindow;
DWORD trayThread;
UINT setupMessage;
bool subclassPresent;

using ShowAt = HRESULT(WINAPI*)(void*, void*);
using ShowAtOptions = HRESULT(WINAPI*)(void*, void*, void*);
ShowAt originalShowAt;
ShowAtOptions originalShowAtOptions;

template<typename Function>
void OnRow(RowInteraction* row, Function&& function) noexcept {
    if (unloading || !session || session.get() != row->owner) return;
    try {
        function(*row);
    } catch (...) {
        Wh_Log(L"Menu input handler failed");
    }
}

void LoadSettings() {
    Settings next;
    const auto value = Wh_GetStringSetting(L"submenuHover");
    const std::wstring_view option = value ? value : L"";
    next.mode = option == L"delayed" ? HoverMode::Delayed :
                option == L"arrow" ? HoverMode::Arrow : HoverMode::Immediate;
    Wh_FreeStringSetting(value);
    for (size_t i = 0; i < std::size(catalog); ++i)
        next.enabled[i] = Wh_GetIntSetting(catalog[i].key) != 0;
    std::lock_guard guard(settingsLock);
    settings = next;
}

Settings ReadSettings() {
    std::lock_guard guard(settingsLock);
    return settings;
}

std::wstring Normalize(std::wstring_view text) {
    std::wstring normalized;
    for (wchar_t ch : text) {
        if (ch != L'&' && ch != L'\u200e' && ch != L'\u200f')
            normalized.push_back(ch);
    }
    CharLowerBuffW(normalized.data(), static_cast<DWORD>(normalized.size()));
    return normalized;
}

std::wstring SystemPath(std::wstring_view name, bool windowsRoot = false) {
    wchar_t directory[MAX_PATH]{};
    const auto size = windowsRoot ? GetWindowsDirectoryW(directory, ARRAYSIZE(directory)) :
                                    GetSystemDirectoryW(directory, ARRAYSIZE(directory));
    if (!size || size >= ARRAYSIZE(directory)) return {};
    auto path = std::wstring(directory) + L'\\' + std::wstring(name);
    std::replace(path.begin(), path.end(), L'/', L'\\');
    return path;
}

std::wstring Indirect(PCWSTR token) {
    wchar_t result[512]{};
    if (FAILED(SHLoadIndirectString(token, result, ARRAYSIZE(result), nullptr))) return {};
    return result;
}

std::wstring SystemString(std::wstring_view reference) {
    const auto token = L"@%SystemRoot%\\System32\\" + std::wstring(reference);
    auto result = Indirect(token.c_str());
    if (reference.starts_with(L"twinui.pcshell.dll,"))
        std::erase(result, L'&');
    return result;
}

std::wstring Twinui(std::wstring_view id) {
    return SystemString(L"twinui.pcshell.dll,-" + std::wstring(id));
}

size_t DisplayLanguage() {
    wchar_t locale[LOCALE_NAME_MAX_LENGTH]{};
    if (!LCIDToLocaleName(MAKELCID(GetUserDefaultUILanguage(), SORT_DEFAULT), locale, ARRAYSIZE(locale), 0))
        return 0;
    const std::wstring_view code(locale);
    constexpr std::array<std::wstring_view, 6> languages = {L"en", L"ru", L"es", L"de", L"pt", L"zh"};
    for (size_t i = 0; i < languages.size(); ++i)
        if (code.starts_with(languages[i])) return i;
    return 0;
}

struct CoTaskMemory {
    void operator()(void* pointer) const noexcept { CoTaskMemFree(pointer); }
};

using Pidl = std::unique_ptr<ITEMIDLIST, CoTaskMemory>;

Pidl FolderId(std::wstring_view guid) {
    GUID id{};
    const auto literal = L"{" + std::wstring(guid) + L"}";
    if (FAILED(CLSIDFromString(literal.c_str(), &id))) return {};
    PIDLIST_ABSOLUTE result{};
    if (FAILED(SHGetKnownFolderIDList(id, KF_FLAG_DONT_VERIFY, nullptr, &result))) return {};
    return Pidl(result);
}

Pidl ShellId(std::wstring_view name) {
    PIDLIST_ABSOLUTE id{};
    if (FAILED(SHParseDisplayName(std::wstring(name).c_str(), nullptr, &id, 0, nullptr))) return {};
    return Pidl(id);
}

std::wstring ShellName(Pidl const& pidl) {
    PWSTR result{};
    if (!pidl || FAILED(SHGetNameFromIDList(pidl.get(), SIGDN_NORMALDISPLAY, &result))) return {};
    std::unique_ptr<wchar_t, CoTaskMemory> text(result);
    return text.get();
}

std::wstring ControlPath(std::wstring_view category) {
    return L"::{26EE0668-A00A-44D7-9371-BEB064C98683}\\" + std::wstring(category);
}

std::wstring ProgramPath(std::wstring_view program) {
    return SystemPath(program, program == L"regedit.exe");
}

bool Exists(std::wstring const& path) {
    return !path.empty() && GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES;
}

std::pair<std::wstring_view, std::wstring_view> Split(std::wstring_view value, wchar_t divider) {
    const size_t pos = value.find(divider);
    return {value.substr(0, pos), pos == value.npos ? std::wstring_view{} : value.substr(pos + 1)};
}

std::wstring RegistryShellName(std::wstring_view guid) {
    const auto key = L"CLSID\\" + std::wstring(guid);
    wchar_t value[512]{};
    DWORD bytes = sizeof(value);
    if (RegGetValueW(HKEY_CLASSES_ROOT, key.c_str(), L"LocalizedString", RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ,
                     nullptr, value, &bytes) == ERROR_SUCCESS) {
        if (auto name = Indirect(value); !name.empty()) return name;
    }
    return ShellName(ShellId(ControlPath(L"0") + L"\\::" + std::wstring(guid)));
}

std::wstring ToolsName(std::wstring_view shortcut) {
    PWSTR result{};
    if (FAILED(SHGetKnownFolderPath(FOLDERID_CommonAdminTools, KF_FLAG_DONT_VERIFY, nullptr, &result))) return {};
    std::unique_ptr<wchar_t, CoTaskMemory> root(result);
    const auto directory = std::wstring(root.get()) + L"\\";
    const auto fileName = std::wstring(shortcut) + L".lnk";
    const auto ini = directory + L"desktop.ini";
    wchar_t resource[512]{};
    if (GetPrivateProfileStringW(L"LocalizedFileNames", fileName.c_str(), L"", resource,
                                 ARRAYSIZE(resource), ini.c_str())) {
        if (auto title = Indirect(resource); !title.empty()) return title;
    }
    return ShellName(ShellId(directory + fileName));
}

std::wstring SettingsCaption(std::wstring_view id) {
    const auto base = L"@{windows?ms-resource://Windows.UI.SettingsAppThreshold/SystemSettings/Resources/" +
                      std::wstring(id);
    // Windows uses the short title on systems without a battery.
    if (id == L"SettingsPagePowerAndBattery") {
        SYSTEM_POWER_STATUS status{};
        if (GetSystemPowerStatus(&status) && status.BatteryFlag == 128) {
            if (auto title = Indirect((base + L"/AlternateDisplayName}").c_str()); !title.empty()) return title;
        }
    }
    return Indirect((base + L"/DisplayName}").c_str());
}

std::wstring SystemCaption(CatalogItem const& item) {
    const auto [kind, target] = Split(item.action, L':');
    const auto [resourceKind, resource] = Split(item.resource, L':');
    if (resourceKind == L"shell") return RegistryShellName(resource);
    if (resourceKind == L"tools") {
        if (resource == L"Windows Tools") return RegistryShellName(L"{D20EA4E1-3957-11D2-A40B-0C5020524153}");
        return ToolsName(resource);
    }
    if (resourceKind == L"settings") return SettingsCaption(resource);
    if (resourceKind == L"pri") {
        return Indirect((L"@{windows?ms-resource://" + std::wstring(resource) + L"}").c_str());
    }
    if (*item.resource) return SystemString(item.resource);
    if (kind == L"folder") return ShellName(FolderId(target));
    if (kind == L"native") return Twinui(target);
    return {};
}

std::wstring Caption(Destination const& destination) {
    if (destination.original) return std::wstring(destination.original.Text());
    const auto& item = *destination.item;
    auto& cached = captionCache[&item - catalog];
    if (!cached) {
        auto caption = SystemCaption(item);
        if (caption.empty()) caption = item.names[DisplayLanguage()];
        cached = std::move(caption);
    }
    return *cached;
}

template<typename Provider>
Provider GetProvider(X::UIElement const& item, AP::PatternInterface pattern) {
    auto peer = AP::FrameworkElementAutomationPeer::CreatePeerForElement(item);
    return peer ? peer.GetPattern(pattern).try_as<Provider>() : nullptr;
}

bool Launch(PCWSTR target, PCWSTR arguments = nullptr) {
    return reinterpret_cast<INT_PTR>(ShellExecuteW(nullptr, L"open", target, arguments, nullptr, SW_SHOWNORMAL)) > 32;
}

bool LaunchPidl(Pidl const& target) {
    if (!target) return false;
    SHELLEXECUTEINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask = SEE_MASK_IDLIST;
    info.lpIDList = target.get();
    info.nShow = SW_SHOWNORMAL;
    return ShellExecuteExW(&info) != FALSE;
}

struct SettingsRoute {
    HMODULE module{};
    unsigned serial;
    std::wstring pageElementId;
    std::wstring appName;
};

winrt::com_ptr<IUIAutomationCondition> TextCondition(IUIAutomation& automation, PROPERTYID property, PCWSTR text) {
    VARIANT value{};
    value.vt = VT_BSTR;
    value.bstrVal = SysAllocString(text);
    winrt::com_ptr<IUIAutomationCondition> condition;
    if (value.bstrVal) automation.CreatePropertyCondition(property, value, condition.put());
    VariantClear(&value);
    return condition;
}

void CALLBACK SelectSettingsCategory(PTP_CALLBACK_INSTANCE callback, PVOID data) noexcept {
    std::unique_ptr<SettingsRoute> route(static_cast<SettingsRoute*>(data));
    FreeLibraryWhenCallbackReturns(callback, route->module);
    const HRESULT initialized = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(initialized)) return;
    struct Apartment {
        ~Apartment() { CoUninitialize(); }
    } apartment;
    try {
        winrt::com_ptr<IUIAutomation> automation;
        if (FAILED(CoCreateInstance(__uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER,
                                   __uuidof(IUIAutomation), automation.put_void()))) return;
        const auto pageId = TextCondition(*automation, UIA_AutomationIdPropertyId, route->pageElementId.c_str());
        const auto barId = TextCondition(*automation, UIA_AutomationIdPropertyId, L"PermanentNavigationViewBreadcrumbBar");
        const auto crumbType = TextCondition(*automation, UIA_ClassNamePropertyId,
                                            L"Microsoft.UI.Xaml.Controls.BreadcrumbBarItem");
        if (!pageId || !barId || !crumbType) return;

        // Wait for the URI's landing page, then invoke its parent breadcrumb.
        // Automation identifiers are independent of the Windows display language.
        for (int i = 0; i < 24 && !unloading && settingsNavigation == route->serial; ++i) {
            const HWND hwnd = FindWindowW(L"ApplicationFrameWindow", route->appName.c_str());
            winrt::com_ptr<IUIAutomationElement> window, page, bar, parent;
            winrt::com_ptr<IUIAutomationElementArray> crumbs;
            winrt::com_ptr<IUIAutomationInvokePattern> invoke;
            int count{};
            if (hwnd && SUCCEEDED(automation->ElementFromHandle(hwnd, window.put())) && window &&
                SUCCEEDED(window->FindFirst(TreeScope_Descendants, pageId.get(), page.put())) && page &&
                SUCCEEDED(window->FindFirst(TreeScope_Descendants, barId.get(), bar.put())) && bar &&
                SUCCEEDED(bar->FindAll(TreeScope_Descendants, crumbType.get(), crumbs.put())) && crumbs &&
                SUCCEEDED(crumbs->get_Length(&count)) && count == 2 &&
                SUCCEEDED(crumbs->GetElement(0, parent.put())) && parent &&
                SUCCEEDED(parent->GetCurrentPatternAs(UIA_InvokePatternId, __uuidof(IUIAutomationInvokePattern),
                                                     invoke.put_void())) && invoke &&
                settingsNavigation == route->serial && !unloading && SUCCEEDED(invoke->Invoke())) return;
            Sleep(90);
        }
    } catch (...) {
    }
}

void RunDestination(MenuSession& active, Destination destination) {
    ++settingsNavigation;
    if (destination.original) {
        if (auto invoke = GetProvider<AV::IInvokeProvider>(destination.original, AP::PatternInterface::Invoke)) {
            auto menu = active.menu;
            invoke.Invoke();
            menu.Hide();
        }
        return;
    }

    const auto& item = *destination.item;
    const auto [kind, target] = Split(item.action, L':');
    auto menu = active.menu;
    menu.Hide();
    bool opened = false;
    if (kind == L"uri" || kind == L"category") {
        const auto [page, pageElementId] = Split(target, L';');
        const auto address = L"ms-settings:" + std::wstring(page);
        opened = Launch(address.c_str());
        if (opened && kind == L"category") {
            auto route = std::make_unique<SettingsRoute>();
            route->serial = settingsNavigation;
            route->pageElementId = pageElementId;
            route->appName = Twinui(L"10922");
            if (route->appName.empty()) route->appName = L"Settings";
            if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                    reinterpret_cast<PCWSTR>(&SelectSettingsCategory), &route->module)) {
                if (TrySubmitThreadpoolCallback(SelectSettingsCategory, route.get(), nullptr))
                    route.release();
                else
                    FreeLibrary(route->module);
            }
        }
    } else if (kind == L"folder") {
        opened = LaunchPidl(FolderId(target));
    } else if (kind == L"panel" || kind == L"shell") {
        const auto path = kind == L"panel" ? ControlPath(target) : L"::" + std::wstring(target);
        opened = LaunchPidl(ShellId(path));
    } else if (kind == L"lock") {
        opened = LockWorkStation() != FALSE;
    } else if (kind == L"control") {
        const auto command = target.starts_with(L"Microsoft.") ? L"/name " + std::wstring(target) :
                                                                  std::wstring(target);
        opened = Launch(SystemPath(L"control.exe").c_str(), command.c_str());
    } else if (kind == L"exe") {
        const auto [program, argument] = Split(target, L';');
        const auto path = ProgramPath(program);
        auto parameters = std::wstring(argument);
        if (argument.ends_with(L".msc")) parameters = L"\"" + SystemPath(argument) + L"\"";
        if (program.ends_with(L".cpl")) {
            parameters = L"\"" + path + L"\"";
            opened = Launch(SystemPath(L"control.exe").c_str(), parameters.c_str());
        } else {
            opened = Launch(path.c_str(), parameters.empty() ? nullptr : parameters.c_str());
        }
    }
    if (!opened) Wh_Log(L"Could not open Quick Link menu item %s", item.key);
}

X::FrameworkElement FindNamed(X::DependencyObject const& node, std::wstring_view name) {
    auto element = node.try_as<X::FrameworkElement>();
    if (element && element.Name() == name) return element;
    for (int index = 0; index < X::Media::VisualTreeHelper::GetChildrenCount(node); ++index) {
        if (auto child = FindNamed(X::Media::VisualTreeHelper::GetChild(node, index), name)) return child;
    }
    return nullptr;
}

constexpr double arrowHitWidth = 32;

winrt::Windows::Foundation::Point ChevronCenter(RowInteraction const& active, X::UIElement const& relativeTo) {
    return active.chevron.TransformToVisual(relativeTo).TransformPoint({
        static_cast<float>(active.chevron.ActualWidth() / 2.0),
        static_cast<float>(active.chevron.ActualHeight() / 2.0),
    });
}

HoverZone PointerZone(RowInteraction const& active, I::PointerRoutedEventArgs const& args) {
    const auto x = args.GetCurrentPoint(active.layout).Position().X;
    return std::abs(x - ChevronCenter(active, active.layout).X) <= arrowHitWidth / 2.0 ?
        HoverZone::Arrow : HoverZone::Body;
}

void UpdateHoverVisual(RowInteraction const& active) {
    if (!active.arrowBackground) return;
    if (active.zone != HoverZone::Arrow) {
        active.arrowBackground.Opacity(0);
        if (active.zone == HoverZone::Body)
            X::VisualStateManager::GoToState(active.split, L"PointerOver", false);
        return;
    }

    if (!active.arrowBackground.Background()) {
        X::VisualStateManager::GoToState(active.split, L"PointerOver", false);
        active.arrowBackground.Background(active.layout.Background());
    }
    const auto row = active.layout.TransformToVisual(active.arrowLayer).TransformBounds({
        0, 0, static_cast<float>(active.layout.ActualWidth()), static_cast<float>(active.layout.ActualHeight()),
    });
    const double center = ChevronCenter(active, active.arrowLayer).X;
    const double width = (std::min)({static_cast<double>(row.Height),
                                    2 * (center - row.X), 2 * (row.X + row.Width - center)});
    if (width <= 0 || !active.arrowBackground.Background()) return;

    active.arrowBackground.Width(width);
    active.arrowBackground.Height(row.Height);
    C::Canvas::SetLeft(active.arrowBackground, center - width / 2);
    C::Canvas::SetTop(active.arrowBackground, row.Y);
    // Only CommonStates changes here. The native focus visuals stay active.
    X::VisualStateManager::GoToState(active.split, L"Normal", false);
    active.arrowBackground.Opacity(1);
}

void SetHoverZone(RowInteraction& active, HoverZone zone) {
    if (std::exchange(active.zone, zone) == zone) return;
    active.openOnUpdate = active.expand &&
        ((active.mode == HoverMode::Immediate && zone != HoverZone::Outside) ||
         (active.mode == HoverMode::Arrow && zone == HoverZone::Arrow));
    UpdateHoverVisual(active);
    if (active.openOnUpdate) active.hoverUpdate.Start();
}

void AddArrowBackground(RowInteraction& active) {
    auto states = X::VisualStateManager::GetVisualStateGroups(active.layout);
    for (const auto& group : states) {
        if (group.Name() == L"CommonStates") {
            active.commonStates = group;
            break;
        }
    }
    if (!active.commonStates) return;

    // An absolutely positioned child of Canvas cannot grow the menu's grid.
    C::Canvas overlay;
    overlay.IsHitTestVisible(false);
    C::Grid::SetColumnSpan(overlay, (std::max)(1u, active.layout.ColumnDefinitions().Size()));
    C::Grid::SetRowSpan(overlay, (std::max)(1u, active.layout.RowDefinitions().Size()));
    C::Canvas::SetZIndex(overlay, -1);
    C::Border background;
    background.CornerRadius(active.layout.CornerRadius());
    background.Opacity(0);
    overlay.Children().Append(background);
    active.layout.Children().Append(overlay);
    active.arrowLayer = overlay;
    active.arrowBackground = background;
    active.visualStateChanged = active.commonStates.CurrentStateChanged(winrt::auto_revoke,
        [row = &active](auto const&, X::VisualStateChangedEventArgs const& args) {
            OnRow(row, [&](auto& active) {
                if (active.zone == HoverZone::Arrow && args.NewState() &&
                    args.NewState().Name() != L"Normal")
                    active.hoverUpdate.Start();
            });
        });
}

void AttachPointerEvents(RowInteraction& active) {
    active.split.ApplyTemplate();
    active.layout = FindNamed(active.split, L"LayoutRoot").try_as<C::Grid>();
    active.chevron = FindNamed(active.split, L"SubItemChevron");
    if (!active.layout || !active.chevron) return;
    active.expand = GetProvider<AV::IExpandCollapseProvider>(active.split, AP::PatternInterface::ExpandCollapse);
    if (active.mode != HoverMode::Delayed && active.expand) {
        // Let XAML finish routing the enter event before opening its submenu.
        active.hoverUpdate = X::DispatcherTimer();
        active.hoverUpdate.Interval(std::chrono::milliseconds(1));
        active.tick = active.hoverUpdate.Tick(winrt::auto_revoke, [row = &active](auto const&, auto const&) {
            OnRow(row, [](auto& active) {
                active.hoverUpdate.Stop();
                if (std::exchange(active.openOnUpdate, false) &&
                    active.expand.ExpandCollapseState() == X::Automation::ExpandCollapseState::Collapsed)
                    active.expand.Expand();
                UpdateHoverVisual(active);
            });
        });
        if (active.mode == HoverMode::Arrow) AddArrowBackground(active);
    }

    active.pointerEntered = active.layout.PointerEntered(winrt::auto_revoke,
        [row = &active](auto const&, I::PointerRoutedEventArgs const& args) {
            OnRow(row, [&](auto& active) {
                if (args.Pointer().PointerDeviceType() == PointerType::Touch) return;
                // In arrow-only mode we drive the native PointerOver state explicitly
                // because the XAML handler pairs it with starting the submenu timer.
                if (active.mode == HoverMode::Arrow && active.expand) args.Handled(true);
                SetHoverZone(active, PointerZone(active, args));
            });
        });
    active.pointerMoved = active.layout.PointerMoved(winrt::auto_revoke,
        [row = &active](auto const&, I::PointerRoutedEventArgs const& args) {
            OnRow(row, [&](auto& active) {
                if (args.Pointer().PointerDeviceType() != PointerType::Touch)
                    SetHoverZone(active, PointerZone(active, args));
            });
        });
    active.pointerExited = active.layout.PointerExited(winrt::auto_revoke,
        [row = &active](auto const&, auto const&) {
            OnRow(row, [](auto& active) {
                active.pressedPointer.reset();
                SetHoverZone(active, HoverZone::Outside);
            });
        });
    if (!active.destination.item) return;

    active.pointerPressed = active.layout.PointerPressed(winrt::auto_revoke,
        [row = &active](auto const&, I::PointerRoutedEventArgs const& args) {
            OnRow(row, [&](auto& active) {
                if (PointerZone(active, args) == HoverZone::Body &&
                    (args.Pointer().PointerDeviceType() == PointerType::Touch ||
                     args.GetCurrentPoint(active.layout).Properties().IsLeftButtonPressed())) {
                    active.pressedPointer = args.Pointer().PointerId();
                    args.Handled(true);
                }
            });
        });
    active.pointerReleased = active.layout.PointerReleased(winrt::auto_revoke,
        [row = &active](auto const&, I::PointerRoutedEventArgs const& args) {
            OnRow(row, [&](auto& active) {
                const bool pressedHere = std::exchange(active.pressedPointer, std::nullopt) == args.Pointer().PointerId();
                const bool body = PointerZone(active, args) == HoverZone::Body;
                if (pressedHere || body) args.Handled(true);
                if (pressedHere && body) RunDestination(*active.owner, active.destination);
            });
        });
    active.pointerCanceled = active.layout.PointerCanceled(winrt::auto_revoke,
        [row = &active](auto const&, auto const&) {
            OnRow(row, [](auto& active) { active.pressedPointer.reset(); });
        });
}

C::FontIcon MakeIcon(wchar_t glyph) {
    C::FontIcon icon;
    icon.FontFamily(X::Media::FontFamily(L"Segoe Fluent Icons"));
    icon.FontSize(16);
    icon.Glyph(winrt::hstring(std::wstring_view(&glyph, 1)));
    return icon;
}

struct StockMenu {
    std::vector<C::MenuFlyoutItemBase> rows;
    C::MenuFlyoutSubItem power{nullptr};
    std::vector<C::MenuFlyoutItemBase> powerRows;
};

C::MenuFlyoutItem StockAction(StockMenu const& stock, std::wstring_view id) {
    const auto matches = [&](std::wstring_view candidate) -> C::MenuFlyoutItem {
        const auto title = Normalize(Twinui(candidate));
        if (title.empty()) return nullptr;
        for (auto const& item : stock.rows)
            if (auto command = item.try_as<C::MenuFlyoutItem>(); command && Normalize(command.Text()) == title)
                return command;
        return nullptr;
    };
    if (auto action = matches(id)) return action;
    if (id == L"10960") return matches(L"10944");
    if (id == L"10961") return matches(L"10945");
    return nullptr;
}

C::MenuFlyoutItem StockPowerAction(StockMenu const& stock, std::wstring_view action) {
    const auto count = stock.powerRows.size();
    if (count < 3) return nullptr;

    // These commands have fixed places even when the two sleep modes are omitted.
    if (action == L"signOut") return stock.powerRows[0].try_as<C::MenuFlyoutItem>();
    if (action == L"shutdown") return stock.powerRows[count - 2].try_as<C::MenuFlyoutItem>();
    if (action == L"restart") return stock.powerRows[count - 1].try_as<C::MenuFlyoutItem>();
    if (count == 5) return stock.powerRows[action == L"sleep" ? 1 : 2].try_as<C::MenuFlyoutItem>();
    if (count != 4) return nullptr;

    const auto item = stock.powerRows[1].try_as<C::MenuFlyoutItem>();
    const auto expected = SystemString(action == L"sleep" ? L"authui.dll,-3019" : L"authui.dll,-3022");
    return item && !expected.empty() && Normalize(item.Text()) == Normalize(expected) ? item : nullptr;
}

bool ControlRegistered(std::wstring_view resource) {
    const auto [kind, guid] = Split(resource, L':');
    if (kind != L"shell") return true;
    const auto key = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ControlPanel\\NameSpace\\" +
                     std::wstring(guid);
    HKEY handle{};
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, key.c_str(), 0, KEY_READ, &handle) != ERROR_SUCCESS)
        return false;
    RegCloseKey(handle);
    return true;
}

bool ExplorerStartsOnThisPc() {
    DWORD destination{};
    DWORD bytes = sizeof(destination);
    return RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                        L"LaunchTo", RRF_RT_REG_DWORD, nullptr, &destination, &bytes) == ERROR_SUCCESS &&
           destination == 1;
}

bool HasWirelessInterface() {
    MIB_IF_TABLE2* table{};
    if (GetIfTable2(&table) != NO_ERROR) return true;
    const bool found = std::any_of(table->Table, table->Table + table->NumEntries,
                                  [](auto const& entry) { return entry.Type == IF_TYPE_IEEE80211; });
    FreeMibTable(table);
    return found;
}

bool IsHomeEdition() {
    DWORD edition{};
    if (!GetProductInfo(10, 0, 0, 0, &edition)) return false;
    return edition == PRODUCT_CORE || edition == PRODUCT_CORE_N ||
           edition == PRODUCT_CORE_SINGLELANGUAGE || edition == PRODUCT_CORE_COUNTRYSPECIFIC;
}

std::optional<Destination> Resolve(CatalogItem const& item, StockMenu const& stock) {
    auto [kind, target] = Split(item.action, L':');
    Destination destination{&item};
    if (kind == L"native" || kind == L"power") {
        destination.original = kind == L"native" ? StockAction(stock, target) : StockPowerAction(stock, target);
        if (!destination.original || !destination.original.IsEnabled()) return std::nullopt;
    } else if (kind == L"exe") {
        const auto [name, args] = Split(target, L';');
        if (!Exists(ProgramPath(name)) || (args.ends_with(L".msc") && !Exists(SystemPath(args))))
            return std::nullopt;
        if ((args == L"gpedit.msc" || args == L"secpol.msc" || args == L"lusrmgr.msc") && IsHomeEdition())
            return std::nullopt;
    } else if (kind == L"control") {
        if (!ControlRegistered(item.resource) || (target == L"Microsoft.BitLockerDriveEncryption" && IsHomeEdition()))
            return std::nullopt;
    } else if (kind == L"folder") {
        if ((std::wstring_view(item.key) == L"commands.submenus.explorer.computer" && ExplorerStartsOnThisPc()) ||
            !FolderId(target)) return std::nullopt;
    } else if (kind == L"uri") {
        if (target == L"network-wifi" && !HasWirelessInterface()) return std::nullopt;
        if (target == L"devices-keyboard" && SystemCaption(item).empty()) return std::nullopt;
    }
    return destination;
}

void RemoveRowVisual(RowInteraction& row) {
    if (row.hoverUpdate) row.hoverUpdate.Stop();
    if (row.arrowLayer && row.layout) {
        uint32_t index{};
        auto children = row.layout.Children();
        if (children.IndexOf(row.arrowLayer, index)) children.RemoveAt(index);
    }
}

void Restore() noexcept {
    auto old = std::move(session);
    if (!old) return;
    for (auto const& row : old->rows) {
        try { RemoveRowVisual(*row); } catch (...) {}
    }
    try {
        if (old->powerChanged) {
            old->power.Items().ReplaceAll(old->originalPower);
            if (old->oldPowerIcon == X::DependencyProperty::UnsetValue())
                old->power.ClearValue(C::MenuFlyoutSubItem::IconProperty());
            else
                old->power.SetValue(C::MenuFlyoutSubItem::IconProperty(), old->oldPowerIcon);
        }
        if (old->menuChanged) old->menu.Items().ReplaceAll(old->original);
    } catch (...) {
        Wh_Log(L"Could not restore the original Win+X menu");
    }
}

C::MenuFlyoutItem CreateLeaf(MenuSession& pending, Destination const& destination) {
    const auto caption = Caption(destination);
    if (caption.empty()) return nullptr;
    C::MenuFlyoutItem leaf;
    leaf.Text(caption);
    if (destination.item->glyph) leaf.Icon(MakeIcon(destination.item->glyph));
    auto owner = &pending;
    pending.clicks.push_back(leaf.Click(winrt::auto_revoke, [owner, destination](auto const&, auto const&) {
        if (unloading || session.get() != owner) return;
        try { RunDestination(*owner, destination); } catch (...) { Wh_Log(L"Win+X command failed"); }
    }));
    return leaf;
}

void AddInteraction(MenuSession& pending, C::MenuFlyoutSubItem const& split,
                    Destination const& destination, HoverMode mode) {
    auto row = std::make_unique<RowInteraction>();
    row->owner = &pending;
    row->split = split;
    row->destination = destination;
    row->mode = mode;
    if (destination.item) {
        row->keyDown = split.PreviewKeyDown(winrt::auto_revoke,
            [ptr = row.get()](auto const&, I::KeyRoutedEventArgs const& args) {
                if (args.Key() != Key::Enter) return;
                OnRow(ptr, [&](auto& active) {
                    args.Handled(true);
                    RunDestination(*active.owner, active.destination);
                });
            });
    }
    pending.rows.push_back(std::move(row));
}

void TryPatchMenu(C::MenuFlyout const& menu) {
    const auto items = menu.Items();
    if (items.Size() < 10) return;
    const auto snapshot = ReadSettings();

    StockMenu stock;
    const auto powerTitle = Normalize(Twinui(L"10930"));
    if (powerTitle.empty()) return;
    for (auto const& item : items) {
        stock.rows.push_back(item);
        if (auto sub = item.try_as<C::MenuFlyoutSubItem>(); sub && Normalize(sub.Text()) == powerTitle)
            stock.power = sub;
    }
    if (!stock.power || !StockAction(stock, L"10921") || !StockAction(stock, L"10923") ||
        !StockAction(stock, L"10925")) return;
    for (auto const& item : stock.power.Items()) {
        if (item.try_as<C::MenuFlyoutItem>()) stock.powerRows.push_back(item);
    }
    if (stock.powerRows.size() < 3 || stock.powerRows.size() > 5) return;

    auto pending = std::make_unique<MenuSession>();
    pending->menu = menu;
    pending->original = stock.rows;
    pending->power = stock.power;
    for (auto const& item : stock.power.Items()) pending->originalPower.push_back(item);
    pending->oldPowerIcon = stock.power.ReadLocalValue(C::MenuFlyoutSubItem::IconProperty());

    std::vector<C::MenuFlyoutItemBase> visible;
    std::vector<C::MenuFlyoutItemBase> powerChildren;
    wchar_t powerGlyph{};
    int previousGroup = -1;
    for (size_t index = 0; index < std::size(catalog); ++index) {
        const auto& item = catalog[index];
        if (item.parent >= 0 || !snapshot.enabled[index]) continue;
        const bool power = std::wstring_view(item.action) == L"power:menu";
        auto destination = power ? std::optional<Destination>{} : Resolve(item, stock);
        if (!power && !destination) continue;

        std::vector<C::MenuFlyoutItem> children;
        for (size_t child = index + 1; child < std::size(catalog) && catalog[child].parent == static_cast<int>(index); ++child) {
            if (!snapshot.enabled[child]) continue;
            if (const auto resolved = Resolve(catalog[child], stock)) {
                if (auto leaf = CreateLeaf(*pending, *resolved)) children.push_back(leaf);
            }
        }

        C::MenuFlyoutItemBase entry{nullptr};
        if (power) {
            if (children.empty()) continue;
            powerChildren.assign(children.begin(), children.end());
            powerGlyph = item.glyph;
            AddInteraction(*pending, stock.power, {}, snapshot.mode);
            entry = stock.power;
        } else if (children.empty()) {
            entry = CreateLeaf(*pending, *destination);
        } else {
            const auto caption = Caption(*destination);
            if (caption.empty()) continue;
            C::MenuFlyoutSubItem split;
            split.Text(caption);
            if (item.glyph) split.Icon(MakeIcon(item.glyph));
            for (auto const& child : children) split.Items().Append(child);
            AddInteraction(*pending, split, *destination, snapshot.mode);
            entry = split;
        }
        if (!entry) continue;
        if (previousGroup >= 0 && previousGroup != item.group) visible.push_back(C::MenuFlyoutSeparator());
        visible.push_back(entry);
        previousGroup = item.group;
    }
    if (visible.empty()) return;

    pending->opened = menu.Opened(winrt::auto_revoke, [owner = pending.get()](auto const&, auto const&) {
        if (unloading || session.get() != owner) return;
        for (auto const& row : owner->rows)
            OnRow(row.get(), AttachPointerEvents);
    });
    pending->closed = menu.Closed(winrt::auto_revoke, [owner = pending.get()](auto const&, auto const&) {
        if (session.get() == owner) Restore();
    });
    session = std::move(pending);
    if (!powerChildren.empty()) {
        session->powerChanged = true;
        stock.power.Items().ReplaceAll(powerChildren);
        if (powerGlyph) stock.power.Icon(MakeIcon(powerGlyph));
    }
    session->menuChanged = true;
    items.ReplaceAll(visible);
}

template<typename Interface>
void Inspect(void* abi) {
    if (unloading || GetCurrentThreadId() != trayThread || session) return;
    try {
        Interface flyout{nullptr};
        winrt::copy_from_abi(flyout, abi);
        if (auto menu = flyout.template try_as<C::MenuFlyout>()) TryPatchMenu(menu);
    } catch (...) {
        Restore();
        Wh_Log(L"Could not customize the Quick Link menu");
    }
}

HRESULT WINAPI HookShowAt(void* self, void* target) {
    Inspect<P::IFlyoutBase>(self);
    const auto result = originalShowAt(self, target);
    if (FAILED(result) && GetCurrentThreadId() == trayThread) Restore();
    return result;
}

HRESULT WINAPI HookShowAtOptions(void* self, void* target, void* options) {
    Inspect<P::IFlyoutBase5>(self);
    const auto result = originalShowAtOptions(self, target, options);
    if (FAILED(result) && GetCurrentThreadId() == trayThread) Restore();
    return result;
}

bool InstallHooks() {
    C::MenuFlyout sample;
    const auto base = sample.as<P::IFlyoutBase>();
    const auto options = sample.as<P::IFlyoutBase5>();
    // The slots are the two ShowAt signatures in the Windows Runtime interfaces.
    void* show = (*reinterpret_cast<void***>(winrt::get_abi(base)))[14];
    void* showOptions = (*reinterpret_cast<void***>(winrt::get_abi(options)))[12];
    return show != showOptions &&
        Wh_SetFunctionHook(show, reinterpret_cast<void*>(HookShowAt), reinterpret_cast<void**>(&originalShowAt)) &&
        Wh_SetFunctionHook(showOptions, reinterpret_cast<void*>(HookShowAtOptions),
                           reinterpret_cast<void**>(&originalShowAtOptions)) &&
        Wh_ApplyHookOperations();
}

LRESULT CALLBACK TraySubclass(HWND window, UINT message, WPARAM action, LPARAM data, DWORD_PTR) {
    if (message == setupMessage) {
        try {
            if (action) return InstallHooks();
            if (session) {
                auto menu = session->menu;
                menu.Hide();
            }
        } catch (...) {
            Wh_Log(L"Quick Link menu taskbar operation failed");
        }
        if (!action) Restore();
        return 0;
    }
    return DefSubclassProc(window, message, action, data);
}

}  // namespace

BOOL Wh_ModInit() {
    trayWindow = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD pid = 0;
    trayThread = GetWindowThreadProcessId(trayWindow, &pid);
    if (!trayThread || pid != GetCurrentProcessId()) return FALSE;
    setupMessage = RegisterWindowMessageW(L"Windhawk.QuickLinkMenu.Setup");
    LoadSettings();
    return setupMessage != 0;
}

void Wh_ModAfterInit() {
    subclassPresent = WindhawkUtils::SetWindowSubclassFromAnyThread(trayWindow, TraySubclass, 0);
    if (!subclassPresent || !SendMessageW(trayWindow, setupMessage, 1, 0))
        Wh_Log(L"Could not initialize the Quick Link menu hooks");
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModBeforeUninit() {
    unloading = true;
    if (subclassPresent && IsWindow(trayWindow)) {
        SendMessageW(trayWindow, setupMessage, 0, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(trayWindow, TraySubclass);
    }
}
