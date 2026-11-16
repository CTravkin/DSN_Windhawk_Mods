// ==WindhawkMod==
// @id              cursor-scheme-extension
// @name            Cursor Scheme Extension
// @description     Use the active Windows cursor scheme for native middle-button autoscroll
// @description:ru-RU Применение активной схемы указателей Windows к штатной автопрокрутке
// @version         0.2.0
// @author          CTravkin
// @github          https://github.com/CTravkin
// @homepage        https://ctravkin.ru
// @license         MIT
// @include         cursor-scheme-extension-preview.exe
// @include         excel.exe
// @include         winword.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -ladvapi32
// ==/WindhawkMod==
// Build: 2

// ==WindhawkModReadme==
/*
# Cursor Scheme Extension

Uses the active Windows cursor scheme for native middle-button autoscroll. At
the starting point, the pointer shows the scrolling axes indicated by the
application. While scrolling, the scheme's Alternate select arrow points in the
scroll direction.

The prototype runs in the preview window, Excel, and Word. It recognizes
autoscroll cursor resources from Windows User32, Common Controls, and Excel.
An application can also draw a separate stationary origin marker.
*/
// ==/WindhawkModReadme==

#include <windows.h>
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

constexpr std::array kExcelResources = {
    ResourceMapping{256, 32654},
    ResourceMapping{293, 32655}, ResourceMapping{294, 32660},
    ResourceMapping{295, 32658}, ResourceMapping{296, 32662},
    ResourceMapping{297, 32656}, ResourceMapping{298, 32661},
    ResourceMapping{299, 32657}, ResourceMapping{300, 32659},
    ResourceMapping{353, 32655}, ResourceMapping{354, 32660},
    ResourceMapping{355, 32658}, ResourceMapping{356, 32662},
    ResourceMapping{357, 32656}, ResourceMapping{358, 32661},
    ResourceMapping{359, 32657}, ResourceMapping{360, 32659},
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

std::array<HCURSOR, kMappings.size()> g_nativeCursors;
std::array<HCURSOR, kExcelResources.size()> g_excelCursors;
std::array<Replacement, kMappings.size()> g_replacements;
std::array<std::optional<CursorImage>, kRoleCount> g_sources;
std::array<bool, kRoleCount> g_needsRefresh;
std::vector<Replacement> g_retired;
std::vector<ThreadSelection> g_selections;
std::mutex g_mutex;
std::atomic<bool> g_hasReplacements;
bool g_stopping;
bool g_isExcel;
HKEY g_cursorsKey;
HANDLE g_changeEvent;
thread_local Role g_previousRole = Role::Count;
decltype(&SetCursor) g_setCursorOriginal;

size_t SystemSlot(WORD id) {
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

size_t ReadResourceSlot(HCURSOR cursor) {
    ICONINFOEXW info{sizeof(info)};
    if (!GetIconInfoExW(cursor, &info)) {
        return kUnknownSlot;
    }
    if (info.hbmMask) {
        DeleteObject(info.hbmMask);
    }
    if (info.hbmColor) {
        DeleteObject(info.hbmColor);
    }
    if (info.fIcon || !info.wResID) {
        return kUnknownSlot;
    }

    if (HasModuleName(info.szModName, L"USER32") ||
        HasModuleName(info.szModName, L"user32.dll")) {
        return SystemSlot(info.wResID);
    }
    if (g_isExcel && HasModuleName(info.szModName, L"excel.exe")) {
        return ResourceSlot(info.wResID, kExcelResources);
    }
    if (HasModuleName(info.szModName, L"comctl32.dll")) {
        return ResourceSlot(info.wResID, kCommonControlsResources);
    }
    return kUnknownSlot;
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
    if (g_isExcel) {
        for (size_t i = 0; i < g_excelCursors.size(); ++i) {
            if (g_excelCursors[i] == cursor) {
                return SystemSlot(kExcelResources[i].systemId);
            }
        }
    }

    // Applications repeatedly set the same cursor while the mouse moves.
    // Cache only consecutive lookups to keep recycled handles out of a long-lived cache.
    thread_local HCURSOR previous;
    thread_local size_t previousSlot = kUnknownSlot;
    if (cursor != previous) {
        previousSlot = ReadResourceSlot(cursor);
        previous = cursor;
    }
    return previousSlot;
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

std::optional<CursorImage> ReadCursor(HCURSOR cursor, bool prepareDiagonals) {
    IconBitmaps info;
    if (!cursor || !GetIconInfo(cursor, &info) || info.fIcon || !info.hbmMask) {
        return std::nullopt;
    }

    BITMAP mask{};
    if (!GetObjectW(info.hbmMask, sizeof(mask), &mask)) {
        return std::nullopt;
    }

    CursorImage image;
    image.width = mask.bmWidth;
    image.height = info.hbmColor ? mask.bmHeight : mask.bmHeight / 2;
    if (image.width < 1 || image.width > kMaxSourceSize || image.height < 1 ||
        image.height > kMaxSourceSize || info.xHotspot >= DWORD(image.width) ||
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

void CheckCursorSettings() {
    if (g_changeEvent && WaitForSingleObject(g_changeEvent, 0) == WAIT_OBJECT_0) {
        g_needsRefresh.fill(true);
        if (!WatchCursorSettings()) {
            CloseHandle(g_changeEvent);
            g_changeEvent = nullptr;
        }
    }
}

bool RefreshSource(Role role) {
    const size_t index = static_cast<size_t>(role);
    auto image = ReadCursor(LoadCursorW(nullptr, kRoleIds[index]), role == Role::Up);
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

HCURSOR WINAPI SetCursorHook(HCURSOR cursor) {
    const size_t slot = IdentifyCursor(cursor);

    if (slot == kUnknownSlot && !g_hasReplacements.load()) {
        g_previousRole = Role::Count;
        return g_setCursorOriginal(cursor);
    }

    try {
        std::lock_guard lock(g_mutex);
        ForgetInactiveThreads();
        HCURSOR display = cursor;
        if (slot != kUnknownSlot && !g_stopping) {
            const auto& mapping = kMappings[slot];
            const size_t role = static_cast<size_t>(mapping.role);
            CheckCursorSettings();
            if ((g_previousRole == mapping.role && !g_needsRefresh[role]) ||
                RefreshSource(mapping.role)) {
                auto& entry = g_replacements[slot];
                if (entry.cursor && entry.original != cursor) {
                    g_retired.push_back(entry);
                    entry = {};
                }
                if (!entry.cursor) {
                    const CursorImage& source = *g_sources[role];
                    const HCURSOR replacement = mapping.eighthTurns
                        ? MakeCursor(Rotate(source, mapping.eighthTurns))
                        : MakeCursor(source);
                    if (replacement) {
                        entry = {replacement, cursor};
                        g_hasReplacements.store(true);
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
        return previous;
    } catch (...) {
        g_previousRole = Role::Count;
        return g_setCursorOriginal(cursor);
    }
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

}  // namespace

BOOL Wh_ModInit() {
    if (!GetModuleHandleW(L"user32.dll")) {
        return FALSE;
    }

    for (size_t i = 0; i < kMappings.size(); ++i) {
        g_nativeCursors[i] = LoadCursorW(nullptr, MAKEINTRESOURCEW(kMappings[i].systemId));
    }
    wchar_t processPath[MAX_PATH];
    const DWORD pathLength = GetModuleFileNameW(nullptr, processPath, MAX_PATH);
    g_isExcel = pathLength && pathLength < MAX_PATH &&
                HasModuleName({processPath, pathLength}, L"excel.exe");
    if (g_isExcel) {
        HMODULE excel = GetModuleHandleW(nullptr);
        for (size_t i = 0; i < kExcelResources.size(); ++i) {
            g_excelCursors[i] = LoadCursorW(excel, MAKEINTRESOURCEW(kExcelResources[i].id));
        }
    }
    g_needsRefresh.fill(true);

    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Cursors", 0,
                       KEY_NOTIFY, &g_cursorsKey) == ERROR_SUCCESS) {
        g_changeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!g_changeEvent || !WatchCursorSettings()) {
            CloseSettingsWatch();
        }
    }

    if (!WindhawkUtils::SetFunctionHook(SetCursor, SetCursorHook, &g_setCursorOriginal)) {
        CloseSettingsWatch();
        return FALSE;
    }
    return TRUE;
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
