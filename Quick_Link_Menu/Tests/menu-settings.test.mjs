import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import test from 'node:test';

const source = readFileSync(new URL('../Mod/quick-link-menu.wh.cpp', import.meta.url), 'utf8');
const lines = source.split('// ==WindhawkModSettings==')[1].split('// ==/WindhawkModSettings==')[0].split(/\r?\n/);
const locales = ['', ':ru-RU', ':es-ES', ':de-DE', ':pt-BR', ':zh-CN'];

function setting(group, key, value) {
  const groupLine = lines.indexOf(`- ${group}:`);
  assert.ok(groupLine >= 0, `Missing ${group}`);
  const end = lines.findIndex((line, index) => index > groupLine && line.startsWith('- '));
  const block = lines.slice(groupLine + 1, end >= 0 ? end : undefined);
  if (!key) return block.filter((line) => /^  \$/.test(line));
  const start = block.indexOf(`  - ${key}: ${value}`);
  assert.ok(start >= 0, `Missing ${group}.${key} with default ${value}`);
  const next = block.findIndex((line, index) => index > start && !line.startsWith('    '));
  return block.slice(start + 1, next >= 0 ? next : undefined);
}

for (const [group, key, value, title] of [
  ['general', null, null, 'Общие настройки'],
  ['general', 'submenuHover', 'immediate', 'Раскрытие подменю'],
  ['general', 'hideAdminCommands', false, 'Скрытие команд администратора'],
  ['general', 'showHandCursor', true, 'Индикация активных команд'],
  ['commandOptions', null, null, 'Настройки команд'],
  ['commandOptions', 'terminalAsAdmin', false, 'Терминал от имени администратора'],
  ['commandOptions', 'primaryPowerAction', 'shutdown', 'Завершение работы'],
]) {
  test(`${[group, key].filter(Boolean).join('.')} has a label and hint in six languages`, () => {
    const block = setting(group, key, value);
    for (const annotation of ['$name', '$description']) {
      for (const locale of locales) {
        const prefix = `${' '.repeat(key ? 4 : 2)}${annotation}${locale}: `;
        const entry = block.find((line) => line.startsWith(prefix));
        assert.ok(entry, `Missing ${annotation}${locale}`);
        const text = JSON.parse(entry.slice(prefix.length));
        assert.ok(text.trim());
        if (annotation === '$name' && locale === ':ru-RU') assert.equal(text, title);
      }
    }
  });
}

test('command settings omit the removed task dialog', () => {
  assert.ok(!lines.some((line) => /^\s*- createTask:/.test(line)));
});

for (const [group, key, value, options] of [
  ['general', 'submenuHover', 'immediate', ['immediate', 'delayed', 'arrow', 'click']],
  ['commandOptions', 'primaryPowerAction', 'shutdown', ['none', 'lock', 'signOut', 'sleep', 'hibernate', 'shutdown', 'restart']],
]) {
  test(`${group}.${key} exposes every option in every language`, () => {
    const block = setting(group, key, value);
    for (const locale of locales) {
      const start = block.indexOf(`    $options${locale}:`);
      assert.ok(start >= 0);
      const entries = block.slice(start + 1, start + 1 + options.length);
      assert.deepEqual(entries.map((line) => line.trim().split(': ')[0]), options.map((option) => `- ${option}`));
      assert.ok(entries.every((line) => JSON.parse(line.trim().split(': ')[1]).trim()));
    }
  });
}

test('Russian section descriptions use the approved wording', () => {
  for (const value of [
    'Программы для управления системой, диагностика и терминалы',
    'Разделы и страницы стандартного приложения «Параметры»',
    'Категории и окна классической панели управления',
  ]) assert.ok(lines.includes(`  $description:ru-RU: ${JSON.stringify(value)}`));
});
