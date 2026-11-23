import { readFileSync, writeFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';

const localFile = (name) => fileURLToPath(new URL(name, import.meta.url));
const blocks = JSON.parse(readFileSync(localFile('../Mod/Source/Catalog.json'), 'utf8'));
const source = readFileSync(localFile('../Mod/Source/QuickLinkMenu.cpp'), 'utf8').replace(/\r\n/g, '\n');
const availability = readFileSync(localFile('../Mod/Source/Availability.h'), 'utf8').replace(/\r\n/g, '\n').trimEnd();
const target = localFile('../Mod/quick-link-menu.wh.cpp');
const locales = ['', ':ru-RU', ':es-ES', ':de-DE', ':pt-BR', ':zh-CN'];
const wide = (value) => `L${JSON.stringify(value)}`;
const settings = [];
const definitions = [];
const keys = new Set();
const sectionKeys = blocks.map(({ id }) => `${id}Enabled`);

function labels(target, indent, key, names) {
  if (names.length !== locales.length || names.some((name) => !name)) {
    throw new Error(`Missing translation for ${key}: ${names.join(', ')}`);
  }
  locales.forEach((suffix, i) => {
    target.push(`${indent}${key}${suffix}: ${JSON.stringify(names[i])}`);
  });
}

function checkGlyphs(items, path, allowRepeats = false) {
  const used = new Map();
  for (const { glyph, id, sameGlyphAs } of items) {
    if (!/^(0|[a-f0-9]{4})$/.test(glyph)) throw new Error(`Invalid glyph at ${path}.${id}`);
    const original = used.get(glyph);
    if (sameGlyphAs !== undefined && (allowRepeats || glyph === '0' || original !== sameGlyphAs)) {
      throw new Error(`Invalid shared glyph at ${path}.${id}`);
    }
    if (allowRepeats || glyph === '0') continue;
    if (original && sameGlyphAs === undefined) throw new Error(`Repeated glyph ${glyph} in ${path}`);
    if (!original) used.set(glyph, id);
  }
}

function addSetting(entry, indent, id = entry.id) {
  settings.push(`${indent}- ${id}: ${entry.enabled !== false}`);
  labels(settings, `${indent}  `, '$name', entry.names);
}

function addDefinition(entry, group, key, parent) {
  if (keys.has(key)) throw new Error(`Repeated setting path ${key}`);
  keys.add(key);
  const index = definitions.length;
  definitions.push(
    `    {${wide(key)}, ${group}, ${parent}, 0x${entry.glyph},`
      + ` ${wide(entry.action)}, ${wide(entry.resource)},\n`
      + `     {${entry.names.map(wide).join(', ')}}},`,
  );
  return index;
}

function addOption(id, initial, names, descriptions, options) {
  settings.push(`  - ${id}: ${initial}`);
  labels(settings, '    ', '$name', names);
  labels(settings, '    ', '$description', descriptions);
  if (options) {
    locales.forEach((suffix, i) => {
      settings.push(`    $options${suffix}:`);
      for (const [key, translations] of options) settings.push(`      - ${key}: ${JSON.stringify(translations[i])}`);
    });
  }
}

settings.push('- general:');
addOption('submenuHover', 'immediate',
  ['Open submenus', 'Раскрытие подменю', 'Apertura de submenús', 'Untermenüs öffnen', 'Abrir submenus', '打开子菜单'],
  ['Controls how to open additional commands from the main menu', 'Определяет, как открывается список дополнительных команд из основного меню', 'Determina cómo se abre la lista de comandos adicionales desde el menú principal', 'Legt fest, wie sich die Liste zusätzlicher Befehle aus dem Hauptmenü öffnen lässt', 'Define como a lista de comandos adicionais é aberta a partir do menu principal', '设置如何从主菜单打开附加命令列表'], [
    ['immediate', ['Immediately', 'Сразу', 'Inmediatamente', 'Sofort', 'Imediatamente', '立即打开']],
    ['delayed', ['With a delay', 'С задержкой', 'Con retardo', 'Mit Verzögerung', 'Com atraso', '延迟打开']],
    ['arrow', ['On arrow hover', 'При наведении на стрелку', 'Al pasar el puntero sobre la flecha', 'Beim Darüberfahren über den Pfeil', 'Ao passar o ponteiro sobre a seta', '悬停在箭头上时打开']],
    ['click', ['On arrow click', 'При нажатии на стрелку', 'Al hacer clic en la flecha', 'Beim Klicken auf den Pfeil', 'Ao clicar na seta', '单击箭头时打开']],
  ]);
addOption('hideAdminCommands', false,
  ['Hide administrator commands', 'Скрытие команд администратора', 'Ocultar comandos de administrador', 'Administratorbefehle ausblenden', 'Ocultar comandos de administrador', '隐藏管理员命令'],
  ['A command is hidden if the current user lacks permission to run it or if elevation is blocked', 'Если выполнение команды запрещено текущему пользователю или повышение прав запрещено, она скрывается', 'Se oculta el comando si el usuario actual no tiene permiso para ejecutarlo o si se bloquea la elevación de privilegios', 'Ein Befehl wird ausgeblendet, wenn dem aktuellen Benutzer die Berechtigung zur Ausführung fehlt oder die Rechteerhöhung gesperrt ist', 'O comando é ocultado se o usuário atual não tiver permissão para executá-lo ou se a elevação de privilégios estiver bloqueada', '如果当前用户无权执行某条命令，或系统禁止提升权限，该命令将被隐藏']);
addOption('showHandCursor', true,
  ['Command indication', 'Индикация активных команд', 'Indicación de comandos activos', 'Aktive Befehle kennzeichnen', 'Indicação de comandos ativos', '指示可执行命令'],
  ['The pointer becomes a hand over an area where clicking runs a command', 'При наведении на область, нажатие в которой выполняет команду, указатель принимает форму руки', 'El puntero toma la forma de una mano al pasar por un área donde un clic ejecuta un comando', 'Der Mauszeiger wird zur Hand, sobald er sich über einem Bereich befindet, in dem ein Klick einen Befehl ausführt', 'O ponteiro assume a forma de uma mão ao passar sobre uma área em que um clique executa um comando', '将指针移到单击即可执行命令的区域时，指针会变成手形']);
labels(settings, '  ', '$name', ['General settings', 'Общие настройки', 'Configuración general', 'Allgemeine Einstellungen', 'Configurações gerais', '常规设置']);
labels(settings, '  ', '$description', ['Global settings for the menu and its commands', 'Глобальные параметры меню и команд', 'Opciones globales del menú y sus comandos', 'Globale Einstellungen für das Menü und seine Befehle', 'Configurações globais do menu e de seus comandos', '菜单及其命令的全局设置']);

settings.push('- commandOptions:');
addOption('terminalAsAdmin', false,
  ['Terminal as administrator', 'Терминал от имени администратора', 'Terminal como administrador', 'Terminal als Administrator', 'Terminal como administrador', '以管理员身份运行终端'],
  ['Opening Terminal from the main menu runs it as administrator', 'Открытие терминала из основного меню запускает его от имени администратора', 'Al abrir Terminal desde el menú principal, se ejecuta como administrador', 'Beim Öffnen über das Hauptmenü wird das Terminal als Administrator gestartet', 'Ao abrir o Terminal pelo menu principal, ele é executado como administrador', '从主菜单打开终端时，将以管理员身份运行']);
const power = blocks.find((group) => group.id === 'commands').rows.find((row) => row.action === 'power:menu');
addOption('primaryPowerAction', 'shutdown',
  ['Power action', 'Завершение работы', 'Acción de apagado', 'Aktion beim Herunterfahren', 'Ação de energia', '电源操作'],
  ['Clicking the power menu item runs the selected action', 'Нажатие на список способов завершения работы выполняет выбранное действие', 'Al hacer clic en la opción de apagado del menú, se ejecuta la acción seleccionada', 'Ein Klick auf den Eintrag für Energieoptionen führt die ausgewählte Aktion aus', 'Um clique no item de energia do menu executa a ação selecionada', '单击电源菜单项即可执行所选操作'],
  [['none', ['None', 'Нет', 'Ninguna', 'Keine', 'Nenhuma', '无']], ...power.children.map(({ id, names }) => [id, names])]);
labels(settings, '  ', '$name', ['Command settings', 'Настройки команд', 'Configuración de comandos', 'Befehlseinstellungen', 'Configurações de comandos', '命令设置']);
labels(settings, '  ', '$description', ['Additional settings for individual commands', 'Дополнительные параметры конкретных команд', 'Opciones adicionales para comandos específicos', 'Zusätzliche Einstellungen für einzelne Befehle', 'Configurações adicionais para comandos específicos', '特定命令的附加设置']);

checkGlyphs(blocks.flatMap((block) => block.rows), 'main menu', true);
blocks.forEach((block, group) => {
  settings.push(`- ${sectionKeys[group]}: true`);
  labels(settings, '  ', '$name', block.names);
  labels(settings, '  ', '$description', block.descriptions);
  settings.push(`- ${block.id}:`);
  for (const row of block.rows) {
    const hasChildren = row.children.length > 0;
    const rowKey = `${block.id}.${row.id}`;
    const switchId = hasChildren ? `${row.id}Visible` : row.id;
    addSetting(row, '  ', switchId);
    const parent = addDefinition(row, group, `${block.id}.${switchId}`, -1);

    if (!hasChildren) continue;
    settings.push(`  - ${row.id}:`);
    checkGlyphs(row.children, rowKey);
    for (const child of row.children) {
      addSetting(child, '    ');
      addDefinition(child, group, `${rowKey}.${child.id}`, parent);
    }
    // Windhawk renders the internal key when the group name is empty or missing.
    settings.push('    $name: " "');
  }
  // The switch above already carries the section title and description.
  settings.push('  $name: " "');
});

for (const token of ['// GENERATED_SETTINGS', '// GENERATED_CATALOG', '// GENERATED_AVAILABILITY']) {
  if (source.split(token).length !== 2) throw new Error(`Missing or repeated ${token}`);
}
const packed = source
  .replace('// GENERATED_SETTINGS', () => `// ==WindhawkModSettings==\n/*\n${settings.join('\n')}\n*/\n// ==/WindhawkModSettings==`)
  .replace('// GENERATED_CATALOG', () => {
    const sectionDefinitions = sectionKeys.map((key) => `    ${wide(key)},`);
    return `constexpr PCWSTR sectionEnabledKeys[] = {\n${sectionDefinitions.join('\n')}\n};\n\nconstexpr CatalogItem catalog[] = {\n${definitions.join('\n')}\n};`;
  })
  .replace('// GENERATED_AVAILABILITY', () => availability);

if (process.argv.includes('--check')) {
  if (readFileSync(target, 'utf8').replace(/\r\n/g, '\n') !== packed) {
    throw new Error('Quick Link Menu standalone source needs regeneration');
  }
} else {
  writeFileSync(target, packed);
}
console.log(`Quick Link Menu: ${blocks.length} command groups, ${definitions.length} item switches, ${locales.length} languages`);
