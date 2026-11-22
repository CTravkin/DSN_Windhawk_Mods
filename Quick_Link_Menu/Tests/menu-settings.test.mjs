import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import test from 'node:test';

const source = readFileSync(new URL('../Mod/quick-link-menu.wh.cpp', import.meta.url), 'utf8');
const settings = source.split('// ==WindhawkModSettings==')[1]?.split('// ==/WindhawkModSettings==')[0];
const locales = ['', ':ru-RU', ':es-ES', ':de-DE', ':pt-BR', ':zh-CN'];

for (const [key, defaultValue] of [['hideAdminCommands', false], ['showHandCursor', true]]) {
  test(`${key} is a top-level setting with the intended default and six labels`, () => {
    const start = `- ${key}: ${defaultValue}`;
    const lines = settings.split(/\r?\n/);
    const index = lines.indexOf(start);
    assert.ok(index >= 0, `Missing ${start}`);
    for (const [offset, suffix] of locales.entries()) {
      assert.ok(lines[index + offset + 1].startsWith(`  $name${suffix}: `));
      assert.ok(JSON.parse(lines[index + offset + 1].split(': ').at(-1)).trim());
    }
  });
}
