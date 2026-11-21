import { readFileSync, writeFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';

const localFile = (name) => fileURLToPath(new URL(name, import.meta.url));
const blocks = JSON.parse(readFileSync(localFile('../Mod/Source/Catalog.json'), 'utf8'));
const source = readFileSync(localFile('../Mod/Source/QuickLinkMenu.cpp'), 'utf8').replace(/\r\n/g, '\n');
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
  const used = new Set();
  for (const { glyph, id } of items) {
    if (!/^(0|[a-f0-9]{4})$/.test(glyph)) throw new Error(`Invalid glyph at ${path}.${id}`);
    if (glyph === '0') continue;
    if (!allowRepeats && used.has(glyph)) throw new Error(`Repeated glyph ${glyph} in ${path}`);
    used.add(glyph);
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

for (const token of ['// GENERATED_SETTINGS', '// GENERATED_CATALOG']) {
  if (source.split(token).length !== 2) throw new Error(`Missing or repeated ${token}`);
}
const packed = source
  .replace('// GENERATED_SETTINGS', () => `// ==WindhawkModSettings==\n/*\n${settings.join('\n')}\n*/\n// ==/WindhawkModSettings==`)
  .replace('// GENERATED_CATALOG', () => `constexpr CatalogItem catalog[] = {\n${definitions.join('\n')}\n};`);

if (process.argv.includes('--check')) {
  if (readFileSync(target, 'utf8').replace(/\r\n/g, '\n') !== packed) {
    throw new Error('Quick Link Menu standalone source needs regeneration');
  }
} else {
  writeFileSync(target, packed);
}
console.log(`Quick Link Menu: ${blocks.length} groups, ${definitions.length} switches, ${locales.length} languages`);
