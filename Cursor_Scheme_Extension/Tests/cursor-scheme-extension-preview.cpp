#include <windows.h>
#include <windowsx.h>

#include <algorithm>
#include <array>
#include <cstdlib>

namespace {

struct Pane {
    PCWSTR title;
    UINT neutral;
    DWORD readerFlags;
    bool horizontal;
    bool vertical;
};

constexpr std::array kPanes = {
    Pane{L"Vertical", 32652, 0x02, false, true},
    Pane{L"Horizontal", 32653, 0x04, true, false},
    Pane{L"Both axes", 32654, 0, true, true},
};

// DoReaderMode is exported from Comctl32 by ordinal 383.
struct ReaderModeInfo {
    UINT size;
    HWND window;
    DWORD flags;
    RECT* bounds;
    BOOL(CALLBACK* scroll)(ReaderModeInfo*, int, int);
    BOOL(CALLBACK* translate)(const MSG*);
    LPARAM data;
};

using DoReaderMode = void(WINAPI*)(ReaderModeInfo*);

DoReaderMode g_readerMode;
std::array<RECT, kPanes.size()> g_bounds;
int g_activePane = -1;
bool g_useReaderMode = false;
bool g_usingCapture = false;
POINT g_origin{};
UINT g_cursor = 32654;
int g_scrollX = 0;
int g_scrollY = 0;

void SelectSystemCursor(UINT id) {
    SetCursor(LoadCursorW(nullptr, MAKEINTRESOURCEW(id)));
}

void StopPreview(HWND window) {
    g_activePane = -1;
    if (g_usingCapture) {
        g_usingCapture = false;
        ReleaseCapture();
    }
    SelectSystemCursor(32512);
    InvalidateRect(window, nullptr, FALSE);
}

UINT Direction(const Pane& pane, int x, int y) {
    constexpr int kDeadZone = 12;
    const int horizontal = pane.horizontal && std::abs(x) > kDeadZone ? x : 0;
    const int vertical = pane.vertical && std::abs(y) > kDeadZone ? y : 0;
    if (vertical < 0) {
        return horizontal < 0 ? 32659 : horizontal > 0 ? 32660 : 32655;
    }
    if (vertical > 0) {
        return horizontal < 0 ? 32661 : horizontal > 0 ? 32662 : 32656;
    }
    return horizontal < 0 ? 32657 : horizontal > 0 ? 32658 : pane.neutral;
}

BOOL CALLBACK ReaderScroll(ReaderModeInfo* info, int x, int y) {
    g_scrollX += x;
    g_scrollY += y;
    InvalidateRect(info->window, nullptr, FALSE);
    return TRUE;
}

void StartPreview(HWND window, POINT point) {
    for (size_t i = 0; i < kPanes.size(); ++i) {
        if (PtInRect(&g_bounds[i], point)) {
            g_activePane = static_cast<int>(i);
            break;
        }
    }

    if (g_activePane < 0) {
        return;
    }

    const auto& pane = kPanes[g_activePane];
    g_origin = point;
    g_scrollX = 0;
    g_scrollY = 0;

    if (g_useReaderMode && g_readerMode) {
        ReaderModeInfo info{sizeof(info), window, pane.readerFlags,
                            &g_bounds[g_activePane], ReaderScroll, nullptr, 0};
        g_readerMode(&info);
        StopPreview(window);
        return;
    }

    g_usingCapture = true;
    g_cursor = pane.neutral;
    SetCapture(window);
    SelectSystemCursor(g_cursor);
    InvalidateRect(window, nullptr, FALSE);
}

void DrawWindow(HWND window) {
    PAINTSTRUCT paint{};
    HDC dc = BeginPaint(window, &paint);
    RECT client{};
    GetClientRect(window, &client);
    FillRect(dc, &client, GetSysColorBrush(COLOR_WINDOW));
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, GetSysColor(COLOR_WINDOWTEXT));
    SelectObject(dc, GetStockObject(DEFAULT_GUI_FONT));

    RECT top{14, 12, client.right - 14, 44};
    DrawTextW(dc, L"Wheel-click a pane, move the mouse, then click again or press Esc.",
              -1, &top, DT_LEFT | DT_SINGLELINE);
    top.top = 35;
    const PCWSTR mode = g_useReaderMode
        ? L"F2: Direct system IDs (current: Windows Reader Mode)"
        : g_readerMode
            ? L"F2: Windows Reader Mode (current: Direct system IDs)"
            : L"Direct system IDs. Windows Reader Mode is unavailable.";
    DrawTextW(dc, mode, -1, &top, DT_LEFT | DT_SINGLELINE | DT_NOCLIP);

    const int paneWidth = (client.right - 48) / static_cast<int>(kPanes.size());
    for (size_t i = 0; i < kPanes.size(); ++i) {
        const int x = 12 + paneWidth * static_cast<int>(i) + 6 * static_cast<int>(i);
        auto& pane = g_bounds[i];
        pane = {x, 72, x + paneWidth, client.bottom - 14};
        FillRect(dc, &pane, GetSysColorBrush(COLOR_BTNFACE));
        FrameRect(dc, &pane, GetSysColorBrush(COLOR_WINDOWFRAME));
        RECT text = pane;
        text.top += 12;
        DrawTextW(dc, kPanes[i].title, -1, &text, DT_CENTER | DT_SINGLELINE);
        if (g_activePane == static_cast<int>(i)) {
            wchar_t status[85];
            if (g_usingCapture) {
                swprintf_s(status, L"System cursor ID %u", g_cursor);
            } else {
                swprintf_s(status, L"Scroll offset %d, %d", g_scrollX, g_scrollY);
            }
            text.top += 28;
            DrawTextW(dc, status, -1, &text, DT_CENTER | DT_SINGLELINE);
        }
    }

    EndPaint(window, &paint);
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_PAINT:
            DrawWindow(window);
            return 0;

        case WM_MBUTTONDOWN:
            if (g_activePane >= 0) {
                StopPreview(window);
            } else {
                StartPreview(window, {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
            }
            return 0;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
            if (g_activePane >= 0) {
                StopPreview(window);
            }
            return 0;

        case WM_MOUSEMOVE:
            if (g_usingCapture) {
                g_cursor = Direction(kPanes[g_activePane],
                                     GET_X_LPARAM(lParam) - g_origin.x,
                                     GET_Y_LPARAM(lParam) - g_origin.y);
                SelectSystemCursor(g_cursor);
                InvalidateRect(window, nullptr, FALSE);
            }
            return 0;

        case WM_SETCURSOR:
            if (g_usingCapture) {
                SelectSystemCursor(g_cursor);
                return TRUE;
            }
            break;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE && g_activePane >= 0) {
                StopPreview(window);
                return 0;
            }
            if (wParam == VK_F2 && g_readerMode && g_activePane < 0) {
                g_useReaderMode = !g_useReaderMode;
                InvalidateRect(window, nullptr, FALSE);
                return 0;
            }
            break;

        case WM_CAPTURECHANGED:
            if (g_usingCapture && reinterpret_cast<HWND>(lParam) != window) {
                g_usingCapture = false;
                StopPreview(window);
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

}  // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR, int show) {
    HMODULE commonControls = LoadLibraryExW(L"comctl32.dll", nullptr,
                                            LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (commonControls) {
        auto address = GetProcAddress(commonControls, MAKEINTRESOURCEA(383));
        g_readerMode = reinterpret_cast<DoReaderMode>(address);
    }

    constexpr wchar_t kClass[] = L"CursorSchemeExtensionPreview";
    WNDCLASSW cls{};
    cls.hInstance = instance;
    cls.lpszClassName = kClass;
    cls.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    cls.lpfnWndProc = WindowProc;
    cls.style = CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClassW(&cls)) {
        return 1;
    }

    HWND window = CreateWindowExW(0, kClass, L"Cursor Scheme Extension - Preview",
                                  WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                  CW_USEDEFAULT, CW_USEDEFAULT, 720, 250,
                                  nullptr, nullptr, instance, nullptr);
    if (!window) {
        return 1;
    }

    ShowWindow(window, show);
    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    if (commonControls) {
        FreeLibrary(commonControls);
    }

    return 0;
}
