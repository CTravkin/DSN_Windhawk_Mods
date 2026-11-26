import { execFileSync } from 'node:child_process';
import { mkdirSync, mkdtempSync, readFileSync, rmdirSync, rmSync, writeFileSync } from 'node:fs';
import { join } from 'node:path';
import assert from 'node:assert/strict';
import test from 'node:test';

const source = readFileSync(new URL('../Mod/modern-run-dialog-ui.wh.cpp', import.meta.url), 'utf8');
const compiler = process.env.WINDHAWK_CLANG ||
    'C:/Program Files/Windhawk/Compiler/bin/clang++.exe';

test('all four buttons share the same opt-out behavior', () => {
    for (const name of ['hideBrowse', 'hideRun', 'hideClose', 'hideSettings']) {
        assert.match(source, new RegExp(`^  - ${name}: false$`, 'm'));
    }

    const actionDefs = source.match(/enum class RunAction[\s\S]*?\nusing ActionWidths[^\n]*;/)?.[0];
    const applyVisibility = source.match(/constexpr ActionWidths VisibleActionWidths\([\s\S]*?\n\}/)?.[0];
    assert.ok(actionDefs && applyVisibility, 'Action widths are resolved by production code');
    assert.match(source, /int wBrowse = compactActions \? btnH/);
    assert.match(source, /drawActionButton\(layout\.rcBrowse, compactActions \? nullptr/);
    compileChecks(`
        #include <array>
        #include <cstddef>
        using std::size_t;
        ${actionDefs}
        ${applyVisibility}
        constexpr ActionWidths widths{86, 80, 76, 32};
        static_assert(VisibleActionWidths({}, widths) == widths);
        static_assert(VisibleActionWidths({true, false, false, false}, widths) == ActionWidths{0, 80, 76, 32});
        static_assert(VisibleActionWidths({false, true, false, false}, widths) == ActionWidths{86, 0, 76, 32});
        static_assert(VisibleActionWidths({false, false, true, false}, widths) == ActionWidths{86, 80, 0, 32});
        static_assert(VisibleActionWidths({false, false, false, true}, widths) == ActionWidths{86, 80, 76, 0});
        static_assert(VisibleActionWidths({true, true, true, true}, widths) == ActionWidths{});
    `);
});

test('layout settings preserve the prior Classic/Pill values and accept Fluent', () => {
    const schema = source.match(/- minimalMode: "0"[\s\S]*?(?=\n  - contextMenuStyle:)/)?.[0];
    assert.match(schema, /- minimalMode: "0"/);
    for (const [value, label] of [['0', 'Classic'], ['1', 'Compact Mini-Pill'], ['2', 'Fluent']]) {
        assert.match(schema, new RegExp(`^    - "${value}": ${label}$`, 'm'));
    }

    const mode = source.match(/enum class DialogLayout[^\n]*;/)?.[0];
    const resolver = source.match(/constexpr DialogLayout ResolveDialogLayout\([\s\S]*?\n\}/)?.[0];
    const override = source.match(/constexpr DialogLayout ApplyDialogLayoutOverride\([\s\S]*?\n\}/)?.[0];
    assert.ok(mode && resolver && override, 'The production layout resolver is present');
    compileChecks(`
            #include <string_view>
            ${mode}
            ${resolver}
            ${override}
            using enum DialogLayout;
            static_assert(ResolveDialogLayout(L"", 0) == Classic);
            static_assert(ResolveDialogLayout(L"", 1) == Pill);
            static_assert(ResolveDialogLayout(L"0", 1) == Classic);
            static_assert(ResolveDialogLayout(L"1", 0) == Pill);
            static_assert(ResolveDialogLayout(L"2", 0) == Fluent);
            static_assert(ApplyDialogLayoutOverride(Fluent, 0) == Classic);
            static_assert(ApplyDialogLayoutOverride(Classic, 1) == Pill);
            static_assert(ApplyDialogLayoutOverride(Classic, 2) == Fluent);
            static_assert(ApplyDialogLayoutOverride(Pill, -1) == Pill);
            static_assert(ApplyDialogLayoutOverride(Pill, 3) == Pill);
`);
});

test('the context menu retains its previous style by default', () => {
    const style = source.match(/- contextMenuStyle: mod[\s\S]*?(?=\n  - uiScalePercent:)/)?.[0];
    assert.match(style, /\$name: Context Menu Style/);
    assert.match(style, /- mod: Mod Style/);
    assert.match(style, /- windows_fluent: Windows \(Fluent\)/);
    assert.match(style, /\$description:ru-RU: Меняет оформление меню при клике правой кнопкой мыши/);
});

test('layout settings keep position and menu style directly below their related controls', () => {
    const layout = source.match(/^- layoutGroup:[\s\S]*?(?=^- actionButtonsGroup:)/m)?.[0];
    assert.ok(layout, 'The layout settings group is present');
    const keys = Array.from(layout.matchAll(/^  - (\w+):/gm), match => match[1]);
    assert.deepEqual(keys.slice(0, 5), ['dialogPosition', 'rememberPosition', 'minimalMode', 'contextMenuStyle', 'uiScalePercent']);
    assert.ok(!keys.includes('contextMenuBorderMode'));
    assert.match(layout, /\$name:ru-RU: Макет окна/);
    assert.match(layout, /\$description:ru-RU: Базовая компоновка окна, использующая параметры персонализации/);
});

test('history follows the Windows short date and short time formats', () => {
    const formatter = source.match(/std::wstring FormatHistoryTimestamp\([\s\S]*?\n\}/)?.[0];
    assert.ok(formatter, 'The production formatter is present');
    compileChecks(`
        #include <windows.h>
        #include <cwchar>
        #include <string>
        #include <cassert>
        ${formatter}
        int main() {
            SYSTEMTIME past{2024, 11, 0, 25, 9, 5, 22, 0};
            SYSTEMTIME utcTime{};
            FILETIME utc{};
            assert(TzSpecificLocalTimeToSystemTimeEx(nullptr, &past, &utcTime));
            assert(SystemTimeToFileTime(&utcTime, &utc));
            wchar_t pattern[128]{}, time[256]{}, date[256]{};
            assert(GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SSHORTTIME, pattern, 128));
            assert(GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &past, pattern, time, 256));
            assert(GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &past, nullptr, date, 256, nullptr));
            assert(FormatHistoryTimestamp(utc, L"Today") == std::wstring(date) + L" " + time);
            GetSystemTimeAsFileTime(&utc);
            assert(FileTimeToSystemTime(&utc, &utcTime));
            assert(SystemTimeToTzSpecificLocalTimeEx(nullptr, &utcTime, &past));
            assert(GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &past, pattern, time, 256));
            assert(FormatHistoryTimestamp(utc, L"Today") == std::wstring(L"Today, ") + time);
        }
    `, true);
});

test('inner corners support auto, square and a radius bounded by the row', () => {
    assert.match(source, /- customInnerCornerRadius: "-1"/);
    const parser = source.match(/int ParseInnerCornerRadius\([\s\S]*?\n\}/)?.[0];
    const radius = source.match(/constexpr float ClampCornerRadius\([\s\S]*?\n\}/)?.[0];
    assert.ok(parser && radius, 'The settings parser and the rendering clamp are present');
    compileChecks(`
        #include <algorithm>
        #include <cassert>
        #include <cwchar>
        #include <string>
        ${parser}
        ${radius}
        static_assert(ClampCornerRadius(0, 100, 30) == 0);
        static_assert(ClampCornerRadius(6, 100, 30) == 6);
        static_assert(ClampCornerRadius(28, 100, 30) == 15);
        static_assert(ClampCornerRadius(12, 8, 30) == 4);
        int main() {
            assert(ParseInnerCornerRadius(L"") == -1);
            assert(ParseInnerCornerRadius(L"-1") == -1);
            assert(ParseInnerCornerRadius(L"0") == 0);
            assert(ParseInnerCornerRadius(L"7") == 7);
            assert(ParseInnerCornerRadius(L"28") == 28);
            assert(ParseInnerCornerRadius(L"29") == -1);
            assert(ParseInnerCornerRadius(L"12abc") == -1);
        }
    `, true);
});

test('vertical panel insets center controls with at most a one-pixel difference', () => {
    const inset = source.match(/constexpr int GetVerticalPanelInset\([\s\S]*?\n\}/)?.[0];
    assert.ok(inset, 'History and the input bar use the shared padding calculation');
    compileChecks(`
        ${inset}
        static_assert(GetVerticalPanelInset(54, 32, false) == 11);
        static_assert(GetVerticalPanelInset(54, 32, true) == 11);
        static_assert(GetVerticalPanelInset(59, 36, false) == 11);
        static_assert(GetVerticalPanelInset(59, 36, true) == 12);
        static_assert(GetVerticalPanelInset(96, 46, false) == 25);
        static_assert(GetVerticalPanelInset(96, 46, true) == 25);
    `);
});

test('history with one row has the same height as the input panel across scales', () => {
    const bar = source.match(/int GetInputBarHeight\([\s\S]*?constexpr int GetVerticalPanelInset\([\s\S]*?\n\}/)?.[0];
    const list = source.match(/int GetListPadding\([\s\S]*?int GetRowHeight\([\s\S]*?\n\}/)?.[0];
    assert.ok(bar && list, 'The production layout calculations are present');
    compileChecks(`
        #include <algorithm>
        #include <cassert>
        using UINT = unsigned;
        constexpr int kBaseInputBarHeight = 54;
        constexpr int kBasePanelInset = 11;
        constexpr int kBaseRowHighlightInset = 2;
        struct RunDialogState {
            struct { int customBarHeight; } settings;
            UINT dpi;
            int scale;
        };
        int MulDiv(int value, int numerator, int denominator) {
            return (value * numerator + denominator / 2) / denominator;
        }
        int DpiScale(int value, UINT dpi, int scale = 100) {
            return MulDiv(value, dpi * scale, 9600);
        }
        int DpiScaleS(int value, const RunDialogState* state) {
            return DpiScale(value, state->dpi, state->scale);
        }
        ${bar}
        ${list}
        int main() {
            const RunDialogState profiles[] = {
                {{0}, 96, 100}, {{0}, 96, 85}, {{0}, 144, 120},
                {{36}, 120, 100}, {{70}, 96, 100}, {{96}, 192, 85}
            };
            for (const auto& profile : profiles) {
                const auto* state = &profile;
                const auto historyHeight = GetListPadding(state) + GetRowHeight(state) + GetListPadding(state, true);
                assert(historyHeight == GetInputBarHeight(state));
            }
        }
    `, true);
});

test('a saved explicit accent becomes the Windows border mode', () => {
    const normalize = source.match(/constexpr std::wstring_view NormalizeBorderMode\([\s\S]*?\n\}/)?.[0];
    assert.ok(normalize);
    compileChecks(`
        #include <string_view>
        ${normalize}
        static_assert(NormalizeBorderMode(L"accent") == L"windows");
        static_assert(NormalizeBorderMode(L"windows") == L"windows");
        static_assert(NormalizeBorderMode(L"off") == L"off");
        static_assert(NormalizeBorderMode(L"own") == L"own");
    `);
});

test('window border preferences respect high contrast', () => {
    const mode = source.match(/enum class WindowBorderMode[^\n]*;/)?.[0];
    const resolve = source.match(/constexpr WindowBorderMode ResolveWindowBorderMode\([\s\S]*?\n\}/)?.[0];
    assert.ok(mode && resolve, 'The window border policy is present');
    compileChecks(`
        ${mode}
        ${resolve}
        using enum WindowBorderMode;
        static_assert(ResolveWindowBorderMode(Mod, false) == Mod);
        static_assert(ResolveWindowBorderMode(Mod, true) == Off);
        static_assert(ResolveWindowBorderMode(Windows, false) == Windows);
        static_assert(ResolveWindowBorderMode(Windows, true) == Windows);
        static_assert(ResolveWindowBorderMode(Off, false) == Off);
        static_assert(ResolveWindowBorderMode(Off, true) == Off);
    `);
});

test('Fluent surface and flyout outlines take the accent only for active windows with the Windows setting enabled', () => {
    const color = source.match(/D2D1_COLOR_F GetWindowsBorderColor\([\s\S]*?\n\}/)?.[0];
    assert.ok(color);
    compileChecks(`
        #include <windows.h>
        #include <d2d1.h>
        #include <cassert>
        #include <initializer_list>
        static D2D1_COLOR_F GetSystemAccentColorF() { return D2D1::ColorF(0x123456); }
        static bool accentEnabled = false;
        static bool WindowsUsesAccentBorder() { return accentEnabled; }
        ${color}
        int main() {
            const auto panel = GetWindowsBorderColor(false, true, false, false);
            assert(panel.r == 117.0f / 255 && panel.g == panel.r && panel.b == panel.r);
            assert(panel.a == 102.0f / 255);
            const auto darkFlyout = GetWindowsBorderColor(false, true, false, true);
            const auto lightFlyout = GetWindowsBorderColor(false, true, true, true);
            assert(darkFlyout.r == 0 && darkFlyout.a == 51.0f / 255);
            assert(lightFlyout.r == 0 && lightFlyout.a == 15.0f / 255);
            accentEnabled = true;
            for (const bool flyout : {false, true}) {
                const auto accent = GetWindowsBorderColor(false, true, true, flyout);
                assert(accent.r == GetSystemAccentColorF().r && accent.a == 1.0f);
                const auto inactive = GetWindowsBorderColor(false, false, true, flyout);
                assert(inactive.a == (flyout ? lightFlyout.a : panel.a));
            }
            const auto contrast = GetWindowsBorderColor(true, true, true, false);
            assert(contrast.r == GetRValue(GetSysColor(COLOR_WINDOWTEXT)) / 255.0f && contrast.a == 1.0f);
        }
    `, true);
});

test('the border settings are grouped in color, window, and context menu order', () => {
    const group = source.match(/^\- windowBordersGroup:[\s\S]*?(?=^\- layoutGroup:)/m)?.[0];
    assert.ok(group, 'The window borders group is present');
    const keys = Array.from(group.matchAll(/^  - (\w+):/gm), match => match[1]);
    assert.deepEqual(keys, ['customBorderColor', 'windowBorderMode', 'contextMenuBorderMode']);
});

test('all localized border lists omit the explicit accent and preserve the option order', () => {
    const group = source.match(/^\- windowBordersGroup:[\s\S]*?(?=^\- layoutGroup:)/m)?.[0];
    const windowOptions = group.slice(group.indexOf('  - windowBorderMode:'), group.indexOf('  - contextMenuBorderMode:'));
    const menuOptions = group.slice(group.indexOf('  - contextMenuBorderMode:'));
    for (const [entry, keys] of [
        [windowOptions, ['mod', 'windows', 'off']],
        [menuOptions, ['own', 'window', 'windows', 'off']],
    ]) {
        const localizedLists = entry.split(/^    \$options[^\n]*\n/m).slice(1);
        assert.equal(localizedLists.length, 21);
        for (const list of localizedLists) {
            assert.deepEqual(Array.from(list.matchAll(/^    - (\w+):/gm), match => match[1]), keys);
        }
    }
});

test('context menus can keep their own border or override the window border policy', () => {
    const modes = source.match(/enum class WindowBorderMode[^\n]*;\s*enum class ContextMenuBorderMode[^\n]*;/)?.[0];
    const resolve = source.match(/constexpr std::optional<WindowBorderMode> ResolveContextMenuWindowBorderMode\([\s\S]*?\n\}/)?.[0];
    assert.ok(modes && resolve);
    assert.match(source, /- contextMenuBorderMode: own/);
    compileChecks(`
        #include <optional>
        ${modes}
        ${resolve}
        using Menu = ContextMenuBorderMode;
        using Window = WindowBorderMode;
        static_assert(!ResolveContextMenuWindowBorderMode(Menu::Own, Window::Off));
        static_assert(ResolveContextMenuWindowBorderMode(Menu::Window, Window::Mod) == Window::Mod);
        static_assert(ResolveContextMenuWindowBorderMode(Menu::Window, Window::Off) == Window::Off);
        static_assert(ResolveContextMenuWindowBorderMode(Menu::Windows, Window::Off) == Window::Windows);
        static_assert(ResolveContextMenuWindowBorderMode(Menu::Off, Window::Windows) == Window::Off);
    `);
});

test('centered rows split spare space and keep dropdown input within its rows', () => {
    const inset = source.match(/constexpr float GetCenteredInset\([\s\S]*?\n\}/)?.[0];
    const hit = source.match(/constexpr int GetDropdownRowIndex\([\s\S]*?\n\}/)?.[0];
    assert.ok(inset && hit);
    compileChecks(`
        ${inset}
        ${hit}
        static_assert(GetCenteredInset(36, 32) == 2);
        static_assert(GetCenteredInset(67, 32) == 17.5f);
        static_assert(GetCenteredInset(32, 36) == 0);
        static_assert(GetDropdownRowIndex(4, 0, 5, 36, 3) == -1);
        static_assert(GetDropdownRowIndex(5, 0, 5, 36, 3) == 0);
        static_assert(GetDropdownRowIndex(41, 0, 5, 36, 3) == 1);
        static_assert(GetDropdownRowIndex(113, 0, 5, 36, 3) == -1);
        static_assert(GetDropdownRowIndex(7, 45, 6, 45, 4) == 1);
    `);
});

test('DirectWrite keeps the rendered history text centered on the row', () => {
    const center = source.match(/float GetCenteredInkTop\([\s\S]*?\n\}/)?.[0];
    const draw = source.match(/void DrawCenteredTextLayout\([\s\S]*?\n\}/)?.[0];
    assert.ok(center && draw, 'The history and other surfaces use the same text renderer');
    compileChecks(String.raw`
        #include <windows.h>
        #include <d2d1.h>
        #include <dwrite.h>
        #include <cassert>
        #include <cmath>
        #include <cstdint>
        #include <cwchar>
        #include <optional>

        ${center}
        ${draw}

        int main() {
            constexpr int width = 420, height = 80;
            IDWriteFactory* textFactory = nullptr;
            assert(SUCCEEDED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                reinterpret_cast<IUnknown**>(&textFactory))));
            ID2D1Factory* graphicsFactory = nullptr;
            assert(SUCCEEDED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &graphicsFactory)));
            const auto dc = CreateCompatibleDC(nullptr);
            BITMAPINFO info{};
            info.bmiHeader = {sizeof(BITMAPINFOHEADER), width, -height, 1, 32, BI_RGB};
            void* pixels = nullptr;
            const auto bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, &pixels, nullptr, 0);
            assert(bitmap && pixels);
            const auto previous = SelectObject(dc, bitmap);
            ID2D1DCRenderTarget* target = nullptr;
            const auto properties = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_SOFTWARE,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
            assert(SUCCEEDED(graphicsFactory->CreateDCRenderTarget(&properties, &target)));
            const RECT client{0, 0, width, height};
            assert(SUCCEEDED(target->BindDC(dc, &client)));
            target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
            IDWriteRenderingParams* rendering = nullptr;
            assert(SUCCEEDED(textFactory->CreateCustomRenderingParams(1.0f, 0.0f, 0.0f,
                DWRITE_PIXEL_GEOMETRY_FLAT, DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC, &rendering)));
            target->SetTextRenderingParams(rendering);
            ID2D1SolidColorBrush* brush = nullptr;
            assert(SUCCEEDED(target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &brush)));

            const wchar_t* examples[] = {
                L"main.cpl   \u2013   Mouse Properties", L"\u0421\u0435\u0433\u043e\u0434\u043d\u044f, 16:11",
                L"Ctrl+O", L"\u8f93\u5165\u547d\u4ee4", L"\u0627\u0633\u0645 \u0627\u0644\u0623\u0645\u0631"
            };
            constexpr float scales[]{1.0f, 1.25f, 1.5f};
            for (const float scale : scales) {
                for (int i = 0; i < 5; ++i) {
                    IDWriteTextFormat* format = nullptr;
                    const float size = (i == 1 ? 12.0f : 13.5f) * scale;
                    assert(SUCCEEDED(textFactory->CreateTextFormat(L"Segoe UI Variable Text", nullptr,
                        DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, L"", &format)));
                    format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                    format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                    IDWriteTextLayout* layout = nullptr;
                    const auto bounds = D2D1::RectF(8, 8 * scale, width - 8, 40 * scale);
                    assert(SUCCEEDED(textFactory->CreateTextLayout(examples[i], static_cast<UINT32>(wcslen(examples[i])),
                        format, bounds.right - bounds.left, bounds.bottom - bounds.top, &layout)));
                    if (i == 0) layout->SetFontWeight(DWRITE_FONT_WEIGHT_SEMI_BOLD, {0, 8});

                    target->BeginDraw();
                    target->Clear(D2D1::ColorF(D2D1::ColorF::Black));
                    DrawCenteredTextLayout(target, layout, bounds, brush);
                    assert(SUCCEEDED(target->EndDraw()));
                    GdiFlush();
                    int top = height, bottom = -1;
                    const auto image = static_cast<const uint32_t*>(pixels);
                    for (int y = 0; y < height; ++y) {
                        for (int x = 0; x < width; ++x) {
                            if ((image[y * width + x] & 255) <= 16) continue;
                            if (y < top) top = y;
                            if (y > bottom) bottom = y;
                        }
                    }
                    assert(top <= bottom && std::abs((top + bottom + 1) * 0.5f - 24.0f * scale) <= 0.5f);
                    layout->Release();
                    format->Release();
                }
            }

            brush->Release();
            rendering->Release();
            target->Release();
            SelectObject(dc, previous);
            DeleteObject(bitmap);
            DeleteDC(dc);
            graphicsFactory->Release();
            textFactory->Release();
        }
    `, true, ['-ld2d1', '-ldwrite', '-lgdi32']);
});

test('history actions keep delete in the same group', () => {
    const deleteItem = source.match(/if \(canDeleteFromHistory\) \{([\s\S]*?)ctxItems\.push_back\(std::move\(delItem\)\)/)?.[1];
    assert.ok(deleteItem);
    assert.doesNotMatch(deleteItem, /isSeparator/);
});

function compileChecks(checks, run = false, libraries = []) {
    const buildRoot = join(process.cwd(), 'Builds/Modern_Run_Dialog_UI');
    mkdirSync(buildRoot, { recursive: true });
    const build = mkdtempSync(join(buildRoot, 'layout-test-'));
    const translationUnit = join(build, 'layout.cpp');
    const executable = join(build, 'layout.exe');
    try {
        writeFileSync(translationUnit, checks);
        const flags = run ? ['-D_WIN32_WINNT=0x0A00', '-static', '-o', executable] : ['-fsyntax-only'];
        execFileSync(compiler, ['-std=c++23', ...flags, translationUnit, ...libraries], { stdio: 'pipe' });
        if (run) execFileSync(executable, { stdio: 'pipe' });
    } finally {
        rmSync(executable, { force: true });
        rmSync(join(build, 'layout.cpp'), { force: true });
        rmdirSync(build);
    }
}
