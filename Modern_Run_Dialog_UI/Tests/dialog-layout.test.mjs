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
    const schema = source.match(/- layoutGroup:\s*\n([\s\S]*?)\n  - rememberPosition:/)?.[1];
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
    const style = source.match(/- contextMenuStyle: mod[\s\S]*?(?=\n  - customBgColor:)/)?.[0];
    assert.match(style, /\$name: Context Menu Style/);
    assert.match(style, /- mod: Mod Style/);
    assert.match(style, /- windows_fluent: Windows \(Fluent\)/);
    assert.match(style, /\$description:ru-RU: Меняет оформление меню при клике правой кнопкой мыши/);
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
            assert(FormatHistoryTimestamp(utc, L"Today") == std::wstring(time) + L" " + date);
            GetSystemTimeAsFileTime(&utc);
            assert(FileTimeToSystemTime(&utc, &utcTime));
            assert(SystemTimeToTzSpecificLocalTimeEx(nullptr, &utcTime, &past));
            assert(GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &past, pattern, time, 256));
            assert(FormatHistoryTimestamp(utc, L"Today") == std::wstring(L"Today, ") + time);
        }
    `, true);
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
