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
    const style = source.match(/- contextMenuStyle: mod[\s\S]*?(?=\n  - contextMenuBorderMode:)/)?.[0];
    assert.match(style, /\$name: Context Menu Style/);
    assert.match(style, /- mod: Mod Style/);
    assert.match(style, /- windows_fluent: Windows \(Fluent\)/);
    assert.match(style, /\$description:ru-RU: Меняет оформление меню при клике правой кнопкой мыши/);
});

test('layout settings keep position and menu style directly below their related controls', () => {
    const layout = source.match(/^- layoutGroup:[\s\S]*?(?=^- actionButtonsGroup:)/m)?.[0];
    assert.ok(layout, 'The layout settings group is present');
    const keys = Array.from(layout.matchAll(/^  - (\w+):/gm), match => match[1]);
    assert.deepEqual(keys.slice(0, 6), ['dialogPosition', 'rememberPosition', 'minimalMode', 'contextMenuStyle', 'contextMenuBorderMode', 'uiScalePercent']);
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
    const sizing = source.match(/int GetInputBarHeight\([\s\S]*?(?=\ninline bool HasActiveCustomBackground)/)?.[0];
    assert.ok(sizing, 'The production layout calculations are present');
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
        ${sizing}
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

test('the four window border modes resolve to the expected outline', () => {
    const mode = source.match(/enum class WindowBorderMode[^\n]*;/)?.[0];
    const resolve = source.match(/constexpr WindowBorderMode ResolveWindowBorderMode\([\s\S]*?\n\}/)?.[0];
    assert.ok(mode && resolve, 'The window border policy is present');
    compileChecks(`
        ${mode}
        ${resolve}
        using enum WindowBorderMode;
        static_assert(ResolveWindowBorderMode(Mod, false, true, true) == Mod);
        static_assert(ResolveWindowBorderMode(Windows, false, false, true) == Windows);
        static_assert(ResolveWindowBorderMode(Windows, false, true, false) == Windows);
        static_assert(ResolveWindowBorderMode(Windows, false, true, true) == Accent);
        static_assert(ResolveWindowBorderMode(Accent, false, false, true) == Accent);
        static_assert(ResolveWindowBorderMode(Accent, true, true, true) == Windows);
        static_assert(ResolveWindowBorderMode(Off, true, true, true) == Off);
    `);
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
        static_assert(ResolveContextMenuWindowBorderMode(Menu::Accent, Window::Off) == Window::Accent);
        static_assert(ResolveContextMenuWindowBorderMode(Menu::Off, Window::Accent) == Window::Off);
    `);
});

test('history actions keep delete in the same group', () => {
    const deleteItem = source.match(/if \(canDeleteFromHistory\) \{([\s\S]*?)ctxItems\.push_back\(std::move\(delItem\)\)/)?.[1];
    assert.ok(deleteItem);
    assert.doesNotMatch(deleteItem, /isSeparator/);
});

function compileChecks(checks, run = false) {
    const buildRoot = join(process.cwd(), 'Builds/Modern_Run_Dialog_UI');
    mkdirSync(buildRoot, { recursive: true });
    const build = mkdtempSync(join(buildRoot, 'layout-test-'));
    const translationUnit = join(build, 'layout.cpp');
    const executable = join(build, 'layout.exe');
    try {
        writeFileSync(translationUnit, checks);
        const flags = run ? ['-D_WIN32_WINNT=0x0A00', '-static', '-o', executable] : ['-fsyntax-only'];
        execFileSync(compiler, ['-std=c++23', ...flags, translationUnit], { stdio: 'pipe' });
        if (run) execFileSync(executable, { stdio: 'pipe' });
    } finally {
        rmSync(executable, { force: true });
        rmSync(join(build, 'layout.cpp'), { force: true });
        rmdirSync(build);
    }
}
