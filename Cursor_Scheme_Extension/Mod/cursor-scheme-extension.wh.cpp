// ==WindhawkMod==
// @id              cursor-scheme-extension
// @name            Cursor Scheme Extension
// @description     Apply the active Windows cursor scheme to recognized application pointers
// @description:ru-RU Применяет активную схему Windows к распознанным указателям приложений
// @description:es-ES Aplica el esquema de punteros activo de Windows a los punteros reconocidos de las aplicaciones
// @description:de-DE Wendet das aktive Windows-Zeigerschema auf erkannte Anwendungszeiger an
// @description:pt-BR Aplica o esquema de ponteiros ativo do Windows aos ponteiros reconhecidos dos aplicativos
// @description:zh-CN 将当前 Windows 指针方案应用于已识别的应用指针
// @version         0.10.1
// @author          CTravkin
// @github          https://github.com/CTravkin
// @homepage        https://ctravkin.ru
// @license         MIT
// @include         *
// @architecture    x86
// @architecture    x86-64
// @compilerOptions -lgdi32 -ladvapi32 -lshell32
// ==/WindhawkMod==
// Build: 16

// ==WindhawkModReadme==
/*
# Cursor Scheme Extension

Applies the active Windows cursor scheme to recognized application pointers.
The mod covers middle-button autoscroll, move and resize pointers, link
pointers, and several selection and text pointers. Choose the application
families and pointer types independently in Settings. Windows includes Common
Controls and Windows Forms. Chromium includes Electron and compatible embedded
runtimes. Gecko covers cell selection, resize pointers, and vertical text.
Qt covers horizontal and vertical splitters. Classic Office can be enabled
separately. Use an executable name or full path to exclude an application.
Smoothing is available for the autoscroll starting pointer, all eight
autoscroll arrows, and selection arrows.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- targets:
    - windows: true
      $name: Windows
      $name:ru-RU: Windows
      $name:es-ES: Windows
      $name:de-DE: Windows
      $name:pt-BR: Windows
      $name:zh-CN: Windows
      $description: Win32, Common Controls, and Windows Forms
      $description:ru-RU: Win32, Common Controls и Windows Forms
      $description:es-ES: Win32, Common Controls y Windows Forms
      $description:de-DE: Win32, Common Controls und Windows Forms
      $description:pt-BR: Win32, Common Controls e Windows Forms
      $description:zh-CN: Win32、通用控件和 Windows Forms
    - chromium: true
      $name: Chromium
      $name:ru-RU: Chromium
      $name:es-ES: Chromium
      $name:de-DE: Chromium
      $name:pt-BR: Chromium
      $name:zh-CN: Chromium
      $description: Chrome, Edge, Electron, and compatible embedded Chromium runtimes
      $description:ru-RU: Chrome, Edge, Electron и совместимые встроенные движки Chromium
      $description:es-ES: Chrome, Edge, Electron y motores Chromium integrados compatibles
      $description:de-DE: Chrome, Edge, Electron und kompatible eingebettete Chromium-Laufzeiten
      $description:pt-BR: Chrome, Edge, Electron e mecanismos Chromium incorporados compatíveis
      $description:zh-CN: Chrome、Edge、Electron 和兼容的嵌入式 Chromium 运行时
    - gecko: true
      $name: Gecko
      $name:ru-RU: Gecko
      $name:es-ES: Gecko
      $name:de-DE: Gecko
      $name:pt-BR: Gecko
      $name:zh-CN: Gecko
      $description: Firefox and applications with compatible Gecko pointers
      $description:ru-RU: Firefox и приложения с совместимыми указателями Gecko
      $description:es-ES: Firefox y aplicaciones con punteros Gecko compatibles
      $description:de-DE: Firefox und Anwendungen mit kompatiblen Gecko-Zeigern
      $description:pt-BR: Firefox e aplicativos com ponteiros Gecko compatíveis
      $description:zh-CN: Firefox 和使用兼容 Gecko 指针的应用
    - qt: true
      $name: Qt
      $name:ru-RU: Qt
      $name:es-ES: Qt
      $name:de-DE: Qt
      $name:pt-BR: Qt
      $name:zh-CN: Qt
      $description: Native Qt interface splitters
      $description:ru-RU: Разделители панелей в интерфейсе Qt
      $description:es-ES: Divisores de paneles de la interfaz nativa de Qt
      $description:de-DE: Bereichstrenner der nativen Qt-Oberfläche
      $description:pt-BR: Divisores de painéis da interface nativa do Qt
      $description:zh-CN: Qt 原生界面的面板分隔条
    - office: false
      $name: Classic Office
      $name:ru-RU: Классический Office
      $name:es-ES: Office clásico
      $name:de-DE: Klassisches Office
      $name:pt-BR: Office clássico
      $name:zh-CN: 经典版 Office
  $name: Where to replace pointers
  $name:ru-RU: Где подменять указатели
  $name:es-ES: Dónde sustituir los punteros
  $name:de-DE: Wo Zeiger ersetzt werden
  $name:pt-BR: Onde substituir os ponteiros
  $name:zh-CN: 替换指针的应用
- excludedApplications: [""]
  $name: Application exclusions
  $name:ru-RU: Исключения приложений
  $name:es-ES: Exclusiones de aplicaciones
  $name:de-DE: Anwendungsausnahmen
  $name:pt-BR: Exclusões de aplicativos
  $name:zh-CN: 应用排除列表
  $description: One executable name or full path per row
  $description:ru-RU: Одно имя исполняемого файла или полный путь на строку
  $description:es-ES: Un nombre de ejecutable o una ruta completa por fila
  $description:de-DE: Ein Dateiname oder vollständiger Pfad pro Zeile
  $description:pt-BR: Um nome de executável ou caminho completo por linha
  $description:zh-CN: 每行填写一个可执行文件名或完整路径
- cursors:
    - autoscrollOrigin: true
      $name: Scroll origin
      $name:ru-RU: Точка запуска
      $name:es-ES: Punto de inicio
      $name:de-DE: Scrollstart
      $name:pt-BR: Ponto inicial
      $name:zh-CN: 滚动起点
      $description: Pointer shown at the starting point of autoscroll
      $description:ru-RU: Указатель в точке запуска автопрокрутки
      $description:es-ES: Puntero en el punto de inicio del desplazamiento automático
      $description:de-DE: Zeiger am Startpunkt des automatischen Scrollens
      $description:pt-BR: Ponteiro no ponto inicial da rolagem automática
      $description:zh-CN: 自动滚动起点处的指针
    - autoscrollArrows: true
      $name: Scroll arrows
      $name:ru-RU: Стрелки прокрутки
      $name:es-ES: Flechas de desplazamiento
      $name:de-DE: Scrollpfeile
      $name:pt-BR: Setas de rolagem
      $name:zh-CN: 滚动箭头
      $description: Arrows that indicate the autoscroll direction
      $description:ru-RU: Стрелки направления автопрокрутки
      $description:es-ES: Flechas que indican la dirección del desplazamiento automático
      $description:de-DE: Pfeile für die Richtung des automatischen Scrollens
      $description:pt-BR: Setas que indicam a direção da rolagem automática
      $description:zh-CN: 指示自动滚动方向的箭头
    - link: true
      $name: Hand
      $name:ru-RU: Рука
      $name:es-ES: Mano
      $name:de-DE: Hand
      $name:pt-BR: Mão
      $name:zh-CN: 手形指针
      $description: Pointing hand over buttons, links, and other interactive elements
      $description:ru-RU: Указательный палец над кнопками, ссылками и другими активными элементами
      $description:es-ES: Mano sobre botones, enlaces y otros elementos interactivos
      $description:de-DE: Handzeiger über Schaltflächen, Links und anderen interaktiven Elementen
      $description:pt-BR: Mão sobre botões, links e outros elementos interativos
      $description:zh-CN: 按钮、链接和其他交互元素上的手形指针
    - move: true
      $name: Move
      $name:ru-RU: Перемещение
      $name:es-ES: Mover
      $name:de-DE: Verschieben
      $name:pt-BR: Mover
      $name:zh-CN: 移动
      $description: Drag cells, ranges, and objects
      $description:ru-RU: Перетаскивание ячеек, диапазонов и объектов
      $description:es-ES: Arrastrar celdas, rangos y objetos
      $description:de-DE: Zellen, Bereiche und Objekte ziehen
      $description:pt-BR: Arrastar células, intervalos e objetos
      $description:zh-CN: 拖动单元格、区域和对象
    - resize: true
      $name: Resize
      $name:ru-RU: Изменение размеров
      $name:es-ES: Cambiar tamaño
      $name:de-DE: Größe ändern
      $name:pt-BR: Redimensionar
      $name:zh-CN: 调整大小
      $description: Resize panels, rows, columns, and objects
      $description:ru-RU: Изменение размеров панелей, строк, столбцов и объектов
      $description:es-ES: Cambiar el tamaño de paneles, filas, columnas y objetos
      $description:de-DE: Größe von Bereichen, Zeilen, Spalten und Objekten ändern
      $description:pt-BR: Redimensionamento de painéis, linhas, colunas e objetos
      $description:zh-CN: 调整面板、行、列和对象的大小
    - marginSelection: true
      $name: Margin selection
      $name:ru-RU: Выделение с поля
      $name:es-ES: Selección desde el margen
      $name:de-DE: Auswahl am Seitenrand
      $name:pt-BR: Seleção pela margem
      $name:zh-CN: 页边距选取
      $description: Arrow for selecting text from the page margin
      $description:ru-RU: Стрелка выделения текста со стороны поля страницы
      $description:es-ES: Flecha para seleccionar texto desde el margen de la página
      $description:de-DE: Pfeil zur Textauswahl vom Seitenrand aus
      $description:pt-BR: Seta para selecionar texto pela margem da página
      $description:zh-CN: 从页边距选取文本时显示的箭头
    - downSelection: true
      $name: Selection arrows
      $name:ru-RU: Стрелки выделения
      $name:es-ES: Flechas de selección
      $name:de-DE: Auswahlpfeile
      $name:pt-BR: Setas de seleção
      $name:zh-CN: 选择箭头
      $description: Arrows for selecting rows, columns, or the entire table
      $description:ru-RU: Стрелки для выделения строк, столбцов или всей таблицы
      $description:es-ES: Flechas para seleccionar filas, columnas o toda la tabla
      $description:de-DE: Pfeile zur Auswahl von Zeilen, Spalten oder der gesamten Tabelle
      $description:pt-BR: Setas para selecionar linhas, colunas ou a tabela inteira
      $description:zh-CN: 用于选择行、列或整个表格的箭头
    - cell: true
      $name: Cell cross
      $name:ru-RU: Крестик ячейки
      $name:es-ES: Cruz de celda
      $name:de-DE: Zellkreuz
      $name:pt-BR: Cruz de célula
      $name:zh-CN: 单元格十字指针
      $description: Cross-shaped pointer for selecting cells in tables
      $description:ru-RU: Указатель в виде крестика для выбора ячеек в таблицах
      $description:es-ES: Puntero en forma de cruz para seleccionar celdas de una tabla
      $description:de-DE: Kreuzförmiger Zeiger zur Auswahl von Tabellenzellen
      $description:pt-BR: Ponteiro em forma de cruz para selecionar células em tabelas
      $description:zh-CN: 用于选择表格单元格的十字指针
    - verticalText: true
      $name: Vertical text
      $name:ru-RU: Вертикальный текст
      $name:es-ES: Texto vertical
      $name:de-DE: Vertikaler Text
      $name:pt-BR: Texto vertical
      $name:zh-CN: 竖排文本
      $description: Horizontal text pointer for vertically written text
      $description:ru-RU: Горизонтальный текстовый указатель для вертикального текста
      $description:es-ES: Puntero de texto horizontal para texto escrito en vertical
      $description:de-DE: Waagerechter Textzeiger für senkrecht geschriebenen Text
      $description:pt-BR: Ponteiro de texto horizontal para texto escrito na vertical
      $description:zh-CN: 用于竖排文本的水平文本指针
  $name: Which pointers to replace
  $name:ru-RU: Какие указатели подменять
  $name:es-ES: Qué punteros sustituir
  $name:de-DE: Welche Zeiger ersetzt werden
  $name:pt-BR: Quais ponteiros substituir
  $name:zh-CN: 要替换的指针
- smoothing:
    - autoscrollOrigin: true
      $name: Autoscroll starting pointer
      $name:ru-RU: Точка запуска автопрокрутки
      $name:es-ES: Inicio del desplazamiento automático
      $name:de-DE: Startpunkt des automatischen Scrollens
      $name:pt-BR: Início da rolagem automática
      $name:zh-CN: 自动滚动起点
    - autoscrollArrows: true
      $name: Autoscroll arrows
      $name:ru-RU: Стрелки автопрокрутки
      $name:es-ES: Flechas de desplazamiento automático
      $name:de-DE: Pfeile für automatisches Scrollen
      $name:pt-BR: Setas de rolagem automática
      $name:zh-CN: 自动滚动方向箭头
    - selectionArrows: true
      $name: Selection arrows
      $name:ru-RU: Стрелки выделения
      $name:es-ES: Flechas de selección
      $name:de-DE: Auswahlpfeile
      $name:pt-BR: Setas de seleção
      $name:zh-CN: 选择箭头
  $name: Smoothing
  $name:ru-RU: Сглаживание
  $name:es-ES: Suavizado
  $name:de-DE: Kantenglättung
  $name:pt-BR: Suavização
  $name:zh-CN: 边缘平滑
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

enum class Role : size_t {
    Vertical, Horizontal, Move, Up, Arrow, Link, Cross, Text, ResizeNwse, ResizeNesw, Count,
};

enum class CursorType {
    AutoscrollOrigin,
    AutoscrollArrows,
    Link,
    Resize,
    MarginSelection,
    SelectionArrows,
    Cell,
    VerticalText,
    Move,
};

constexpr uint32_t TypeBit(CursorType type) {
    return uint32_t{1} << static_cast<unsigned>(type);
}

struct CursorMapping {
    WORD systemId;  // Zero for selection arrow shapes without a system ID.
    Role role;
    CursorType type;
    unsigned eighthTurns = 0;
    bool mirror = false;
};

constexpr WORD kFirstAutoscrollId = 32652;
constexpr WORD kLastAutoscrollId = 32662;
constexpr WORD kArrowId = 32512;
constexpr WORD kTextId = 32513;
constexpr WORD kCrossId = 32515;
constexpr WORD kNwseResizeId = 32642;
constexpr WORD kNeswResizeId = 32643;
constexpr WORD kHorizontalResizeId = 32644;
constexpr WORD kVerticalResizeId = 32645;
constexpr WORD kMoveId = 32646;
constexpr WORD kLinkId = 32649;
constexpr int kShapeSize = 32;

constexpr std::array kMappings = {
    CursorMapping{32652, Role::Vertical, CursorType::AutoscrollOrigin},
    CursorMapping{32653, Role::Horizontal, CursorType::AutoscrollOrigin},
    CursorMapping{32654, Role::Move, CursorType::AutoscrollOrigin},
    CursorMapping{32655, Role::Up, CursorType::AutoscrollArrows},
    CursorMapping{32656, Role::Up, CursorType::AutoscrollArrows, 4},
    CursorMapping{32657, Role::Up, CursorType::AutoscrollArrows, 6},
    CursorMapping{32658, Role::Up, CursorType::AutoscrollArrows, 2},
    CursorMapping{32659, Role::Up, CursorType::AutoscrollArrows, 7},
    CursorMapping{32660, Role::Up, CursorType::AutoscrollArrows, 1},
    CursorMapping{32661, Role::Up, CursorType::AutoscrollArrows, 5},
    CursorMapping{32662, Role::Up, CursorType::AutoscrollArrows, 3},
    CursorMapping{kArrowId, Role::Arrow, CursorType::MarginSelection, 0, true},
    CursorMapping{kLinkId, Role::Link, CursorType::Link},
    CursorMapping{kHorizontalResizeId, Role::Horizontal, CursorType::Resize},
    CursorMapping{kVerticalResizeId, Role::Vertical, CursorType::Resize},
    CursorMapping{kNwseResizeId, Role::ResizeNwse, CursorType::Resize},
    CursorMapping{kNeswResizeId, Role::ResizeNesw, CursorType::Resize},
    CursorMapping{kMoveId, Role::Move, CursorType::Move},
    CursorMapping{0, Role::Up, CursorType::SelectionArrows, 4},
    CursorMapping{0, Role::Up, CursorType::SelectionArrows, 2},
    CursorMapping{0, Role::Up, CursorType::SelectionArrows, 6},
    CursorMapping{0, Role::Up, CursorType::SelectionArrows, 3},
    CursorMapping{0, Role::Up, CursorType::SelectionArrows, 5},
    CursorMapping{kCrossId, Role::Cross, CursorType::Cell},
    CursorMapping{kTextId, Role::Text, CursorType::VerticalText, 2},
};

struct ResourceMapping {
    WORD id;
    WORD systemId;
};

constexpr std::array kCommonControlsResources = {
    ResourceMapping{109, 32652}, ResourceMapping{110, 32653},
    ResourceMapping{111, 32654}, ResourceMapping{112, 32655},
    ResourceMapping{113, 32656}, ResourceMapping{114, 32658},
    ResourceMapping{115, 32657}, ResourceMapping{116, 32660},
    ResourceMapping{117, 32659}, ResourceMapping{118, 32662},
    ResourceMapping{119, 32661},
};

struct CursorFingerprint {
    uint64_t pixels;
    WORD systemId;
    int hotX = kShapeSize / 2;
    int hotY = kShapeSize / 2;
};

// Chromium variants package the same Windows cursor images under different IDs.
// Keep the color plane as well as the mask to distinguish the three idle states.
constexpr std::array kChromiumCursors = {
    CursorFingerprint{0x357a05e1db72cda9, 32658},  // East
    CursorFingerprint{0x1c43369d8a027071, 32654},  // Both axes
    CursorFingerprint{0xfb403a4219749d01, 32653},  // Horizontal
    CursorFingerprint{0xd2923fde86022921, 32652},  // Vertical
    CursorFingerprint{0x4ca73a7cfff22179, 32655},  // North
    CursorFingerprint{0x7f5820a966f2b7d0, 32660},  // Northeast
    CursorFingerprint{0x44ab733a9503e37e, 32659},  // Northwest
    CursorFingerprint{0x01610cf653deb089, 32656},  // South
    CursorFingerprint{0xf438f4b2379ecc48, 32662},  // Southeast
    CursorFingerprint{0x10ea8f9d7a1c0876, 32661},  // Southwest
    CursorFingerprint{0x7ce71739ad53d0d5, 32657},  // West
    CursorFingerprint{0x702794672c4a6ec5, kHorizontalResizeId, 10, 8},
    CursorFingerprint{0x19747d1629ec5525, kVerticalResizeId, 9, 10},
    CursorFingerprint{0x5d9bc833e83e07c8, kCrossId, 7, 7},
    CursorFingerprint{0xae4a396c7f94f919, kTextId, 9, 3},
};

constexpr std::array kGeckoResources = {
    ResourceMapping{4103, kCrossId}, ResourceMapping{4108, kHorizontalResizeId},
    ResourceMapping{4109, kVerticalResizeId}, ResourceMapping{4110, kTextId},
};

constexpr std::array kWinFormsCursors = {
    CursorFingerprint{0x3d2b81e973cf56e9, 32658, 10, 14},
    CursorFingerprint{0xd93d0fd3350a3d65, kVerticalResizeId, 15, 15},
    CursorFingerprint{0x71eb275f9ab04048, 32660, 12, 18},
    CursorFingerprint{0xaf7c0cde3dde97f9, 32654, 16, 15},
    CursorFingerprint{0x7d323e4f02f00091, 32653, 16, 15},
    CursorFingerprint{0x13f5d35db6fc2c31, 32652, 16, 15},
    CursorFingerprint{0xadbd3ff7cbab4a91, 32655, 15, 20},
    CursorFingerprint{0x0f8885c50070a3a2, 32659, 20, 18},
    CursorFingerprint{0x32816356d17c25b8, 32662, 12, 11},
    CursorFingerprint{0xa968ca07a7fce9e9, 32656, 16, 8},
    CursorFingerprint{0x6d15dc3f672c03c8, 32661, 19, 10},
    CursorFingerprint{0x0c50ddb1fa40c055, kHorizontalResizeId, 15, 15},
    CursorFingerprint{0x46ea5b501abd3e51, 32657, 21, 14},
};

// Some Office sizing handles share an outline, with the direction in the XOR plane.
constexpr std::array kOfficeResizeCursors = {
    CursorFingerprint{0x2689c39224fa1451, kNeswResizeId, 10, 10},
    CursorFingerprint{0xb2b90ccbfdaee413, kNeswResizeId, 9, 9},
    CursorFingerprint{0xe80141f08ed56999, kNwseResizeId, 9, 9},
    CursorFingerprint{0x15fc4d693e6441d7, kNwseResizeId, 9, 9},
};

const std::array kRoleIds = {
    IDC_SIZENS, IDC_SIZEWE, IDC_SIZEALL, IDC_UPARROW, IDC_ARROW, IDC_HAND, IDC_CROSS, IDC_IBEAM,
    IDC_SIZENWSE, IDC_SIZENESW,
};
constexpr size_t kRoleCount = static_cast<size_t>(Role::Count);
constexpr size_t kUnknownSlot = kMappings.size();
constexpr int kMaxSourceSize = 512;

constexpr size_t SystemSlot(WORD id) {
    if (!id) {
        return kUnknownSlot;
    }
    for (size_t i = 0; i < kMappings.size(); ++i) {
        if (kMappings[i].systemId == id) {
            return i;
        }
    }
    return kUnknownSlot;
}

constexpr size_t SelectionSlot(unsigned eighthTurns) {
    for (size_t i = 0; i < kMappings.size(); ++i) {
        const auto& mapping = kMappings[i];
        if (mapping.type == CursorType::SelectionArrows && mapping.eighthTurns == eighthTurns) {
            return i;
        }
    }
    return kUnknownSlot;
}

struct CursorImage {
    int width = 0;
    int height = 0;
    int hotX = 0;
    int hotY = 0;
    // A monochrome cursor keeps the XOR plane below its AND plane.
    std::vector<uint32_t> mask;
    std::vector<uint32_t> color;
    std::vector<uint32_t> smoothPixels;

    bool operator==(const CursorImage&) const = default;
};

struct IconBitmaps : ICONINFO {
    IconBitmaps() : ICONINFO{} {}
    ~IconBitmaps() {
        if (hbmMask) {
            DeleteObject(hbmMask);
        }
        if (hbmColor) {
            DeleteObject(hbmColor);
        }
    }
};

struct MemoryDc {
    HDC value = CreateCompatibleDC(nullptr);

    ~MemoryDc() {
        if (value) {
            DeleteDC(value);
        }
    }
};

struct Replacement {
    HCURSOR cursor = nullptr;
    HCURSOR original = nullptr;
    bool smooth = false;
};

struct ThreadSelection {
    DWORD id;
    HANDLE lifetime;
    HWND window = nullptr;
    HCURSOR cursor = nullptr;
};

struct CursorShape {
    int width = 0;
    int height = 0;
    std::array<uint32_t, kShapeSize> rows{};

    bool operator==(const CursorShape&) const = default;
};

// Cropped silhouettes of shared Office pointers, independent of resource IDs.
constexpr std::array kOfficeShapes = {
    std::pair{SystemSlot(kArrowId), CursorShape{12, 20, {
        0x0c00, 0x0e00, 0x0f00, 0x0f80, 0x0fc0, 0x0fe0, 0x0ff0, 0x0ff8,
        0x0ffc, 0x0ffe, 0x0fff, 0x0ff0, 0x0ff0, 0x0e78, 0x0c78, 0x083c,
        0x003c, 0x001e, 0x001e, 0x000c,
    }}},
    std::pair{SystemSlot(kLinkId), CursorShape{17, 22, {
        0x0060, 0x00f0, 0x00f0, 0x00f0, 0x00f0, 0x03f0, 0x1ff0, 0x7ff0,
        0xfff0, 0x1fff7, 0x1ffff, 0x1ffff, 0x1fffe, 0x1fffc, 0x1fffc, 0x1fff8,
        0xfff8, 0xfff0, 0xfff0, 0x7fe0, 0x7fe0, 0x7fe0,
    }}},
    std::pair{SelectionSlot(4), CursorShape{9, 12, {
        0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x1ff, 0x1ff,
        0xfe, 0x7c, 0x38, 0x10,
    }}},
    std::pair{SelectionSlot(2), CursorShape{12, 9, {
        0xc0, 0x1c0, 0x3ff, 0x7ff, 0xfff, 0x7ff, 0x3ff, 0x1c0, 0xc0,
    }}},
    std::pair{SelectionSlot(6), CursorShape{12, 9, {
        0x30, 0x38, 0xffc, 0xffe, 0xfff, 0xffe, 0xffc, 0x38, 0x30,
    }}},
    std::pair{SelectionSlot(3), CursorShape{13, 13, {
        0x8, 0x1c, 0x3e, 0x7f, 0xfe, 0x19fc, 0x1ff8, 0x1ff0,
        0x1fe0, 0x1fc0, 0x1fc0, 0x1fe0, 0x1fe0,
    }}},
    std::pair{SelectionSlot(5), CursorShape{10, 10, {
        0x80, 0x1c0, 0x3e0, 0x1f3, 0xff, 0x7f, 0x3f, 0x3f, 0x7f, 0x7f,
    }}},
    std::pair{SystemSlot(kCrossId), CursorShape{15, 15, {
        0x3f0, 0x7f0, 0x7f0, 0x7f0, 0x3fff, 0x7fff, 0x7fff, 0x7fff,
        0x7fff, 0x7fff, 0x7ffe, 0x7f0, 0x7f0, 0x7f0, 0x7e0,
    }}},
    std::pair{SystemSlot(kMoveId), CursorShape{17, 15, {
        0x100, 0x380, 0x7c0, 0xfe0, 0x27c8, 0x739c, 0xfffe, 0x1ffff,
        0xfffe, 0x739c, 0x27c8, 0xfe0, 0x7c0, 0x380, 0x100,
    }}},
    std::pair{SystemSlot(kMoveId), CursorShape{18, 18, {
        0x300, 0x780, 0xfc0, 0x1fe0, 0x1fe0, 0x6798, 0xe79c, 0x1fffe,
        0x3ffff, 0x3ffff, 0x1fffe, 0xe79c, 0x6798, 0x1fe0, 0x1fe0, 0xfc0,
        0x780, 0x300,
    }}},
    std::pair{SystemSlot(kMoveId), CursorShape{19, 19, {
        0x200, 0x700, 0xf80, 0x1fc0, 0x1fc0, 0x700, 0xc718, 0x1c71c,
        0x3fafe, 0x7ffff, 0x3fafe, 0x1c71c, 0xc718, 0x700, 0x1fc0, 0x1fc0,
        0xf80, 0x700, 0x200,
    }}},
    std::pair{SystemSlot(kMoveId), CursorShape{20, 28, {
        0x200, 0x700, 0xf80, 0x1fc0, 0x1fc0, 0x700, 0xc718, 0x1c71c,
        0x3f8fe, 0x7faff, 0x3fefe, 0x1ce1c, 0xde18, 0x3e00, 0x7fc0, 0xffc0,
        0x1ff80, 0x3ff00, 0x7fe00, 0xffe00, 0xfe00, 0x1ee00, 0x1e600, 0x3c200,
        0x3c000, 0x78000, 0x78000, 0x70000,
    }}},
    std::pair{SystemSlot(kNwseResizeId), CursorShape{13, 13, {
        0x7f, 0x7f, 0x3f, 0x1f, 0x3f, 0x77, 0x18e3, 0x1dc0,
        0x1f80, 0x1f00, 0x1f80, 0x1fc0, 0x1fc0,
    }}},
    std::pair{SystemSlot(kNeswResizeId), CursorShape{13, 13, {
        0x1fc0, 0x1fc0, 0x1f80, 0x1f00, 0x1f80, 0x1dc0, 0x18e3, 0x77,
        0x3f, 0x1f, 0x3f, 0x7f, 0x7f,
    }}},
    std::pair{SystemSlot(kHorizontalResizeId), CursorShape{17, 15, {
        0x6c0, 0x6c0, 0x6c0, 0x6c0, 0x36d8, 0x76dc, 0xfefe, 0x1feff,
        0xfefe, 0x76dc, 0x36d8, 0x6c0, 0x6c0, 0x6c0, 0x6c0,
    }}},
    std::pair{SystemSlot(kVerticalResizeId), CursorShape{15, 17, {
        0x80, 0x1c0, 0x3e0, 0x7f0, 0x7f0, 0x1c0, 0x7fff, 0x7fff,
        0x0, 0x7fff, 0x7fff, 0x1c0, 0x7f0, 0x7f0, 0x3e0, 0x1c0, 0x80,
    }}},
    // Worksheet headers have variants with different separator thicknesses.
    std::pair{SystemSlot(kHorizontalResizeId), CursorShape{18, 18, {
        0x780, 0x780, 0x780, 0x780, 0x780, 0x6798, 0xe79c, 0x1fffe,
        0x3ffff, 0x3ffff, 0x1fffe, 0xe79c, 0x6798, 0x780, 0x780, 0x780,
        0x780, 0x780,
    }}},
    std::pair{SystemSlot(kVerticalResizeId), CursorShape{18, 18, {
        0x300, 0x780, 0xfc0, 0x1fe0, 0x1fe0, 0x780, 0x780, 0x3ffff,
        0x3ffff, 0x3ffff, 0x3ffff, 0x780, 0x780, 0x1fe0, 0x1fe0, 0xfc0,
        0x780, 0x300,
    }}},
    std::pair{SystemSlot(kHorizontalResizeId), CursorShape{18, 18, {
        0xfc0, 0xfc0, 0xfc0, 0xfc0, 0xfc0, 0x6fd8, 0xefdc, 0x1fffe,
        0x3ffff, 0x3ffff, 0x1fffe, 0xefdc, 0x6fd8, 0xfc0, 0xfc0, 0xfc0,
        0xfc0, 0xfc0,
    }}},
    std::pair{SystemSlot(kVerticalResizeId), CursorShape{18, 18, {
        0x300, 0x780, 0xfc0, 0x1fe0, 0x1fe0, 0x780, 0x3ffff, 0x3ffff,
        0x3ffff, 0x3ffff, 0x3ffff, 0x3ffff, 0x780, 0x1fe0, 0x1fe0, 0xfc0,
        0x780, 0x300,
    }}},
};

enum class Target { Windows, Chromium, Gecko, Qt, Office };

struct CursorMatch {
    size_t slot = kUnknownSlot;
    Target target = Target::Windows;
};

std::array<CursorShape, kLastAutoscrollId - kFirstAutoscrollId + 1> g_referenceShapes;
std::array<Replacement, kMappings.size()> g_replacements;
std::array<std::optional<CursorImage>, kRoleCount> g_sources;
std::array<bool, kRoleCount> g_needsRefresh;
std::vector<Replacement> g_retired;
std::vector<ThreadSelection> g_selections;
std::mutex g_mutex;
std::once_flag g_referenceShapesInit;
std::atomic<bool> g_needsCleanup;
std::atomic<uint32_t> g_enabledTargets;
std::atomic<uint32_t> g_enabledTypes;
std::atomic<bool> g_isOffice;
std::atomic<bool> g_processExcluded;
std::vector<std::wstring> g_excludedApplications;
uint32_t g_smoothingTypes;
bool g_stopping;
bool g_watchStarted;
HKEY g_cursorsKey;
HANDLE g_changeEvent;
thread_local Role g_previousRole = Role::Count;
decltype(&SetCursor) g_setCursorOriginal;

template <size_t N>
size_t ResourceSlot(WORD id, const std::array<ResourceMapping, N>& resources) {
    for (const auto& resource : resources) {
        if (resource.id == id) {
            return SystemSlot(resource.systemId);
        }
    }
    return kUnknownSlot;
}

bool HasModuleName(std::wstring_view path, std::wstring_view module) {
    const auto slash = path.find_last_of(L"\\/");
    if (slash != path.npos) {
        path.remove_prefix(slash + 1);
    }
    return path.size() == module.size() &&
           CompareStringOrdinal(path.data(), static_cast<int>(path.size()),
                                module.data(), static_cast<int>(module.size()),
                                TRUE) == CSTR_EQUAL;
}

bool IsChromiumChildProcess() {
    const auto commandLine = GetCommandLineW();
    if (!std::wstring_view(commandLine).contains(L"--type=")) {
        return false;
    }
    int argumentCount = 0;
    LPWSTR* arguments = CommandLineToArgvW(commandLine, &argumentCount);
    if (!arguments) {
        return false;
    }
    bool type = false;
    bool chromiumHandle = false;
    for (int i = 1; i < argumentCount; ++i) {
        const std::wstring_view argument(arguments[i]);
        type |= argument == L"--type=renderer" || argument == L"--type=gpu-process" ||
                argument == L"--type=utility";
        chromiumHandle |= argument.starts_with(L"--field-trial-handle=") ||
                          argument.starts_with(L"--mojo-platform-channel-handle=");
    }
    LocalFree(arguments);
    return type && chromiumHandle;
}

bool ObserveOffice() {
    if (g_isOffice.load(std::memory_order_relaxed)) {
        return true;
    }
    // The common Office runtime can be loaded after the mod starts.
    if (GetModuleHandleW(L"mso20win32client.dll")) {
        g_isOffice.store(true, std::memory_order_relaxed);
        return true;
    }
    return false;
}

BITMAPINFO ColorBitmapInfo(int width, int height) {
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    return info;
}

bool ReadBitmap(HDC dc,
                HBITMAP bitmap,
                int width,
                int height,
                std::vector<uint32_t>& pixels) {
    pixels.resize(size_t(width) * height);
    auto info = ColorBitmapInfo(width, height);
    return GetDIBits(dc, bitmap, 0, height, pixels.data(), &info,
                     DIB_RGB_COLORS) == height;
}

std::vector<uint32_t> DrawAlphaCursor(HCURSOR cursor, HDC dc, int width, int height) {
    auto info = ColorBitmapInfo(width, height);
    void* pixels = nullptr;
    IconBitmaps bitmap;
    bitmap.hbmColor = CreateDIBSection(dc, &info, DIB_RGB_COLORS, &pixels, nullptr, 0);
    if (!bitmap.hbmColor || !pixels) {
        return {};
    }

    const size_t count = size_t(width) * height;
    std::vector<uint32_t> result(count);
    HGDIOBJ previous = SelectObject(dc, bitmap.hbmColor);
    if (!previous || previous == HGDI_ERROR) {
        return {};
    }
    std::memset(pixels, 0, count * sizeof(uint32_t));
    const bool drawn = DrawIconEx(dc, 0, 0, cursor, width, height, 0,
                                 nullptr, DI_NORMAL | DI_NOMIRROR);
    GdiFlush();
    SelectObject(dc, previous);
    if (!drawn) {
        return {};
    }
    // Drawing into a transparent surface gives premultiplied BGRA.
    std::memcpy(result.data(), pixels, count * sizeof(uint32_t));
    return result;
}

std::vector<uint32_t> PrepareSmoothPixels(HCURSOR cursor, HDC dc,
                                        const CursorImage& image) {
    if (std::any_of(image.color.begin(), image.color.end(),
                    [](uint32_t pixel) { return pixel >> 24; })) {
        return DrawAlphaCursor(cursor, dc, image.width, image.height);
    }

    const size_t count = size_t(image.width) * image.height;
    std::vector<uint32_t> result(count);
    for (size_t i = 0; i < count; ++i) {
        const uint32_t xorPixel = (image.color.empty() ? image.mask[count + i]
                                                      : image.color[i]) & 0x00FFFFFF;
        if (image.mask[i] & 0x00FFFFFF) {
            if (xorPixel) {
                // An inverted background cannot be represented with alpha blending.
                return {};
            }
        } else {
            result[i] = 0xFF000000 | xorPixel;
        }
    }
    return result;
}

std::optional<CursorImage> ReadCursorBitmaps(HCURSOR cursor, const ICONINFO& info,
                                            bool prepareSmoothing, int maxSize) {
    if (info.fIcon || !info.hbmMask) {
        return std::nullopt;
    }

    BITMAP mask{};
    if (!GetObjectW(info.hbmMask, sizeof(mask), &mask)) {
        return std::nullopt;
    }

    CursorImage image;
    image.width = mask.bmWidth;
    image.height = info.hbmColor ? mask.bmHeight : mask.bmHeight / 2;
    if (image.width < 1 || image.width > maxSize || image.height < 1 ||
        image.height > maxSize || info.xHotspot >= DWORD(image.width) ||
        info.yHotspot >= DWORD(image.height) ||
        (!info.hbmColor && mask.bmHeight != 2 * image.height)) {
        return std::nullopt;
    }

    image.hotX = static_cast<int>(info.xHotspot);
    image.hotY = static_cast<int>(info.yHotspot);
    MemoryDc dc;
    if (!dc.value || !ReadBitmap(dc.value, info.hbmMask, image.width,
                                 mask.bmHeight, image.mask)) {
        return std::nullopt;
    }

    if (info.hbmColor) {
        BITMAP color{};
        if (!GetObjectW(info.hbmColor, sizeof(color), &color) ||
            color.bmWidth != image.width || color.bmHeight != image.height ||
            !ReadBitmap(dc.value, info.hbmColor, image.width, image.height,
                        image.color)) {
            return std::nullopt;
        }
    }

    if (prepareSmoothing) {
        image.smoothPixels = PrepareSmoothPixels(cursor, dc.value, image);
    }
    return image;
}

std::optional<CursorImage> ReadCursor(HCURSOR cursor, bool prepareSmoothing,
                                     int maxSize = kMaxSourceSize) {
    IconBitmaps info;
    if (!cursor || !GetIconInfo(cursor, &info)) {
        return std::nullopt;
    }
    return ReadCursorBitmaps(cursor, info, prepareSmoothing, maxSize);
}

CursorShape MakeShape(const CursorImage& image) {
    const size_t plane = size_t(image.width) * image.height;
    const bool hasAlpha = std::any_of(image.color.begin(), image.color.end(),
                                     [](uint32_t pixel) { return pixel >> 24; });
    const auto visible = [&](int x, int y) {
        const size_t i = size_t(y) * image.width + x;
        const uint32_t pixel = image.color.empty() ? image.mask[plane + i]
                                                  : image.color[i];
        if (hasAlpha) {
            return (pixel >> 24) >= 128;
        }
        return !(image.mask[i] & 0x00FFFFFF) || (pixel & 0x00FFFFFF) != 0;
    };

    int left = image.width, right = -1;
    int top = image.height, bottom = -1;
    for (int y = 0; y < image.height; ++y) {
        for (int x = 0; x < image.width; ++x) {
            if (visible(x, y)) {
                left = (std::min)(left, x);
                right = (std::max)(right, x);
                top = (std::min)(top, y);
                bottom = (std::max)(bottom, y);
            }
        }
    }
    if (right < 0 || right - left >= kShapeSize || bottom - top >= kShapeSize) {
        return {};
    }

    // Office editions can pad the same figure differently or move its hotspot.
    CursorShape shape{right - left + 1, bottom - top + 1};
    for (int y = top; y <= bottom; ++y) {
        for (int x = left; x <= right; ++x) {
            if (visible(x, y)) {
                shape.rows[y - top] |= uint32_t{1} << (x - left);
            }
        }
    }
    return shape;
}

std::optional<CursorImage> LoadResourceImage(HMODULE module, PCWSTR name) {
    const auto cursor = static_cast<HCURSOR>(LoadImageW(module, name, IMAGE_CURSOR,
                                                       kShapeSize, kShapeSize, 0));
    if (!cursor) {
        return std::nullopt;
    }

    std::optional<CursorImage> image;
    try {
        image = ReadCursor(cursor, false, kShapeSize);
    } catch (...) {
        DestroyCursor(cursor);
        throw;
    }
    DestroyCursor(cursor);
    return image;
}

CursorShape LoadResourceShape(HMODULE module, PCWSTR name) {
    const auto image = LoadResourceImage(module, name);
    return image ? MakeShape(*image) : CursorShape{};
}

size_t FindOfficeSlot(const CursorShape& shape) {
    if (!shape.width) {
        return kUnknownSlot;
    }
    for (const auto& [slot, reference] : kOfficeShapes) {
        if (reference == shape) {
            return slot;
        }
    }
    std::call_once(g_referenceShapesInit, [] {
        for (WORD id = kFirstAutoscrollId; id <= kLastAutoscrollId; ++id) {
            g_referenceShapes[id - kFirstAutoscrollId] =
                LoadResourceShape(nullptr, MAKEINTRESOURCEW(id));
        }
    });

    for (size_t i = 0; i < g_referenceShapes.size(); ++i) {
        if (g_referenceShapes[i] == shape) {
            return SystemSlot(static_cast<WORD>(kFirstAutoscrollId + i));
        }
    }
    return kUnknownSlot;
}

template <size_t N>
size_t FindFingerprintSlot(const CursorImage& image,
                           const std::array<CursorFingerprint, N>& fingerprints) {
    if (image.width != kShapeSize || image.height != kShapeSize) {
        return kUnknownSlot;
    }
    const auto matchesHotspot = [&](const CursorFingerprint& known) {
        return known.hotX == image.hotX && known.hotY == image.hotY;
    };
    if (std::none_of(fingerprints.begin(), fingerprints.end(), matchesHotspot)) {
        return kUnknownSlot;
    }
    uint64_t hash = 0xcbf29ce484222325;
    for (const auto* plane : {&image.mask, &image.color}) {
        for (uint32_t pixel : *plane) {
            hash = (hash ^ pixel) * 0x100000001b3;
        }
    }
    for (const auto& known : fingerprints) {
        if (known.pixels == hash && matchesHotspot(known)) {
            return SystemSlot(known.systemId);
        }
    }
    return kUnknownSlot;
}

size_t FindQtSplitterSlot(const CursorImage& image) {
    struct Splitter {
        int size;
        int hotX;
        int hotY;
        uint64_t mask;
        WORD systemId;
    };
    // Qt creates these handles from PNGs, so GetIconInfoEx has no resource ID.
    constexpr std::array splitters = {
        Splitter{32, 11, 11, 0xce3de8a32fb52cae, kVerticalResizeId},
        Splitter{48, 16, 17, 0xb08f48e137820f25, kVerticalResizeId},
        Splitter{64, 22, 22, 0x43f54dc870c43aa9, kVerticalResizeId},
        Splitter{32, 11, 11, 0x5ca6f5b51c206092, kHorizontalResizeId},
        Splitter{48, 16, 17, 0xf0c2ba92e0955965, kHorizontalResizeId},
        Splitter{64, 22, 22, 0x55eb77441590bd35, kHorizontalResizeId},
    };
    if (image.color.empty() || image.width != image.height) {
        return kUnknownSlot;
    }
    uint64_t mask = 0xcbf29ce484222325;
    for (uint32_t pixel : image.color) {
        mask = (mask ^ ((pixel >> 24) >= 128)) * 0x100000001b3;
    }
    for (const auto& splitter : splitters) {
        if (splitter.size == image.width && splitter.hotX == image.hotX &&
            splitter.hotY == image.hotY && splitter.mask == mask) {
            return SystemSlot(splitter.systemId);
        }
    }
    return kUnknownSlot;
}

BOOL CALLBACK FindChromiumPanningResource(HMODULE module, LPCWSTR, LPWSTR name, LONG_PTR data) {
    auto& found = *reinterpret_cast<bool*>(data);
    try {
        const auto image = LoadResourceImage(module, name);
        if (image) {
            const auto slot = FindFingerprintSlot(*image, kChromiumCursors);
            if (slot != kUnknownSlot && kMappings[slot].systemId >= kFirstAutoscrollId) {
                found = true;
                return FALSE;
            }
        }
        return TRUE;
    } catch (...) {
        return FALSE;
    }
}

bool HasChromiumPanningResources(PCWSTR name) {
    // Resolve an ambiguous image in a mixed Office/Chromium process once per source.
    thread_local std::wstring moduleName;
    thread_local bool matches;
    if (moduleName != name) {
        moduleName = name;
        matches = false;
        HMODULE module = nullptr;
        if (*name && GetModuleHandleExW(0, name, &module)) {
            EnumResourceNamesW(module, MAKEINTRESOURCEW(12), FindChromiumPanningResource,
                               reinterpret_cast<LONG_PTR>(&matches));
            FreeLibrary(module);
        }
    }
    return matches;
}

CursorMatch ReadResourceSlot(HCURSOR cursor) {
    const bool office = ObserveOffice();
    ICONINFOEXW info{sizeof(info)};
    if (!GetIconInfoExW(cursor, &info)) {
        return {};
    }
    IconBitmaps bitmaps;
    bitmaps.fIcon = info.fIcon;
    bitmaps.xHotspot = info.xHotspot;
    bitmaps.yHotspot = info.yHotspot;
    bitmaps.hbmMask = info.hbmMask;
    bitmaps.hbmColor = info.hbmColor;

    if (info.fIcon) {
        return {};
    }
    if (HasModuleName(info.szModName, L"USER32") ||
        HasModuleName(info.szModName, L"user32.dll")) {
        // Native move and resize handles already use the active Windows scheme.
        return info.wResID >= kFirstAutoscrollId && info.wResID <= kLastAutoscrollId
            ? CursorMatch{SystemSlot(info.wResID), office ? Target::Office : Target::Windows}
            : CursorMatch{};
    }
    if (HasModuleName(info.szModName, L"comctl32.dll")) {
        return {ResourceSlot(info.wResID, kCommonControlsResources),
                office ? Target::Office : Target::Windows};
    }
    const bool named = *info.szModName;
    const bool gecko = HasModuleName(info.szModName, L"xul.dll");
    const size_t geckoSlot = gecko ? ResourceSlot(info.wResID, kGeckoResources) : kUnknownSlot;
    if (gecko && geckoSlot == kUnknownSlot) {
        return {};
    }
    const bool qtCandidate = !named && bitmaps.hbmColor &&
        (info.xHotspot == 11 || info.xHotspot == 16 || info.xHotspot == 22);
    const bool winForms = !named &&
        (GetModuleHandleW(L"clr.dll") || GetModuleHandleW(L"coreclr.dll"));
    if (!named && !office && !qtCandidate && !winForms) {
        return {};
    }

    const auto findSlot = [&](const CursorImage& image) -> CursorMatch {
        const auto chromiumSlot = named ? FindFingerprintSlot(image, kChromiumCursors)
                                        : kUnknownSlot;
        if (gecko) {
            return {chromiumSlot == geckoSlot ? geckoSlot : kUnknownSlot, Target::Gecko};
        }
        if (qtCandidate) {
            const auto qtSlot = FindQtSplitterSlot(image);
            if (qtSlot != kUnknownSlot) {
                return {qtSlot, Target::Qt};
            }
        }
        if (winForms) {
            const auto formsSlot = FindFingerprintSlot(image, kWinFormsCursors);
            if (formsSlot != kUnknownSlot) {
                return {formsSlot, Target::Windows};
            }
        }
        if (chromiumSlot != kUnknownSlot &&
            (!office || HasChromiumPanningResources(info.szModName))) {
            return {chromiumSlot, Target::Chromium};
        }
        if (office) {
            const auto slot = FindFingerprintSlot(image, kOfficeResizeCursors);
            return {slot != kUnknownSlot ? slot : FindOfficeSlot(MakeShape(image)), Target::Office};
        }
        return {};
    };
    const auto image = ReadCursorBitmaps(cursor, bitmaps, false, kShapeSize * 2);
    if (image) {
        const auto match = findSlot(*image);
        if (match.slot != kUnknownSlot ||
            (!office && image->width == kShapeSize && image->height == kShapeSize)) {
            return match;
        }
    }

    // A DPI-specific copy can differ from the original resource's 32px raster.
    PCWSTR name = info.wResID ? MAKEINTRESOURCEW(info.wResID) : info.szResName;
    HMODULE module = nullptr;
    if (!named || (!info.wResID && !*name) ||
        !GetModuleHandleExW(0, info.szModName, &module)) {
        return {};
    }

    std::optional<CursorImage> resource;
    try {
        resource = LoadResourceImage(module, name);
    } catch (...) {
        FreeLibrary(module);
        throw;
    }
    FreeLibrary(module);
    return resource ? findSlot(*resource) : CursorMatch{};
}

CursorMatch IdentifyCursor(HCURSOR cursor) {
    if (!cursor) {
        return {};
    }

    struct Lookup {
        HCURSOR cursor = nullptr;
        CursorMatch match;
    };
    // Bounded per GUI thread. Normal mouse movement usually hits the same slot.
    thread_local std::array<Lookup, 32> entries;
    const uintptr_t key = reinterpret_cast<uintptr_t>(cursor);
    auto& entry = entries[(key ^ (key >> 16)) % entries.size()];
    if (entry.cursor != cursor) {
        entry = {cursor, ReadResourceSlot(cursor)};
    }
    return entry.match;
}

uint32_t SampleSmooth(const CursorImage& source, double centerX, double centerY,
                      double cosine, double sine) {
    // The outer taps reach adjacent pixels at exact right angles as well.
    constexpr std::array offsets = {-0.5625, -0.1875, 0.1875, 0.5625};
    constexpr std::array weights = {2u, 6u, 6u, 2u};
    constexpr unsigned totalWeight = 16 * 16;
    std::array<unsigned, 4> channels{};
    for (size_t j = 0; j < offsets.size(); ++j) {
        const double y = offsets[j];
        for (size_t k = 0; k < offsets.size(); ++k) {
            const double x = offsets[k];
            const int sx = static_cast<int>(std::lround(centerX + x * cosine + y * sine));
            const int sy = static_cast<int>(std::lround(centerY - x * sine + y * cosine));
            if (sx < 0 || sx >= source.width || sy < 0 || sy >= source.height) {
                continue;
            }
            const uint32_t pixel = source.smoothPixels[size_t(sy) * source.width + sx];
            const unsigned weight = weights[j] * weights[k];
            for (unsigned i = 0; i < channels.size(); ++i) {
                channels[i] += weight * ((pixel >> (8 * i)) & 0xFF);
            }
        }
    }

    uint32_t result = 0;
    for (unsigned i = 0; i < channels.size(); ++i) {
        result |= ((channels[i] + totalWeight / 2) / totalWeight) << (8 * i);
    }
    return result;
}

CursorImage Rotate(const CursorImage& source, unsigned eighthTurns, bool smooth) {
    const bool monochrome = source.color.empty() && !smooth;
    const double angle = eighthTurns * (3.14159265358979323846 / 4.0);
    // Round the exact right angles so trigonometric error cannot add a row.
    const double cosine = eighthTurns % 2 ? std::cos(angle) : std::round(std::cos(angle));
    const double sine = eighthTurns % 2 ? std::sin(angle) : std::round(std::sin(angle));

    double minX = 0, maxX = 0, minY = 0, maxY = 0;
    for (int x : {-source.hotX, source.width - 1 - source.hotX}) {
        for (int y : {-source.hotY, source.height - 1 - source.hotY}) {
            const double rx = x * cosine - y * sine;
            const double ry = x * sine + y * cosine;
            minX = (std::min)(minX, rx);
            maxX = (std::max)(maxX, rx);
            minY = (std::min)(minY, ry);
            maxY = (std::max)(maxY, ry);
        }
    }

    CursorImage result;
    const int margin = smooth ? 1 : 0;
    result.hotX = margin - static_cast<int>(std::floor(minX));
    result.hotY = margin - static_cast<int>(std::floor(minY));
    result.width = result.hotX + static_cast<int>(std::ceil(maxX)) + 1 + margin;
    result.height = result.hotY + static_cast<int>(std::ceil(maxY)) + 1 + margin;
    const size_t plane = size_t(result.width) * result.height;
    const size_t sourcePlane = size_t(source.width) * source.height;
    result.mask.assign(plane, 0x00FFFFFF);
    if (monochrome) {
        result.mask.resize(plane * 2);
    } else {
        result.color.resize(plane);
    }

    for (int y = 0; y < result.height; ++y) {
        for (int x = 0; x < result.width; ++x) {
            const int dx = x - result.hotX;
            const int dy = y - result.hotY;
            const size_t to = size_t(y) * result.width + x;
            if (smooth) {
                const uint32_t pixel = SampleSmooth(source,
                    dx * cosine + dy * sine + source.hotX,
                    -dx * sine + dy * cosine + source.hotY, cosine, sine);
                result.color[to] = pixel;
                if (pixel >> 24) {
                    result.mask[to] = 0;
                }
                continue;
            }

            const int sx = static_cast<int>(std::lround(dx * cosine + dy * sine)) +
                           source.hotX;
            const int sy = static_cast<int>(std::lround(-dx * sine + dy * cosine)) +
                           source.hotY;
            if (sx < 0 || sx >= source.width || sy < 0 || sy >= source.height) {
                continue;
            }

            const size_t from = size_t(sy) * source.width + sx;
            result.mask[to] = source.mask[from];
            if (monochrome) {
                result.mask[plane + to] = source.mask[sourcePlane + from];
            } else {
                result.color[to] = source.color[from];
            }
        }
    }

    return result;
}

CursorImage Mirror(CursorImage image) {
    image.hotX = image.width - 1 - image.hotX;
    for (auto* plane : {&image.mask, &image.color}) {
        for (size_t row = 0; row < plane->size(); row += image.width) {
            std::reverse(plane->data() + row, plane->data() + row + image.width);
        }
    }
    return image;
}

HCURSOR MakeCursor(const CursorImage& image) {
    const int maskHeight = image.color.empty() ? image.height * 2 : image.height;
    // CreateBitmap takes scan lines aligned to a WORD for a monochrome DDB.
    const size_t stride = (size_t(image.width) + 15) / 16 * 2;
    std::vector<BYTE> maskBits(stride * maskHeight);
    for (int y = 0; y < maskHeight; ++y) {
        for (int x = 0; x < image.width; ++x) {
            if (image.mask[size_t(y) * image.width + x] & 0x00FFFFFF) {
                maskBits[size_t(y) * stride + x / 8] |= BYTE(0x80 >> (x % 8));
            }
        }
    }

    IconBitmaps info;
    info.xHotspot = image.hotX;
    info.yHotspot = image.hotY;
    info.hbmMask = CreateBitmap(image.width, maskHeight, 1, 1, maskBits.data());
    if (!info.hbmMask) {
        return nullptr;
    }

    if (!image.color.empty()) {
        auto dib = ColorBitmapInfo(image.width, image.height);
        void* pixels = nullptr;
        info.hbmColor = CreateDIBSection(nullptr, &dib, DIB_RGB_COLORS, &pixels,
                                         nullptr, 0);
        if (!info.hbmColor || !pixels) {
            return nullptr;
        }
        std::memcpy(pixels, image.color.data(), image.color.size() * sizeof(uint32_t));
    }

    return CreateIconIndirect(&info);
}

bool WatchCursorSettings() {
    return RegNotifyChangeKeyValue(g_cursorsKey, FALSE,
                                   REG_NOTIFY_CHANGE_LAST_SET |
                                       REG_NOTIFY_THREAD_AGNOSTIC,
                                   g_changeEvent, TRUE) == ERROR_SUCCESS;
}

void CloseSettingsWatch() {
    if (g_cursorsKey) {
        RegCloseKey(g_cursorsKey);
        g_cursorsKey = nullptr;
    }
    if (g_changeEvent) {
        CloseHandle(g_changeEvent);
        g_changeEvent = nullptr;
    }
}

void CheckCursorSettings() {
    if (!g_watchStarted) {
        g_watchStarted = true;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Cursors", 0,
                           KEY_NOTIFY, &g_cursorsKey) == ERROR_SUCCESS) {
            g_changeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
            if (!g_changeEvent || !WatchCursorSettings()) {
                CloseSettingsWatch();
            }
        }
    }

    if (g_changeEvent && WaitForSingleObject(g_changeEvent, 0) == WAIT_OBJECT_0) {
        g_needsRefresh.fill(true);
        if (!WatchCursorSettings()) {
            CloseSettingsWatch();
        }
    }
}

bool RefreshSource(Role role) {
    const size_t index = static_cast<size_t>(role);
    const bool prepareSmoothing = std::any_of(
        kMappings.begin(), kMappings.end(), [role](const CursorMapping& mapping) {
            return mapping.role == role && (g_smoothingTypes & TypeBit(mapping.type));
        });
    auto image = ReadCursor(LoadCursorW(nullptr, kRoleIds[index]), prepareSmoothing);
    if (!image) {
        g_needsRefresh[index] = true;
        return false;
    }

    if (!g_sources[index] || image != g_sources[index]) {
        for (size_t i = 0; i < kMappings.size(); ++i) {
            if (kMappings[i].role == role && g_replacements[i].cursor) {
                g_retired.push_back(g_replacements[i]);
                g_replacements[i] = {};
            }
        }
        g_sources[index] = std::move(image);
    }

    g_needsRefresh[index] = false;
    return true;
}

HCURSOR ResolvePrevious(HCURSOR previous) {
    if (previous) {
        for (const auto& entry : g_replacements) {
            if (entry.cursor == previous) {
                return entry.original;
            }
        }
        for (const auto& entry : g_retired) {
            if (entry.cursor == previous) {
                return entry.original;
            }
        }
    }
    return previous;
}

ThreadSelection* FindCurrentSelection() {
    const DWORD id = GetCurrentThreadId();
    for (auto& selection : g_selections) {
        if (selection.id == id) {
            return &selection;
        }
    }
    return nullptr;
}

ThreadSelection* TrackCurrentSelection() {
    HANDLE lifetime = nullptr;
    if (!DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(),
                         &lifetime, SYNCHRONIZE, FALSE, 0)) {
        return nullptr;
    }

    try {
        g_selections.push_back({GetCurrentThreadId(), lifetime});
    } catch (...) {
        CloseHandle(lifetime);
        throw;
    }
    return &g_selections.back();
}

HWND FindCursorWindow(DWORD threadId) {
    // These queries do not send messages into a window while the lock is held.
    for (HWND window : {GetCapture(), GetFocus(), GetActiveWindow()}) {
        if (window && GetWindowThreadProcessId(window, nullptr) == threadId) {
            return window;
        }
    }
    return nullptr;
}

void ForgetInactiveThreads() {
    std::erase_if(g_selections, [](const ThreadSelection& selection) {
        if (!selection.cursor || WaitForSingleObject(selection.lifetime, 0) == WAIT_OBJECT_0) {
            CloseHandle(selection.lifetime);
            return true;
        }
        return false;
    });
}

bool IsAssigned(HCURSOR cursor) {
    return std::any_of(g_selections.begin(), g_selections.end(),
                       [cursor](const ThreadSelection& selection) {
                           return selection.cursor == cursor;
                       });
}

void FreeRetired() {
    if (g_retired.empty()) {
        return;
    }

    CURSORINFO visible{sizeof(visible)};
    if (!GetCursorInfo(&visible)) {
        return;
    }

    const HCURSOR threadCursor = GetCursor();
    std::erase_if(g_retired, [&](const Replacement& entry) {
        return entry.cursor != visible.hCursor && entry.cursor != threadCursor &&
               !IsAssigned(entry.cursor) && DestroyCursor(entry.cursor);
    });
}

constexpr uint32_t TargetBit(Target target) {
    return uint32_t{1} << static_cast<unsigned>(target);
}

std::wstring ProcessImagePath(HANDLE process) {
    std::wstring path(MAX_PATH, L'\0');
    for (;;) {
        DWORD length = static_cast<DWORD>(path.size());
        if (QueryFullProcessImageNameW(process, 0, path.data(), &length)) {
            path.resize(length);
            return path;
        }
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER || path.size() >= 32768) {
            return {};
        }
        path.resize((std::min)(path.size() * 2, size_t{32768}));
    }
}

bool IsExcluded(std::wstring_view path, const std::vector<std::wstring>& exclusions) {
    if (path.empty()) {
        return false;
    }
    for (const auto& entry : exclusions) {
        if (entry.find_first_of(L"\\:") == std::wstring::npos) {
            if (HasModuleName(path, entry)) {
                return true;
            }
        } else if (path.size() == entry.size() &&
                   CompareStringOrdinal(path.data(), static_cast<int>(path.size()),
                                        entry.data(), static_cast<int>(entry.size()),
                                        TRUE) == CSTR_EQUAL) {
            return true;
        }
    }
    return false;
}

std::vector<std::wstring> ReadExcludedApplications() {
    std::vector<std::wstring> entries;
    // The settings API exposes indexed values without an array length.
    for (int i = 0, empty = 0; empty < 32; ++i) {
        auto value = WindhawkUtils::StringSetting::make(L"excludedApplications[%d]", i);
        const std::wstring_view text(value.get());
        const auto first = text.find_first_not_of(L" \t\r\n");
        if (first == std::wstring_view::npos) {
            ++empty;
            continue;
        }
        empty = 0;
        auto& entry = entries.emplace_back(text.substr(
            first, text.find_last_not_of(L" \t\r\n") - first + 1));
        std::replace(entry.begin(), entry.end(), L'/', L'\\');
    }
    return entries;
}

bool IsExcludedHostWindow(HWND window) {
    const HWND root = window ? GetAncestor(window, GA_ROOT) : nullptr;
    DWORD processId = 0;
    if (!root || !GetWindowThreadProcessId(root, &processId) ||
        processId == GetCurrentProcessId()) {
        return false;
    }

    struct Host {
        HWND root;
        DWORD processId;
        std::wstring path;
    };
    thread_local Host host{};
    if (host.root != root || host.processId != processId) {
        host = {root, processId, {}};
        if (HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId)) {
            try {
                host.path = ProcessImagePath(process);
            } catch (...) {
                CloseHandle(process);
                throw;
            }
            CloseHandle(process);
        }
    }
    return IsExcluded(host.path, g_excludedApplications);
}

bool IsExcludedHost() {
    if (g_excludedApplications.empty()) {
        return false;
    }

    // A Chromium child HWND can be embedded in a different process.
    const DWORD thread = GetCurrentThreadId();
    HWND window = GetCapture();
    if (!window) {
        POINT point{};
        if (GetCursorPos(&point)) {
            window = WindowFromPoint(point);
        }
    }
    if (!window || GetWindowThreadProcessId(window, nullptr) != thread) {
        window = FindCursorWindow(thread);
    }
    return IsExcludedHostWindow(window);
}

bool ReplacementEnabled(CursorMatch match) {
    return match.slot != kUnknownSlot && !g_processExcluded.load(std::memory_order_acquire) &&
           (g_enabledTargets.load(std::memory_order_acquire) & TargetBit(match.target)) &&
           (g_enabledTypes.load(std::memory_order_acquire) & TypeBit(kMappings[match.slot].type));
}

HCURSOR WINAPI SetCursorHook(HCURSOR cursor) {
    try {
        const auto match = g_processExcluded.load(std::memory_order_acquire)
            ? CursorMatch{} : IdentifyCursor(cursor);
        if (!ReplacementEnabled(match) && !g_needsCleanup.load()) {
            g_previousRole = Role::Count;
            return g_setCursorOriginal(cursor);
        }

        std::lock_guard lock(g_mutex);
        ForgetInactiveThreads();
        HCURSOR display = cursor;
        if (!g_stopping && ReplacementEnabled(match) &&
            (match.target != Target::Chromium || !IsExcludedHost())) {
            const auto& mapping = kMappings[match.slot];
            const size_t role = static_cast<size_t>(mapping.role);
            CheckCursorSettings();
            if ((g_previousRole == mapping.role && !g_needsRefresh[role]) ||
                RefreshSource(mapping.role)) {
                const CursorImage& source = *g_sources[role];
                const bool smooth = (g_smoothingTypes & TypeBit(mapping.type)) &&
                                    !source.smoothPixels.empty();
                auto& entry = g_replacements[match.slot];
                if (entry.cursor && (entry.original != cursor || entry.smooth != smooth)) {
                    g_retired.push_back(entry);
                    entry = {};
                }
                if (!entry.cursor) {
                    HCURSOR replacement;
                    if (mapping.mirror) {
                        replacement = MakeCursor(Mirror(source));
                    } else if (mapping.eighthTurns || smooth) {
                        replacement = MakeCursor(Rotate(source, mapping.eighthTurns, smooth));
                    } else {
                        replacement = MakeCursor(source);
                    }
                    if (replacement) {
                        entry = {replacement, cursor, smooth};
                    }
                }
                if (entry.cursor) {
                    display = entry.cursor;
                }
            }
            g_previousRole = mapping.role;
        } else {
            g_previousRole = Role::Count;
        }

        auto* selection = FindCurrentSelection();
        if (display != cursor) {
            if (!selection) {
                selection = TrackCurrentSelection();
            }
            if (selection) {
                selection->window = FindCursorWindow(selection->id);
                g_needsCleanup.store(true);
            } else {
                display = cursor;
            }
        }

        HCURSOR previous = ResolvePrevious(g_setCursorOriginal(display));
        if (selection) {
            const HCURSOR current = GetCursor();
            if (display != cursor && current == display) {
                selection->cursor = display;
            } else if (current != selection->cursor) {
                selection->cursor = nullptr;
            }
        }
        ForgetInactiveThreads();
        FreeRetired();
        g_needsCleanup.store(!g_selections.empty() || !g_retired.empty());
        return previous;
    } catch (...) {
        g_previousRole = Role::Count;
        return g_setCursorOriginal(cursor);
    }
}

void UpdateSettings() {
    constexpr std::array smoothingSettings = {
        std::pair{CursorType::AutoscrollOrigin, L"smoothing.autoscrollOrigin"},
        std::pair{CursorType::AutoscrollArrows, L"smoothing.autoscrollArrows"},
        std::pair{CursorType::SelectionArrows, L"smoothing.selectionArrows"},
    };
    uint32_t smoothingTypes = 0;
    for (const auto& [type, name] : smoothingSettings) {
        if (Wh_GetIntSetting(name)) {
            smoothingTypes |= TypeBit(type);
        }
    }
    constexpr std::array targetSettings = {
        std::pair{Target::Windows, L"targets.windows"},
        std::pair{Target::Chromium, L"targets.chromium"},
        std::pair{Target::Gecko, L"targets.gecko"},
        std::pair{Target::Qt, L"targets.qt"},
        std::pair{Target::Office, L"targets.office"},
    };
    constexpr std::array cursorSettings = {
        std::pair{CursorType::AutoscrollOrigin, L"cursors.autoscrollOrigin"},
        std::pair{CursorType::AutoscrollArrows, L"cursors.autoscrollArrows"},
        std::pair{CursorType::Link, L"cursors.link"},
        std::pair{CursorType::Move, L"cursors.move"},
        std::pair{CursorType::Resize, L"cursors.resize"},
        std::pair{CursorType::MarginSelection, L"cursors.marginSelection"},
        std::pair{CursorType::SelectionArrows, L"cursors.downSelection"},
        std::pair{CursorType::Cell, L"cursors.cell"},
        std::pair{CursorType::VerticalText, L"cursors.verticalText"},
    };
    uint32_t enabledTypes = 0;
    for (const auto& [type, name] : cursorSettings) {
        if (Wh_GetIntSetting(name)) {
            enabledTypes |= TypeBit(type);
        }
    }
    uint32_t enabledTargets = 0;
    for (const auto& [target, name] : targetSettings) {
        if (Wh_GetIntSetting(name)) {
            enabledTargets |= TargetBit(target);
        }
    }
    auto exclusions = ReadExcludedApplications();
    const bool processExcluded = !exclusions.empty() &&
        IsExcluded(ProcessImagePath(GetCurrentProcess()), exclusions);
    std::lock_guard lock(g_mutex);
    const uint32_t changedSmoothing = g_smoothingTypes ^ smoothingTypes;
    if (changedSmoothing) {
        for (const auto& mapping : kMappings) {
            if (changedSmoothing & TypeBit(mapping.type)) {
                g_needsRefresh[static_cast<size_t>(mapping.role)] = true;
            }
        }
        g_smoothingTypes = smoothingTypes;
    }
    g_excludedApplications = std::move(exclusions);
    g_processExcluded.store(processExcluded, std::memory_order_release);
    g_enabledTargets.store(enabledTargets, std::memory_order_release);
    g_enabledTypes.store(enabledTypes, std::memory_order_release);
}

}  // namespace

BOOL Wh_ModInit() {
    if (!GetModuleHandleW(L"user32.dll") || IsChromiumChildProcess()) {
        return FALSE;
    }

    ObserveOffice();
    g_needsRefresh.fill(true);
    UpdateSettings();

    if (!WindhawkUtils::SetFunctionHook(SetCursor, SetCursorHook, &g_setCursorOriginal)) {
        CloseSettingsWatch();
        return FALSE;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    UpdateSettings();
}

void Wh_ModBeforeUninit() {
    try {
        std::vector<std::pair<DWORD, HWND>> windows;
        {
            std::lock_guard lock(g_mutex);
            g_stopping = true;
            ForgetInactiveThreads();
            for (const auto& selection : g_selections) {
                windows.emplace_back(selection.id, selection.window);
            }
        }

        // The owning GUI thread must restore its cursor. Avoid blocking while
        // holding the lock because the window can call SetCursor from here.
        for (auto [id, window] : windows) {
            DWORD process = 0;
            if (window && GetWindowThreadProcessId(window, &process) == id &&
                process == GetCurrentProcessId()) {
                SendMessageTimeoutW(window, WM_SETCURSOR,
                                    reinterpret_cast<WPARAM>(window),
                                    MAKELPARAM(HTCLIENT, WM_MOUSEMOVE),
                                    SMTO_ABORTIFHUNG | SMTO_BLOCK | SMTO_ERRORONEXIT,
                                    100, nullptr);
            }
        }
    } catch (...) {
        // A cursor still assigned to a live thread must survive unloading.
    }
}

void Wh_ModUninit() {
    std::lock_guard lock(g_mutex);
    CloseSettingsWatch();
    ForgetInactiveThreads();
    CURSORINFO visible{sizeof(visible)};
    const bool haveVisible = GetCursorInfo(&visible);
    const auto freeIfUnused = [&](const Replacement& entry) {
        if (haveVisible && entry.cursor && entry.cursor != visible.hCursor &&
            !IsAssigned(entry.cursor)) {
            DestroyCursor(entry.cursor);
        }
    };
    for (const auto& entry : g_replacements) {
        freeIfUnused(entry);
    }
    for (const auto& entry : g_retired) {
        freeIfUnused(entry);
    }
    for (const auto& selection : g_selections) {
        CloseHandle(selection.lifetime);
    }
}
