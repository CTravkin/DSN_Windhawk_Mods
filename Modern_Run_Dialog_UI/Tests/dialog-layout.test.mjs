import { execFileSync } from 'node:child_process';
import { mkdirSync, mkdtempSync, readFileSync, rmdirSync, rmSync, writeFileSync } from 'node:fs';
import { join } from 'node:path';
import assert from 'node:assert/strict';
import test from 'node:test';

const source = readFileSync(new URL('../Mod/modern-run-dialog-ui.wh.cpp', import.meta.url), 'utf8');
const compiler = process.env.WINDHAWK_CLANG ||
    'C:/Program Files/Windhawk/Compiler/bin/clang++.exe';

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
    const buildRoot = join(process.cwd(), 'Builds/Modern_Run_Dialog_UI');
    mkdirSync(buildRoot, { recursive: true });
    const build = mkdtempSync(join(buildRoot, 'layout-test-'));
    try {
        const translationUnit = join(build, 'layout.cpp');
        writeFileSync(translationUnit, `
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
        execFileSync(compiler, ['-std=c++23', '-fsyntax-only', translationUnit], { stdio: 'pipe' });
    } finally {
        rmSync(join(build, 'layout.cpp'), { force: true });
        rmdirSync(build);
    }
});
