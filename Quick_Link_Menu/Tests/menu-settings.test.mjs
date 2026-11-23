import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import test from 'node:test';
import { migrateSettings } from '../Tools/migrate-settings.mjs';

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

for (const [group, key, value, title, hint] of [
  ['general', null, null, 'Общие настройки', 'Глобальные параметры меню и команд'],
  ['general', 'submenuHover', 'immediate', 'Раскрытие подменю', 'Определяет, как открывается список дополнительных команд из основного меню'],
  ['general', 'hideAdminCommands', false, 'Скрытие команд администратора', 'Если выполнение команды запрещено текущему пользователю или повышение прав запрещено, она скрывается'],
  ['general', 'showHandCursor', true, 'Индикация активных команд', 'При наведении на область, нажатие в которой выполняет команду, указатель принимает форму руки'],
  ['commandOptions', null, null, 'Настройки команд', 'Дополнительные параметры конкретных команд'],
  ['commandOptions', 'terminalAsAdmin', false, 'Терминал от имени администратора', 'Открытие терминала из основного меню запускает его от имени администратора'],
  ['commandOptions', 'primaryPowerAction', 'shutdown', 'Завершение работы', 'Нажатие на список способов завершения работы выполняет выбранное действие'],
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
        if (locale === ':ru-RU') assert.equal(text, annotation === '$name' ? title : hint);
      }
    }
  });
}

test('the section title and description belong to the top-level switch', () => {
  for (const group of ['windowsSettings', 'controlPanel', 'administration', 'commands']) {
    const start = lines.indexOf(`- ${group}Enabled: true`);
    assert.ok(start >= 0, `${group} needs a top-level switch enabled by default`);
    const end = lines.indexOf(`- ${group}:`);
    assert.equal(end, lines.findIndex((line, index) => index > start && line.startsWith('- ')));
    const block = lines.slice(start + 1, end);
    for (const annotation of ['$name', '$description']) {
      for (const locale of locales) {
        const prefix = `  ${annotation}${locale}: `;
        const line = block.find((entry) => entry.startsWith(prefix));
        assert.ok(line && JSON.parse(line.slice(prefix.length)).trim(), `${group} ${prefix}`);
      }
    }
    assert.deepEqual(setting(group, null), ['  $name: " "']);
  }
});

test('the mod description is present in six languages and uses the approved Russian text', () => {
  const metadata = source.split('// ==/WindhawkMod==')[0].split(/\r?\n/);
  for (const locale of locales) {
    const prefix = `// @description${locale} `;
    const entry = metadata.find((line) => line.startsWith(prefix));
    assert.ok(entry?.slice(prefix.length).trim());
    if (locale === ':ru-RU') {
      assert.equal(entry.slice(prefix.length).trim(), 'Кастомизация и расширение возможностей меню Win+X (правый клик по кнопке «Пуск»)');
    }
  }
});

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

test('preserves flat general settings from early development builds', () => {
  const old = { submenuHover: 'arrow', hideAdminCommands: 1, showHandCursor: 0, 'commands.run': 0 };
  const current = {
    windowsSettingsEnabled: 1,
    controlPanelEnabled: 1,
    administrationEnabled: 1,
    commandsEnabled: 1,
    'commandOptions.primaryPowerAction': 'shutdown',
    'general.submenuHover': 'immediate',
    'general.hideAdminCommands': 0,
    'general.showHandCursor': 1,
    'commands.run': 1,
    submenuHover: 'arrow',
  };
  assert.deepEqual(migrateSettings(old, current), {
    windowsSettingsEnabled: 1,
    controlPanelEnabled: 1,
    administrationEnabled: 1,
    commandsEnabled: 1,
    'commandOptions.primaryPowerAction': 'shutdown',
    'general.submenuHover': 'arrow',
    'general.hideAdminCommands': 1,
    'general.showHandCursor': 0,
    'commands.run': 0,
  });
  assert.equal(old.submenuHover, 'arrow');
});

test('migrates section switches and preserves explicit values on current paths', () => {
  const old = {
    'windowsSettings.sectionEnabled': 0,
    'controlPanel.sectionEnabled': 0,
    'administration.sectionEnabled': 1,
    'commands.sectionEnabled': 0,
    commandsEnabled: 1,
    showHandCursor: 0,
    'general.showHandCursor': 1,
  };
  const expected = {
    windowsSettingsEnabled: 0,
    controlPanelEnabled: 0,
    administrationEnabled: 1,
    commandsEnabled: 1,
    'general.showHandCursor': 1,
  };
  const current = { windowsSettingsEnabled: 1, controlPanelEnabled: 1, administrationEnabled: 1, commandsEnabled: 1 };
  assert.deepEqual(migrateSettings(old, current), expected);
  assert.deepEqual(migrateSettings(expected, expected), expected);
});
