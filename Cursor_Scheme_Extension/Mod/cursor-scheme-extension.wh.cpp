// ==WindhawkMod==
// @id              cursor-scheme-extension
// @name            Cursor Scheme Extension
// @description     Apply the active Windows cursor scheme to recognized application pointers
// @description:ru-RU Применяет активную схему Windows к распознанным указателям приложений
// @description:es-ES Aplica el esquema de punteros activo de Windows a los punteros reconocidos de las aplicaciones
// @description:de-DE Wendet das aktive Windows-Zeigerschema auf erkannte Anwendungszeiger an
// @description:pt-BR Aplica o esquema de ponteiros ativo do Windows aos ponteiros reconhecidos dos aplicativos
// @description:zh-CN 将当前 Windows 指针方案应用于已识别的应用指针
// @version         0.9.0
// @author          CTravkin
// @github          https://github.com/CTravkin
// @homepage        https://ctravkin.ru
// @license         MIT
// @include         *
// @architecture    x86
// @architecture    x86-64
// @compilerOptions -lgdi32 -ladvapi32 -lshell32
// ==/WindhawkMod==
// Build: 14

// ==WindhawkModReadme==
/*
# Cursor Scheme Extension

Applies the active Windows cursor scheme to recognized application pointers.
The mod covers middle-button autoscroll, move and resize pointers, link
pointers, and several selection and text pointers. Choose the application
families and pointer types independently in Settings. Autoscroll arrows can
be smoothed in all eight directions.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- targets:
    - chromium: true
      $name: Chromium and Electron
      $name:ru-RU: Chromium и Electron
      $name:es-ES: Chromium y Electron
      $name:de-DE: Chromium und Electron
      $name:pt-BR: Chromium e Electron
      $name:zh-CN: Chromium 和 Electron
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
- smoothDiagonals: true
  $name: Smoothing
  $name:ru-RU: Сглаживание
  $name:es-ES: Suavizado
  $name:de-DE: Kantenglättung
  $name:pt-BR: Suavização
  $name:zh-CN: 边缘平滑
  $description: Smooth the edges of autoscroll arrows in all eight directions
  $description:ru-RU: Сглаживание краев стрелок автопрокрутки во всех восьми направлениях
  $description:es-ES: Suaviza los bordes de las flechas de desplazamiento automático en las ocho direcciones
  $description:de-DE: Glättet die Kanten der Scrollpfeile in allen acht Richtungen
  $description:pt-BR: Suaviza as bordas das setas de rolagem automática nas oito direções
  $description:zh-CN: 平滑八个方向的自动滚动箭头边缘
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

struct CursorMatch {
    size_t slot = kUnknownSlot;
    bool chromium = false;
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
std::atomic<bool> g_replaceInOffice;
std::atomic<bool> g_replaceInElectron;
std::atomic<uint32_t> g_enabledTypes;
std::atomic<bool> g_isOffice;
bool g_smoothArrows;
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
            ? CursorMatch{SystemSlot(info.wResID)} : CursorMatch{};
    }
    if (HasModuleName(info.szModName, L"comctl32.dll")) {
        return {ResourceSlot(info.wResID, kCommonControlsResources)};
    }
    if (!office && !*info.szModName) {
        return {};
    }

    const auto findSlot = [office](const CursorImage& image) -> CursorMatch {
        if (!office) {
            return {FindFingerprintSlot(image, kChromiumCursors), true};
        }
        const auto slot = FindFingerprintSlot(image, kOfficeResizeCursors);
        return {slot != kUnknownSlot ? slot : FindOfficeSlot(MakeShape(image))};
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
    if (!*info.szModName || (!info.wResID && !*name) ||
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
    auto image = ReadCursor(LoadCursorW(nullptr, kRoleIds[index]),
                            role == Role::Up && g_smoothArrows);
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

constexpr uint32_t TypeBit(CursorType type) {
    return uint32_t{1} << static_cast<unsigned>(type);
}

bool ReplacementEnabled(CursorMatch match) {
    if (match.slot == kUnknownSlot ||
        !(g_enabledTypes.load(std::memory_order_acquire) & TypeBit(kMappings[match.slot].type))) {
        return false;
    }
    if (g_isOffice.load(std::memory_order_relaxed)) {
        return g_replaceInOffice.load(std::memory_order_acquire);
    }
    return !match.chromium || g_replaceInElectron.load(std::memory_order_acquire);
}

HCURSOR WINAPI SetCursorHook(HCURSOR cursor) {
    try {
        const auto match = IdentifyCursor(cursor);
        if (!ReplacementEnabled(match) && !g_needsCleanup.load()) {
            g_previousRole = Role::Count;
            return g_setCursorOriginal(cursor);
        }

        std::lock_guard lock(g_mutex);
        ForgetInactiveThreads();
        HCURSOR display = cursor;
        if (!g_stopping && ReplacementEnabled(match)) {
            const auto& mapping = kMappings[match.slot];
            const size_t role = static_cast<size_t>(mapping.role);
            CheckCursorSettings();
            if ((g_previousRole == mapping.role && !g_needsRefresh[role]) ||
                RefreshSource(mapping.role)) {
                auto& entry = g_replacements[match.slot];
                if (entry.cursor && entry.original != cursor) {
                    g_retired.push_back(entry);
                    entry = {};
                }
                if (!entry.cursor) {
                    const CursorImage& source = *g_sources[role];
                    const bool smooth = mapping.type == CursorType::AutoscrollArrows &&
                                        !source.smoothPixels.empty();
                    HCURSOR replacement;
                    if (mapping.mirror) {
                        replacement = MakeCursor(Mirror(source));
                    } else if (mapping.eighthTurns || smooth) {
                        replacement = MakeCursor(Rotate(source, mapping.eighthTurns, smooth));
                    } else {
                        replacement = MakeCursor(source);
                    }
                    if (replacement) {
                        entry = {replacement, cursor};
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
    const bool replaceInOffice = Wh_GetIntSetting(L"targets.office") != 0;
    const bool replaceInElectron = Wh_GetIntSetting(L"targets.chromium") != 0;
    const bool smoothArrows = Wh_GetIntSetting(L"smoothDiagonals") != 0;
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
    std::lock_guard lock(g_mutex);
    if (g_smoothArrows != smoothArrows) {
        g_smoothArrows = smoothArrows;
        g_needsRefresh[static_cast<size_t>(Role::Up)] = true;
    }
    g_replaceInElectron.store(replaceInElectron, std::memory_order_release);
    g_replaceInOffice.store(replaceInOffice, std::memory_order_release);
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
