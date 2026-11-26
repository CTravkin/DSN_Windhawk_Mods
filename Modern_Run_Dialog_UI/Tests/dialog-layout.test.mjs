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
    assert.deepEqual(keys.slice(0, 6), ['dialogPosition', 'rememberPosition', 'minimalMode', 'contextMenuStyle', 'dialogWidthPreset', 'uiScalePercent']);
    assert.ok(!keys.includes('contextMenuBorderMode'));
    assert.match(layout, /\$name:ru-RU: Макет окна/);
    assert.match(layout, /\$description:ru-RU: Базовая компоновка окна, использующая параметры персонализации/);
});

test('all width presets reserve 100 pixels for input beside visible controls', () => {
    const layout = source.match(/enum class DialogLayout[^\n]*;/)?.[0];
    const preset = source.match(/enum class DialogWidthPreset[^\n]*;/)?.[0];
    const desired = source.match(/constexpr int GetLayoutWidthDip\([\s\S]*?\n\}/)?.[0];
    const fit = source.match(/constexpr int FitDialogWidth\([\s\S]*?\n\}/)?.[0];
    assert.ok(layout && preset && desired && fit);
    compileChecks(`
        #include <algorithm>
        ${layout}
        ${preset}
        ${desired}
        ${fit}
        static_assert(GetLayoutWidthDip(DialogLayout::Fluent, DialogWidthPreset::Layout) == 500);
        static_assert(GetLayoutWidthDip(DialogLayout::Fluent, DialogWidthPreset::Narrow) == 425);
        static_assert(GetLayoutWidthDip(DialogLayout::Fluent, DialogWidthPreset::Wide) == 575);
        static_assert(GetLayoutWidthDip(DialogLayout::Classic, DialogWidthPreset::Layout) == 600);
        static_assert(GetLayoutWidthDip(DialogLayout::Classic, DialogWidthPreset::Narrow) == 510);
        static_assert(GetLayoutWidthDip(DialogLayout::Classic, DialogWidthPreset::Wide) == 690);
        static_assert(GetLayoutWidthDip(DialogLayout::Pill, DialogWidthPreset::Narrow) == 306);
        static_assert(GetLayoutWidthDip(DialogLayout::Pill, DialogWidthPreset::Wide) == 414);
        static_assert(FitDialogWidth(420, 467, 85) == 567);
        static_assert(FitDialogWidth(500, 467, 100) == 567);
        static_assert(FitDialogWidth(520, 467, 150) == 617);
        static_assert(FitDialogWidth(780, 467, 100) == 780);
    `);
});

test('the inline shield uses the standard gap before any visible action', () => {
    const layout = source.match(/enum class DialogLayout[^\n]*;/)?.[0];
    const preset = source.match(/enum class DialogWidthPreset[^\n]*;/)?.[0];
    const actions = source.match(/enum class RunAction[\s\S]*?\nusing ActionWidths[^\n]*;/)?.[0];
    const rects = source.match(/struct ActionButtonsLayout \{[\s\S]*?\n\};/)?.[0];
    const functions = ['GetLayoutWidthDip', 'FitDialogWidth', 'GetInputLeft',
        'GetAdminIndicatorWidth', 'GetDlgWidth', 'GetActionButtonsLayout']
        .map(name => source.match(new RegExp(`(?:constexpr int|int|ActionButtonsLayout) ${name}\\([^;]*?\\) \\{[\\s\\S]*?\\n\\}`))?.[0]);
    assert.ok(layout && preset && actions && rects && functions.every(Boolean));
    compileChecks(`
        #include <windows.h>
        #include <algorithm>
        #include <array>
        #include <cassert>
        #include <cmath>
        ${layout}
        ${preset}
        ${actions}
        ${rects}
        struct RunDialogState {
            UINT dpi = 96;
            struct {
                DialogLayout dialogLayout = DialogLayout::Pill;
                DialogWidthPreset dialogWidthPreset = DialogWidthPreset::Narrow;
                int customDialogWidth = 0;
                bool inlineAdminIndicator = true;
            } settings;
            ActionWidths actions{};
        };
        constexpr int kBaseDlgWidth = 600;
        constexpr int kBasePanelInset = 11;
        int DpiScale(int size, UINT, int = 100) { return size; }
        int DpiScaleS(int size, const RunDialogState*) { return size; }
        int GetInputBarHeight(const RunDialogState*) { return 54; }
        int GetActionButtonHeight(const RunDialogState*, int) { return 32; }
        int GetInputBarTopY(const RunDialogState*) { return 0; }
        int GetVerticalPanelInset(int bar, int button, bool) { return (bar - button) / 2; }
        float GetFontSizeSF(float size, const RunDialogState*) { return size; }
        ActionWidths GetVisibleActionButtonWidths(const RunDialogState* state, int) { return state->actions; }
        ${functions.join('\n')}
        int main() {
            RunDialogState state;
            for (unsigned visible = 0; visible < 16; ++visible) {
                for (size_t i = 0; i < kRunActionCount; ++i) state.actions[i] = visible & (1u << i) ? 32 : 0;
                const auto result = GetActionButtonsLayout(&state);
                const RECT buttons[]{result.rcBrowse, result.rcRun, result.rcClose, result.rcGear};
                assert(result.rcEdit.right - result.rcEdit.left >= 100);
                assert(result.rcAdminIndicator.left - result.rcEdit.right == 8);
                bool matched = false;
                for (const auto& button : buttons) {
                    if (button.right == button.left) continue;
                    assert(button.left - result.rcAdminIndicator.right == 6);
                    matched = true;
                    break;
                }
                if (!matched) assert(GetDlgWidth(&state) - result.rcAdminIndicator.right == 11);
            }
        }
    `, true);
});

test('the administrator gesture inverts the effective default without changing it', () => {
    const action = source.match(/enum class RunEnterAction[^\n]*;/)?.[0];
    const resolve = source.match(/constexpr RunEnterAction ResolveRunEnterAction\([\s\S]*?\n\}/)?.[0];
    const privileges = source.match(/constexpr bool ResolveRunElevation\([\s\S]*?\n\}/)?.[0];
    assert.ok(action && resolve && privileges);
    compileChecks(`
        ${action}
        ${resolve}
        ${privileges}
        using enum RunEnterAction;
        static_assert(ResolveRunEnterAction(false, false, false) == Normal);
        static_assert(ResolveRunEnterAction(true, false, false) == Normal);
        static_assert(ResolveRunEnterAction(false, true, false) == Terminal);
        static_assert(ResolveRunEnterAction(false, true, true) == InvertElevation);
        static_assert(ResolveRunEnterAction(true, true, false) == InvertElevation);
        static_assert(ResolveRunEnterAction(true, true, true) == InvertElevation);
        static_assert(!ResolveRunElevation(false, Normal));
        static_assert(ResolveRunElevation(true, Normal));
        static_assert(ResolveRunElevation(false, InvertElevation));
        static_assert(!ResolveRunElevation(true, InvertElevation));
        static_assert(ResolveRunElevation(true, Terminal));
    `);
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

test('Auto contrast follows solid theme backgrounds in both Windows color modes', () => {
    const palette = source.match(/struct ResolvedThemeVisuals \{[\s\S]*?\n\}/)?.[0];
    const visuals = source.match(/ResolvedThemeVisuals ResolveEffectiveThemeVisuals\([\s\S]*?\n\}/)?.[0];
    const contrast = source.match(/bool ResolveDialogLightContrast\([^;\n]*\) \{[\s\S]*?\n\}/)?.[0];
    assert.ok(palette && visuals && contrast);
    compileChecks(`
        #include <windows.h>
        #include <d2d1.h>
        #include <algorithm>
        #include <cassert>
        #include <string>
        enum class SurfaceTheme { Glass, Blur, Solid };
        enum class ColorMode { Auto, Dark, Light };
        struct ModSettings {
            std::wstring themePreset, customBgColor, customBorderColor;
            int customCornerRadius = -1;
            ColorMode colorMode = ColorMode::Auto;
        };
        struct RunDialogState {
            ModSettings settings;
            bool customBgLumValid = false, customBgIsLight = false, backdropLightSample = false;
            SurfaceTheme effectiveSurface = SurfaceTheme::Solid;
        };
        bool systemLight;
        bool IsSystemAppLightTheme() { return systemLight; }
        bool IsHighContrastActive() { return false; }
        bool HasActiveCustomBackground(const RunDialogState*) { return false; }
        bool TryParseHexColor(const std::wstring& color, COLORREF* result) {
            *result = RGB(0, 0, 0);
            return color == L"000000";
        }
        ${palette};
        ${visuals}
        ${contrast}
        int main() {
            RunDialogState dialog{};
            for (bool light : {false, true}) {
                systemLight = light;
                dialog.settings.themePreset = L"warm_paper";
                assert(ResolveDialogLightContrast(&dialog));
                dialog.settings.themePreset = L"rose_pine";
                assert(!ResolveDialogLightContrast(&dialog));
                dialog.settings.themePreset = L"fluent_solid";
                assert(ResolveDialogLightContrast(&dialog) == light);
            }
            dialog.settings.themePreset = L"warm_paper";
            dialog.settings.customBgColor = L"000000";
            assert(!ResolveDialogLightContrast(&dialog));
            dialog.settings.colorMode = ColorMode::Light;
            assert(ResolveDialogLightContrast(&dialog));
        }
    `, true, ['-luser32']);
});

test('DirectWrite centers history text and shares a baseline for menu labels and shortcuts', () => {
    const center = source.match(/float GetCenteredInkTop\([\s\S]*?\n\}/)?.[0];
    const draw = source.match(/void DrawCenteredTextLayout\([\s\S]*?\n\}/)?.[0];
    const single = source.match(/void DrawCenteredText\([\s\S]*?\n\}/)?.[0];
    const menu = source.match(/void DrawMenuItemText\([\s\S]*?\n\}/)?.[0];
    assert.ok(center && draw && single && menu, 'Use the production text renderers');
    compileChecks(String.raw`
        #include <windows.h>
        #include <d2d1.h>
        #include <dwrite.h>
        #include <cassert>
        #include <cmath>
        #include <cstdint>
        #include <cwchar>
        #include <optional>
        #include <string>
        #include <winrt/base.h>

        IDWriteFactory* g_pDWriteFactory = nullptr;
        ${center}
        ${draw}
        ${single}
        ${menu}

        int main() {
            constexpr int width = 420, height = 80;
            IDWriteFactory* textFactory = nullptr;
            assert(SUCCEEDED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                reinterpret_cast<IUnknown**>(&textFactory))));
            g_pDWriteFactory = textFactory;
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

            const wchar_t* labels[] = {L"\u041e\u0431\u0437\u043e\u0440",
                L"\u0412\u044b\u043f\u043e\u043b\u043d\u0438\u0442\u044c",
                L"\u0417\u0430\u043a\u0440\u044b\u0442\u044c",
                L"\u041d\u0430\u0441\u0442\u0440\u043e\u0439\u043a\u0438"};
            const wchar_t* shortcuts[] = {L"Ctrl+O", L"Enter", L"Esc", L"Ctrl+,"};
            for (float scale : scales) {
                IDWriteTextFormat* format = nullptr;
                assert(SUCCEEDED(textFactory->CreateTextFormat(L"Segoe UI Variable Text", nullptr,
                    DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
                    13.0f * scale, L"", &format)));
                format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                const auto labelBounds = D2D1::RectF(8, 9 * scale, 220, 43 * scale);
                const auto shortcutBounds = D2D1::RectF(250, 9 * scale, width - 8, 43 * scale);
                for (int i = 0; i < 4; ++i) {
                    target->BeginDraw();
                    target->Clear(D2D1::ColorF(D2D1::ColorF::Black));
                    DrawMenuItemText(target, labels[i], shortcuts[i], format,
                        labelBounds, shortcutBounds, brush, brush);
                    assert(SUCCEEDED(target->EndDraw()));
                    GdiFlush();
                    const auto image = static_cast<const uint32_t*>(pixels);
                    for (const auto& bounds : {labelBounds, shortcutBounds}) {
                        int top = height, bottom = -1;
                        for (int y = 0; y < height; ++y) {
                            for (int x = static_cast<int>(bounds.left); x < bounds.right; ++x) {
                                if ((image[y * width + x] & 255) <= 16) continue;
                                if (y < top) top = y;
                                if (y > bottom) bottom = y;
                            }
                        }
                        assert(top <= bottom &&
                            std::abs((top + bottom + 1) * 0.5f - 26.0f * scale) <= 1.5f);
                    }
                }
                format->Release();
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

test('layered icon rendering preserves transparent and translucent pixels', () => {
    const drawIcon = source.match(/void DrawLayeredRunIcon\([\s\S]*?\n\}/)?.[0];
    assert.ok(drawIcon, 'The Run dialog uses an alpha-aware icon renderer');
    compileChecks(`
        #include <windows.h>
        #include <d2d1.h>
        #include <wincodec.h>
        #include <winrt/base.h>
        #include <cassert>
        #include <cstdlib>
        #include <cstdint>

        ${drawIcon}

        int main() {
            assert(SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)));
            winrt::com_ptr<IWICImagingFactory> wic;
            assert(SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(wic.put()))));
            const auto dc = CreateCompatibleDC(nullptr);
            assert(dc);
            BITMAPINFO format{};
            format.bmiHeader = {sizeof(BITMAPINFOHEADER), 8, -8, 1, 32, BI_RGB};
            void* iconBits = nullptr;
            const auto color = CreateDIBSection(dc, &format, DIB_RGB_COLORS, &iconBits, nullptr, 0);
            const auto mask = CreateBitmap(8, 8, 1, 1, nullptr);
            assert(color && iconBits && mask);
            auto pixels = static_cast<uint32_t*>(iconBits);
            for (int y = 0; y < 8; ++y) {
                for (int x = 0; x < 8; ++x) {
                    pixels[y * 8 + x] = x < 2 ? 0 : x < 5 ? 0x8000FF00 : 0xFFFF0000;
                }
            }
            ICONINFO info{TRUE, 0, 0, mask, color};
            const auto icon = CreateIconIndirect(&info);
            assert(icon);
            DeleteObject(color);
            DeleteObject(mask);

            BYTE andBits[16]{}, xorBits[16]{};
            for (int y = 0; y < 8; ++y) {
                andBits[y * 2] = 0xF0;
                xorBits[y * 2] = 0x0F;
            }
            const auto legacy = CreateIcon(nullptr, 8, 8, 1, 1, andBits, xorBits);
            assert(legacy);

            constexpr int width = 48, height = 16;
            format.bmiHeader.biWidth = width;
            format.bmiHeader.biHeight = -height;
            void* surfaceBits = nullptr;
            const auto bitmap = CreateDIBSection(dc, &format, DIB_RGB_COLORS, &surfaceBits, nullptr, 0);
            assert(bitmap && surfaceBits);
            const auto previous = SelectObject(dc, bitmap);
            winrt::com_ptr<ID2D1Factory> factory;
            assert(SUCCEEDED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, factory.put())));
            const auto properties = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_SOFTWARE,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
            winrt::com_ptr<ID2D1DCRenderTarget> target;
            assert(SUCCEEDED(factory->CreateDCRenderTarget(&properties, target.put())));
            const RECT client{0, 0, width, height};
            assert(SUCCEEDED(target->BindDC(dc, &client)));

            target->BeginDraw();
            target->Clear(D2D1::ColorF(0.25f, 0.5f, 0.75f, 0.4f));
            DrawLayeredRunIcon(target.get(), wic.get(), icon, D2D1::RectF(8, 0, 24, 16));
            DrawLayeredRunIcon(target.get(), wic.get(), legacy, D2D1::RectF(24, 0, 40, 16));
            assert(SUCCEEDED(target->EndDraw()));
            GdiFlush();
            const auto row = static_cast<const uint32_t*>(surfaceBits) + width * 8;
            assert((row[4] >> 24) == 102);
            assert(row[9] == row[4]);
            assert(row[28] == row[4]);
            assert(row[22] == 0xFFFF0000);
            assert(row[38] == 0xFFFFFFFF);
            assert(std::abs(static_cast<int>(row[14] >> 24) - 179) <= 1);
            assert(std::abs(static_cast<int>((row[14] >> 8) & 255) - 153) <= 1);

            target = nullptr;
            wic = nullptr;
            SelectObject(dc, previous);
            DeleteObject(bitmap);
            DeleteDC(dc);
            DestroyIcon(icon);
            DestroyIcon(legacy);
            CoUninitialize();
        }
    `, true, ['-ld2d1', '-lwindowscodecs', '-lgdi32', '-luser32', '-lole32']);
});

test('the settings window is shown on the first request from a hidden tool process', () => {
    const show = source.match(/void ShowModernRunSettingsWindow\([\s\S]*?\n\}/)?.[0];
    assert.ok(show, 'Use the production settings-window presenter');
    compileChecks(`
        #include <windows.h>
        #include <cassert>
        #include <cwchar>
        ${show}
        int main(int argc, char**) {
            if (argc > 1) {
                HWND window = CreateWindowExW(WS_EX_TOOLWINDOW, L"STATIC", L"",
                    WS_POPUP, -32000, -32000, 64, 64, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
                assert(window && !IsWindowVisible(window));
                ShowModernRunSettingsWindow(window, SW_SHOWNORMAL);
                assert(IsWindowVisible(window));
                DestroyWindow(window);
                return 0;
            }
            wchar_t executable[MAX_PATH]{};
            assert(GetModuleFileNameW(nullptr, executable, MAX_PATH));
            wchar_t command[MAX_PATH + 20]{};
            swprintf_s(command, L"\\\"%ls\\\" --hidden", executable);
            STARTUPINFOW startup{sizeof(startup)};
            startup.dwFlags = STARTF_USESHOWWINDOW;
            startup.wShowWindow = SW_HIDE;
            PROCESS_INFORMATION child{};
            assert(CreateProcessW(executable, command, nullptr, nullptr, FALSE, 0,
                nullptr, nullptr, &startup, &child));
            const auto wait = WaitForSingleObject(child.hProcess, 4000);
            if (wait == WAIT_TIMEOUT) TerminateProcess(child.hProcess, 1);
            DWORD status = 1;
            assert(GetExitCodeProcess(child.hProcess, &status));
            CloseHandle(child.hThread);
            CloseHandle(child.hProcess);
            assert(wait == WAIT_OBJECT_0 && status == 0);
        }
    `, true, ['-luser32']);
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
