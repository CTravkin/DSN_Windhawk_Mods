import { readFileSync, writeFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';

const localFile = (name) => fileURLToPath(new URL(name, import.meta.url));
const blocks = JSON.parse(readFileSync(localFile('../Mod/Source/Catalog.json'), 'utf8'));
const source = readFileSync(localFile('../Mod/Source/QuickLinkMenu.cpp'), 'utf8').replace(/\r\n/g, '\n');
const availability = readFileSync(localFile('../Mod/Source/Availability.h'), 'utf8').replace(/\r\n/g, '\n').trimEnd();
const target = localFile('../Mod/quick-link-menu.wh.cpp');
const locales = ['', ':ru-RU', ':es-ES', ':de-DE', ':pt-BR', ':zh-CN'];
const optionNames = [
  ['Open submenus on hover', 'Раскрытие подменю при наведении', 'Apertura de submenús al pasar el puntero', 'Untermenüs beim Darüberfahren öffnen', 'Abrir submenus ao passar o ponteiro', '悬停时打开子菜单'],
  ['Immediately', 'Сразу', 'Inmediatamente', 'Sofort', 'Imediatamente', '立即打开'],
  ['With the Windows delay', 'С задержкой Windows', 'Con el retardo de Windows', 'Mit Windows-Verzögerung', 'Com o atraso do Windows', '使用 Windows 延迟'],
  ['When hovering over the arrow', 'При наведении на стрелку', 'Al pasar el puntero sobre la flecha', 'Beim Darüberfahren über den Pfeil', 'Ao passar o ponteiro sobre a seta', '悬停在箭头上时打开'],
];

const wide = (value) => `L${JSON.stringify(value)}`;
const settings = ['- submenuHover: immediate'];
const definitions = [];
const keys = new Set();

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
    `    {${wide(key)}, ${group}, ${parent}, ${entry.enabled !== false}, 0x${entry.glyph},`
      + ` ${wide(entry.action)}, ${wide(entry.resource)},\n`
      + `     {${entry.names.map(wide).join(', ')}}},`,
  );
  return index;
}

labels(settings, '  ', '$name', optionNames[0]);
locales.forEach((suffix, i) => {
  settings.push(`  $options${suffix}:`);
  ['immediate', 'delayed', 'arrow'].forEach((key, j) => {
    settings.push(`  - ${key}: ${JSON.stringify(optionNames[j + 1][i])}`);
  });
});

settings.push('- hideAdminCommands: false');
labels(settings, '  ', '$name', ['Hide administrator commands', 'Скрыть команды администратора', 'Ocultar comandos de administrador', 'Administratorbefehle ausblenden', 'Ocultar comandos de administrador', '隐藏管理员命令']);
labels(settings, '  ', '$description', ['Hide commands that require credentials or cannot be elevated in the current session', 'Скрывать команды, если для повышения прав нужны учетные данные или повышение запрещено', 'Oculta comandos que requieren credenciales o no pueden elevarse en la sesión actual', 'Befehle ausblenden, die Anmeldedaten erfordern oder in dieser Sitzung nicht erhöht werden können', 'Oculta comandos que exigem credenciais ou não podem ser elevados na sessão atual', '隐藏需要凭据或无法在当前会话中提升权限的命令']);
settings.push('- showHandCursor: true');
labels(settings, '  ', '$name', ['Show hand cursor over commands', 'Показывать курсор-руку над командами', 'Mostrar el puntero de mano sobre los comandos', 'Handzeiger über Befehlen anzeigen', 'Mostrar o cursor de mão sobre os comandos', '悬停在命令上时显示手形指针']);

checkGlyphs(blocks.flatMap((block) => block.rows), 'main menu', true);
blocks.forEach((block, group) => {
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
  labels(settings, '  ', '$name', block.names);
  labels(settings, '  ', '$description', block.descriptions);
});

for (const token of ['// GENERATED_SETTINGS', '// GENERATED_CATALOG', '// GENERATED_AVAILABILITY']) {
  if (source.split(token).length !== 2) throw new Error(`Missing or repeated ${token}`);
}
const packed = source
  .replace('// GENERATED_SETTINGS', () => `// ==WindhawkModSettings==\n/*\n${settings.join('\n')}\n*/\n// ==/WindhawkModSettings==`)
  .replace('// GENERATED_CATALOG', () => `constexpr CatalogItem catalog[] = {\n${definitions.join('\n')}\n};`)
  .replace('// GENERATED_AVAILABILITY', () => availability);

if (process.argv.includes('--check')) {
  if (readFileSync(target, 'utf8').replace(/\r\n/g, '\n') !== packed) {
    throw new Error('Quick Link Menu standalone source needs regeneration');
  }
} else {
  writeFileSync(target, packed);
}
console.log(`Quick Link Menu: ${blocks.length} groups, ${definitions.length} item switches, 2 additional switches, ${locales.length} languages`);
