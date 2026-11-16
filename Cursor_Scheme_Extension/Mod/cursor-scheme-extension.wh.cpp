// ==WindhawkMod==
// @id              cursor-scheme-extension
// @name            Cursor Scheme Extension
// @description     Use the active Windows cursor scheme for native middle-button autoscroll
// @description:ru-RU Применение активной схемы указателей Windows к штатной автопрокрутке
// @version         0.4.2
// @author          CTravkin
// @github          https://github.com/CTravkin
// @homepage        https://ctravkin.ru
// @license         MIT
// @include         cursor-scheme-extension-preview.exe
// @include         excel.exe
// @include         winword.exe
// @include         powerpnt.exe
// @include         outlook.exe
// @include         onenote.exe
// @include         msaccess.exe
// @include         mspub.exe
// @include         visio.exe
// @include         winproj.exe
// @architecture    x86
// @architecture    x86-64
// @compilerOptions -lgdi32 -ladvapi32
// ==/WindhawkMod==
// Build: 7

// ==WindhawkModReadme==
/*
# Cursor Scheme Extension

Uses the active Windows cursor scheme for native middle-button autoscroll. At
the starting point, the pointer shows the scrolling axes indicated by the
application. While scrolling, the scheme's Alternate select arrow points in the
scroll direction.

The Office option replaces recognized autoscroll cursors in the classic desktop
Office applications. Excel also uses the scheme's Move pointer for recognized
starting markers. Diagonal smoothing is optional.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- replaceInOffice: false
  $name: Replace cursors in Office
  $name:ru-RU: Замена указателей в Office
  $description: Apply the Windows scheme to recognized autoscroll cursors in classic desktop Office apps
  $description:ru-RU: Применение схемы Windows к распознанным указателям автопрокрутки в классических приложениях Office
- smoothDiagonals: true
  $name: Smooth diagonal arrows
  $name:ru-RU: Сглаживание диагональных стрелок
  $description: Smooth edges with partially transparent pixels when rotating the arrow diagonally
  $description:ru-RU: Сглаживание краев полупрозрачными пикселями при повороте стрелки по диагонали
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <psapi.h>
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

enum class Role : size_t { Vertical, Horizontal, Move, Up, Count };

struct CursorMapping {
    WORD systemId;
    Role role;
    unsigned eighthTurns;
};

constexpr std::array kMappings = {
    CursorMapping{32652, Role::Vertical, 0},
    CursorMapping{32653, Role::Horizontal, 0},
    CursorMapping{32654, Role::Move, 0},
    CursorMapping{32655, Role::Up, 0},
    CursorMapping{32656, Role::Up, 4},
    CursorMapping{32657, Role::Up, 6},
    CursorMapping{32658, Role::Up, 2},
    CursorMapping{32659, Role::Up, 7},
    CursorMapping{32660, Role::Up, 1},
    CursorMapping{32661, Role::Up, 5},
    CursorMapping{32662, Role::Up, 3},
};

struct ResourceMapping {
    WORD id;
    WORD systemId;
};

constexpr PCWSTR kExcelProcess = L"excel.exe";
constexpr std::array kOfficeProcesses = {
    kExcelProcess, L"winword.exe", L"powerpnt.exe", L"outlook.exe",
    L"onenote.exe", L"msaccess.exe", L"mspub.exe", L"visio.exe", L"winproj.exe",
};

constexpr std::array kCommonControlsResources = {
    ResourceMapping{109, 32652}, ResourceMapping{110, 32653},
    ResourceMapping{111, 32654}, ResourceMapping{112, 32655},
    ResourceMapping{113, 32656}, ResourceMapping{114, 32658},
    ResourceMapping{115, 32657}, ResourceMapping{116, 32660},
    ResourceMapping{117, 32659}, ResourceMapping{118, 32662},
    ResourceMapping{119, 32661},
};

const std::array kRoleIds = {IDC_SIZENS, IDC_SIZEWE, IDC_SIZEALL, IDC_UPARROW};
constexpr size_t kRoleCount = static_cast<size_t>(Role::Count);
constexpr size_t kUnknownSlot = kMappings.size();
constexpr int kMaxSourceSize = 512;
constexpr int kShapeSize = 32;

struct CursorImage {
    int width = 0;
    int height = 0;
    int hotX = 0;
    int hotY = 0;
    // A monochrome cursor keeps the XOR plane below its AND plane.
    std::vector<uint32_t> mask;
    std::vector<uint32_t> color;
    std::vector<uint32_t> diagonalPixels;

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

struct ExcelMarker {
    HBITMAP bitmap = nullptr;
    HCURSOR neutralCursor = nullptr;
    HCURSOR exitCursor = nullptr;
    bool painted = false;
};

std::array<HCURSOR, kMappings.size()> g_nativeCursors;
std::array<CursorShape, kMappings.size()> g_referenceShapes;
std::array<Replacement, kMappings.size()> g_replacements;
std::array<std::optional<CursorImage>, kRoleCount> g_sources;
std::array<bool, kRoleCount> g_needsRefresh;
std::vector<Replacement> g_retired;
std::vector<ThreadSelection> g_selections;
std::mutex g_mutex;
std::once_flag g_referenceShapesInit;
std::atomic<bool> g_needsCleanup;
std::atomic<bool> g_replaceInOffice;
bool g_smoothDiagonals;
bool g_stopping;
bool g_isOffice;
bool g_isExcel;
bool g_watchStarted;
bool g_handlesReady;
HKEY g_cursorsKey;
HANDLE g_changeEvent;
thread_local Role g_previousRole = Role::Count;
decltype(&SetCursor) g_setCursorOriginal;
using OfficeLoadBitmap = HBITMAP(WINAPI*)(HMODULE, WORD);
OfficeLoadBitmap g_officeLoadBitmapOriginal;
decltype(&BitBlt) g_bitBltOriginal;
std::atomic<bool> g_excelBitmapHookPending;
thread_local ExcelMarker g_excelMarker;

constexpr size_t SystemSlot(WORD id) {
    for (size_t i = 0; i < kMappings.size(); ++i) {
        if (kMappings[i].systemId == id) {
            return i;
        }
    }
    return kUnknownSlot;
}

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

std::vector<uint32_t> PrepareDiagonalPixels(HCURSOR cursor, HDC dc,
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
                                            bool prepareDiagonals, int maxSize) {
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

    if (prepareDiagonals) {
        image.diagonalPixels = PrepareDiagonalPixels(cursor, dc.value, image);
    }
    return image;
}

std::optional<CursorImage> ReadCursor(HCURSOR cursor, bool prepareDiagonals,
                                     int maxSize = kMaxSourceSize) {
    IconBitmaps info;
    if (!cursor || !GetIconInfo(cursor, &info)) {
        return std::nullopt;
    }
    return ReadCursorBitmaps(cursor, info, prepareDiagonals, maxSize);
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

CursorShape LoadResourceShape(HMODULE module, PCWSTR name) {
    const auto cursor = static_cast<HCURSOR>(LoadImageW(module, name, IMAGE_CURSOR,
                                                       kShapeSize, kShapeSize, 0));
    if (!cursor) {
        return {};
    }

    std::optional<CursorImage> image;
    try {
        image = ReadCursor(cursor, false, kShapeSize);
    } catch (...) {
        DestroyCursor(cursor);
        throw;
    }
    DestroyCursor(cursor);
    return image ? MakeShape(*image) : CursorShape{};
}

size_t FindShapeSlot(const CursorShape& shape) {
    if (!shape.width) {
        return kUnknownSlot;
    }
    std::call_once(g_referenceShapesInit, [] {
        for (size_t i = 0; i < kMappings.size(); ++i) {
            g_referenceShapes[i] = LoadResourceShape(nullptr,
                MAKEINTRESOURCEW(kMappings[i].systemId));
        }
    });

    for (size_t i = 0; i < g_referenceShapes.size(); ++i) {
        if (g_referenceShapes[i] == shape) {
            return i;
        }
    }
    return kUnknownSlot;
}

size_t ReadResourceSlot(HCURSOR cursor) {
    ICONINFOEXW info{sizeof(info)};
    if (!GetIconInfoExW(cursor, &info)) {
        return kUnknownSlot;
    }
    IconBitmaps bitmaps;
    bitmaps.fIcon = info.fIcon;
    bitmaps.xHotspot = info.xHotspot;
    bitmaps.yHotspot = info.yHotspot;
    bitmaps.hbmMask = info.hbmMask;
    bitmaps.hbmColor = info.hbmColor;

    if (info.fIcon) {
        return kUnknownSlot;
    }
    if (HasModuleName(info.szModName, L"USER32") ||
        HasModuleName(info.szModName, L"user32.dll")) {
        return SystemSlot(info.wResID);
    }
    if (HasModuleName(info.szModName, L"comctl32.dll")) {
        return ResourceSlot(info.wResID, kCommonControlsResources);
    }
    if (!g_isOffice) {
        return kUnknownSlot;
    }

    const auto image = ReadCursorBitmaps(cursor, bitmaps, false, kShapeSize * 2);
    const size_t slot = image ? FindShapeSlot(MakeShape(*image)) : kUnknownSlot;
    if (slot != kUnknownSlot) {
        return slot;
    }

    // A DPI-specific copy can differ from the original resource's 32px raster.
    PCWSTR name = info.wResID ? MAKEINTRESOURCEW(info.wResID) : info.szResName;
    HMODULE module = nullptr;
    if (!*info.szModName || (!info.wResID && !*name) ||
        !GetModuleHandleExW(0, info.szModName, &module)) {
        return kUnknownSlot;
    }

    CursorShape shape;
    try {
        shape = LoadResourceShape(module, name);
    } catch (...) {
        FreeLibrary(module);
        throw;
    }
    FreeLibrary(module);
    return FindShapeSlot(shape);
}

size_t IdentifyCursor(HCURSOR cursor) {
    if (!cursor) {
        return kUnknownSlot;
    }
    for (size_t i = 0; i < g_nativeCursors.size(); ++i) {
        if (g_nativeCursors[i] == cursor) {
            return i;
        }
    }

    struct Lookup {
        HCURSOR cursor = nullptr;
        size_t slot = kUnknownSlot;
    };
    // Bounded per GUI thread. Normal mouse movement usually hits the same slot.
    thread_local std::array<Lookup, 32> entries;
    const uintptr_t key = reinterpret_cast<uintptr_t>(cursor);
    auto& entry = entries[(key ^ (key >> 16)) % entries.size()];
    if (entry.cursor != cursor) {
        entry = {cursor, ReadResourceSlot(cursor)};
    }
    return entry.slot;
}

uint32_t SampleDiagonal(const CursorImage& source, double centerX, double centerY,
                        double cosine, double sine) {
    constexpr std::array offsets = {-0.375, -0.125, 0.125, 0.375};
    constexpr unsigned count = offsets.size() * offsets.size();
    std::array<unsigned, 4> channels{};
    for (double x : offsets) {
        for (double y : offsets) {
            const int sx = static_cast<int>(std::lround(centerX + x * cosine + y * sine));
            const int sy = static_cast<int>(std::lround(centerY - x * sine + y * cosine));
            if (sx < 0 || sx >= source.width || sy < 0 || sy >= source.height) {
                continue;
            }
            const uint32_t pixel = source.diagonalPixels[size_t(sy) * source.width + sx];
            for (unsigned i = 0; i < channels.size(); ++i) {
                channels[i] += (pixel >> (8 * i)) & 0xFF;
            }
        }
    }

    uint32_t result = 0;
    for (unsigned i = 0; i < channels.size(); ++i) {
        result |= ((channels[i] + count / 2) / count) << (8 * i);
    }
    return result;
}

CursorImage Rotate(const CursorImage& source, unsigned eighthTurns) {
    const bool smooth = eighthTurns % 2 && !source.diagonalPixels.empty();
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
                const uint32_t pixel = SampleDiagonal(source,
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
                            role == Role::Up && g_smoothDiagonals);
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

bool ReplacementEnabled() {
    return !g_isOffice || g_replaceInOffice.load(std::memory_order_acquire);
}

struct ExcelMarkerResource {
    WORD id;
    DWORD size;
    uint64_t hash;
};

// Excel packs the same marker at three DPI sizes.
constexpr std::array kExcelMarkerResources = {
    ExcelMarkerResource{365, 136, 0xb36b32c450eb710c},
    ExcelMarkerResource{1989, 452, 0x2bc2f0d09d3303d1},
    ExcelMarkerResource{1990, 737, 0xe85c7048c3735c62},
};

HRSRC FindExcelBitmapResource(HMODULE module, WORD id) {
    return FindResourceW(module, MAKEINTRESOURCEW(id), MAKEINTRESOURCEW(998));
}

bool ExcelMarkerMatches(HMODULE module, HRSRC resource, const ExcelMarkerResource& marker) {
    if (!resource || SizeofResource(module, resource) != marker.size) {
        return false;
    }
    const auto* data = static_cast<const BYTE*>(LockResource(LoadResource(module, resource)));
    if (!data) {
        return false;
    }
    uint64_t hash = 0xcbf29ce484222325;
    for (DWORD i = 0; i < marker.size; ++i) {
        hash = (hash ^ data[i]) * 0x100000001b3;
    }
    return hash == marker.hash;
}

auto ReadExcelCommonMarkers() {
    std::array<bool, kExcelMarkerResources.size()> matches{};
    constexpr wchar_t commonFile[] = L"XLINTL32.COMMON.DLL";
    wchar_t path[MAX_PATH];
    const DWORD length = GetModuleFileNameW(GetModuleHandleW(nullptr), path, MAX_PATH);
    if (!length || length >= MAX_PATH) {
        return matches;
    }
    const auto slash = std::wstring_view(path, length).find_last_of(L"\\/");
    if (slash == std::wstring_view::npos || slash + 1 + std::size(commonFile) > MAX_PATH) {
        return matches;
    }
    std::copy_n(commonFile, std::size(commonFile), path + slash + 1);
    HMODULE common = LoadLibraryExW(path, nullptr, LOAD_LIBRARY_AS_DATAFILE);
    if (!common) {
        return matches;
    }
    for (size_t i = 0; i < matches.size(); ++i) {
        const auto& marker = kExcelMarkerResources[i];
        matches[i] = ExcelMarkerMatches(common, FindExcelBitmapResource(common, marker.id), marker);
    }
    FreeLibrary(common);
    return matches;
}

bool IsExcelMarkerResource(HMODULE module, WORD id) {
    for (size_t i = 0; i < kExcelMarkerResources.size(); ++i) {
        const auto& marker = kExcelMarkerResources[i];
        if (id != marker.id) {
            continue;
        }
        if (HRSRC resource = FindExcelBitmapResource(module, id)) {
            return ExcelMarkerMatches(module, resource, marker);
        }
        // Office resolves a missing localized graphic against the common resource DLL.
        wchar_t path[MAX_PATH];
        const auto mapped = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(module) & ~uintptr_t{3});
        const DWORD length = K32GetMappedFileNameW(GetCurrentProcess(), mapped, path, MAX_PATH);
        if (!length || length >= MAX_PATH ||
            !HasModuleName(std::wstring_view(path, length), L"XLINTL32.DLL")) {
            return false;
        }
        static const auto commonMarkers = ReadExcelCommonMarkers();
        return commonMarkers[i];
    }
    return false;
}

HBITMAP MakeExcelMarker(HBITMAP original) {
    BITMAP bounds{};
    if (!GetObjectW(original, sizeof(bounds), &bounds) ||
        bounds.bmWidth < 2 || bounds.bmWidth > 64 ||
        bounds.bmHeight < 2 || bounds.bmHeight > 64) {
        return nullptr;
    }
    const auto image = ReadCursor(LoadCursorW(nullptr, IDC_SIZEALL), true);
    if (!image || image->diagonalPixels.empty()) {
        return nullptr;
    }
    const int width = bounds.bmWidth;
    const int height = bounds.bmHeight;
    const int centerX = width / 2;
    const int centerY = height / 2;
    // Excel centers the bitmap on the click and reads it back as a bottom-up DIB.
    const double scale = (std::min)({1.0,
        double(centerX) / (std::max)(image->hotX, image->width - image->hotX),
        double(centerY) / (std::max)(image->hotY, image->height - image->hotY)});
    auto info = ColorBitmapInfo(width, height);
    info.bmiHeader.biHeight = height;
    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap) {
        return nullptr;
    }
    auto* pixels = static_cast<uint32_t*>(bits);
    std::fill_n(pixels, size_t(width) * height, 0);
    for (int y = 0; y < height; ++y) {
        const int sy = image->hotY + static_cast<int>(std::lround((y - centerY) / scale));
        if (sy < 0 || sy >= image->height) {
            continue;
        }
        auto* row = pixels + size_t(height - 1 - y) * width;
        for (int x = 0; x < width; ++x) {
            const int sx = image->hotX + static_cast<int>(std::lround((x - centerX) / scale));
            if (sx >= 0 && sx < image->width) {
                row[x] = image->diagonalPixels[size_t(sy) * image->width + sx];
            }
        }
    }
    return bitmap;
}

HBITMAP WINAPI OfficeLoadBitmapHook(HMODULE module, WORD id) {
    HBITMAP original = g_officeLoadBitmapOriginal(module, id);
    if (!original || !ReplacementEnabled()) {
        return original;
    }
    try {
        if (!IsExcelMarkerResource(module, id)) {
            return original;
        }
        g_excelMarker = {};
        HMODULE excel = GetModuleHandleW(nullptr);
        HCURSOR neutral = LoadCursorW(excel, MAKEINTRESOURCEW(256));
        HCURSOR exit = LoadCursorW(excel, MAKEINTRESOURCEW(257));
        if (neutral && exit) {
            g_excelMarker = {original, neutral, exit};
        }
    } catch (...) {
        // Excel still owns the original marker.
    }
    return original;
}

BOOL WINAPI BitBltHook(HDC destination, int x, int y, int width, int height,
                       HDC source, int sourceX, int sourceY, DWORD operation) {
    if (g_excelMarker.bitmap && operation == SRCCOPY && source && ReplacementEnabled() &&
        width > 0 && width <= 64 && height > 0 && height <= 64 &&
        GetCurrentObject(source, OBJ_BITMAP) == g_excelMarker.bitmap) {
        g_excelMarker.painted = false;
        try {
            // Excel copies this bitmap without alpha. Blend the themed image on the same DC.
            MemoryDc themed;
            IconBitmaps bitmap;
            bitmap.hbmColor = themed.value ? MakeExcelMarker(g_excelMarker.bitmap) : nullptr;
            if (bitmap.hbmColor) {
                HGDIOBJ previous = SelectObject(themed.value, bitmap.hbmColor);
                if (previous && previous != HGDI_ERROR) {
                    constexpr BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
                    const bool drawn = GdiAlphaBlend(destination, x, y, width, height,
                                                     themed.value, sourceX, sourceY,
                                                     width, height, blend);
                    SelectObject(themed.value, previous);
                    if (drawn) {
                        g_excelMarker.painted = true;
                        return TRUE;
                    }
                }
            }
        } catch (...) {
            // Excel can still draw its original bitmap.
        }
    }
    return g_bitBltOriginal(destination, x, y, width, height, source, sourceX, sourceY,
                            operation);
}

void TryHookExcelBitmapLoader(bool applyNow) {
    constexpr PCWSTR moduleName = L"Mso30win32client.dll";
#if defined(__x86_64__)
    constexpr WORD ordinal = 35939;
    constexpr BYTE prefix[] = {0x48, 0x89, 0x5c, 0x24, 0x08, 0x48, 0x89, 0x74,
                               0x24, 0x10, 0x57, 0x48, 0x83, 0xec, 0x40, 0x45,
                               0x33, 0xc9, 0x45, 0x33, 0xc0, 0xe8};
    constexpr size_t bodyOffset = 0x37;
    constexpr BYTE body[] = {0x66, 0x44, 0x39, 0x43, 0x0e, 0x75, 0x40, 0x8b,
                             0x53, 0x08, 0x45, 0x8b, 0xc8, 0x8b, 0x4b, 0x04};
#elif defined(__i386__)
    constexpr WORD ordinal = 61718;
    constexpr BYTE prefix[] = {0x55, 0x8b, 0xec, 0x53, 0x56, 0x33, 0xdb, 0x53,
                               0x53, 0xff, 0x75, 0x0c, 0xff, 0x75, 0x08, 0xe8};
    constexpr size_t bodyOffset = 0x78;
    constexpr BYTE body[] = {0x8b, 0xc3, 0x5f, 0x5e, 0x5b, 0x5d, 0xc2, 0x08, 0x00};
#else
    constexpr WORD ordinal = 0;
    constexpr BYTE prefix[] = {0};
    constexpr size_t bodyOffset = 0;
    constexpr BYTE body[] = {0};
#endif
    if (!ordinal) {
        g_excelBitmapHookPending.store(false, std::memory_order_relaxed);
        return;
    }
    HMODULE module = GetModuleHandleW(moduleName);
    if (!module || !g_excelBitmapHookPending.exchange(false)) {
        return;
    }
    // Validate the private export before calling its trampoline with this ABI.
    auto entry = GetProcAddress(module, MAKEINTRESOURCEA(ordinal));
    std::array<BYTE, bodyOffset + sizeof(body)> bytes;
    if (!entry || !ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<const void*>(entry),
                                     bytes.data(), bytes.size(), nullptr) ||
        std::memcmp(bytes.data(), prefix, sizeof(prefix)) ||
        std::memcmp(bytes.data() + bodyOffset, body, sizeof(body))) {
        return;
    }
    if (WindhawkUtils::SetFunctionHook(reinterpret_cast<OfficeLoadBitmap>(entry),
                                       OfficeLoadBitmapHook, &g_officeLoadBitmapOriginal) &&
        applyNow) {
        Wh_ApplyHookOperations();
    }
}

HCURSOR MakeTransparentCursor() {
    constexpr int size = 32;
    std::array<BYTE, size * size / 8> andMask;
    andMask.fill(0xff);
    const std::array<BYTE, size * size / 8> xorMask{};
    return CreateCursor(GetModuleHandleW(nullptr), 0, 0, size, size,
                        andMask.data(), xorMask.data());
}

HCURSOR WINAPI SetCursorHook(HCURSOR cursor) {
    try {
        const bool enabled = ReplacementEnabled();
        if (enabled && g_excelBitmapHookPending.load(std::memory_order_relaxed)) {
            TryHookExcelBitmapLoader(true);
        }
        if (g_excelMarker.bitmap && cursor == g_excelMarker.exitCursor) {
            g_excelMarker = {};
        }
        const bool hideNeutral = enabled && g_excelMarker.painted &&
            cursor == g_excelMarker.neutralCursor;
        size_t slot = kUnknownSlot;
        if (hideNeutral) {
            slot = SystemSlot(32654);
        } else if (enabled) {
            slot = IdentifyCursor(cursor);
            if (g_isExcel && slot != kUnknownSlot && kMappings[slot].role == Role::Move) {
                slot = kUnknownSlot;
            }
        }
        if (slot == kUnknownSlot && !g_needsCleanup.load()) {
            g_previousRole = Role::Count;
            return g_setCursorOriginal(cursor);
        }

        std::lock_guard lock(g_mutex);
        ForgetInactiveThreads();
        HCURSOR display = cursor;
        if (slot != kUnknownSlot && !g_stopping && ReplacementEnabled()) {
            const auto& mapping = kMappings[slot];
            const size_t role = static_cast<size_t>(mapping.role);
            if (!hideNeutral) {
                CheckCursorSettings();
            }
            if (hideNeutral || (g_previousRole == mapping.role && !g_needsRefresh[role]) ||
                RefreshSource(mapping.role)) {
                auto& entry = g_replacements[slot];
                if (entry.cursor && entry.original != cursor) {
                    g_retired.push_back(entry);
                    entry = {};
                }
                if (!entry.cursor) {
                    HCURSOR replacement;
                    if (hideNeutral) {
                        replacement = MakeTransparentCursor();
                    } else {
                        const CursorImage& source = *g_sources[role];
                        replacement = mapping.eighthTurns
                            ? MakeCursor(Rotate(source, mapping.eighthTurns))
                            : MakeCursor(source);
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
    const bool replaceInOffice = Wh_GetIntSetting(L"replaceInOffice") != 0;
    const bool smoothDiagonals = Wh_GetIntSetting(L"smoothDiagonals") != 0;
    std::lock_guard lock(g_mutex);
    if (g_smoothDiagonals != smoothDiagonals) {
        g_smoothDiagonals = smoothDiagonals;
        g_needsRefresh[static_cast<size_t>(Role::Up)] = true;
    }
    if ((!g_isOffice || replaceInOffice) && !g_handlesReady) {
        for (size_t i = 0; i < kMappings.size(); ++i) {
            g_nativeCursors[i] = LoadCursorW(nullptr,
                                             MAKEINTRESOURCEW(kMappings[i].systemId));
        }
        g_handlesReady = true;
    }
    g_replaceInOffice.store(replaceInOffice, std::memory_order_release);
}

}  // namespace

BOOL Wh_ModInit() {
    if (!GetModuleHandleW(L"user32.dll")) {
        return FALSE;
    }

    wchar_t processPath[MAX_PATH];
    const DWORD pathLength = GetModuleFileNameW(nullptr, processPath, MAX_PATH);
    if (!pathLength || pathLength >= MAX_PATH) {
        return FALSE;
    }
    const std::wstring_view path(processPath, pathLength);
    g_isExcel = HasModuleName(path, kExcelProcess);
    g_isOffice = std::any_of(kOfficeProcesses.begin(), kOfficeProcesses.end(),
                           [&](PCWSTR name) { return HasModuleName(path, name); });
    g_needsRefresh.fill(true);
    UpdateSettings();

    if (!WindhawkUtils::SetFunctionHook(SetCursor, SetCursorHook, &g_setCursorOriginal)) {
        CloseSettingsWatch();
        return FALSE;
    }
    if (g_isExcel && WindhawkUtils::SetFunctionHook(BitBlt, BitBltHook, &g_bitBltOriginal)) {
        g_excelBitmapHookPending.store(true, std::memory_order_relaxed);
        if (ReplacementEnabled()) {
            TryHookExcelBitmapLoader(false);
        }
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    UpdateSettings();
}

void Wh_ModBeforeUninit() {
    g_excelBitmapHookPending.store(false, std::memory_order_relaxed);
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
