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
  ['Choose how to open the additional commands for a menu item', 'Определяет, как открывается список дополнительных команд пункта меню', 'Elige cómo abrir los comandos adicionales de un elemento del menú', 'Legt fest, wie die zusätzlichen Befehle eines Menüeintrags geöffnet werden', 'Define como abrir os comandos adicionais de um item do menu', '选择如何打开菜单项的其他命令'], [
    ['immediate', ['Immediately', 'Сразу', 'Inmediatamente', 'Sofort', 'Imediatamente', '立即打开']],
    ['delayed', ['With a delay', 'С задержкой', 'Con retardo', 'Mit Verzögerung', 'Com atraso', '延迟打开']],
    ['arrow', ['On arrow hover', 'При наведении на стрелку', 'Al pasar el puntero sobre la flecha', 'Beim Darüberfahren über den Pfeil', 'Ao passar o ponteiro sobre a seta', '悬停在箭头上时打开']],
    ['click', ['On arrow click', 'При нажатии на стрелку', 'Al hacer clic en la flecha', 'Beim Klicken auf den Pfeil', 'Ao clicar na seta', '单击箭头时打开']],
  ]);
addOption('hideAdminCommands', false,
  ['Hide administrator commands', 'Скрытие команд администратора', 'Ocultar comandos de administrador', 'Administratorbefehle ausblenden', 'Ocultar comandos de administrador', '隐藏管理员命令'],
  ['Hide commands the current user cannot run or for which elevation is blocked', 'Скрывать команды, выполнение которых недоступно текущему пользователю, или повышение прав запрещено', 'Oculta los comandos que el usuario actual no puede ejecutar o cuya elevación está bloqueada', 'Blendet Befehle aus, die der aktuelle Benutzer nicht ausführen kann oder deren Rechteerhöhung gesperrt ist', 'Oculta comandos que o usuário atual não pode executar ou cuja elevação está bloqueada', '隐藏当前用户无法运行或禁止提升权限的命令']);
addOption('showHandCursor', true,
  ['Command indication', 'Индикация активных команд', 'Indicación de comandos activos', 'Aktive Befehle kennzeichnen', 'Indicação de comandos ativos', '指示可执行命令'],
  ['Show a hand pointer over areas that run a command when clicked', 'При наведении на область, где нажатие выполняет команду, указатель принимает форму руки', 'Muestra un puntero de mano sobre las áreas que ejecutan un comando al hacer clic', 'Zeigt einen Handzeiger über Bereichen an, die beim Anklicken einen Befehl ausführen', 'Mostra um ponteiro em forma de mão sobre as áreas que executam um comando ao clicar', '将鼠标悬停在单击即可执行命令的区域时显示手形指针']);
labels(settings, '  ', '$name', ['General settings', 'Общие настройки', 'Configuración general', 'Allgemeine Einstellungen', 'Configurações gerais', '常规设置']);
labels(settings, '  ', '$description', ['These settings apply to the entire command list', 'Эти настройки действуют на весь список команд', 'Estos ajustes se aplican a toda la lista de comandos', 'Diese Einstellungen gelten für die gesamte Befehlsliste', 'Estas configurações se aplicam à lista inteira de comandos', '这些设置适用于整个命令列表']);

settings.push('- commandOptions:');
addOption('terminalAsAdmin', false,
  ['Terminal as administrator', 'Терминал от имени администратора', 'Terminal como administrador', 'Terminal als Administrator', 'Terminal como administrador', '以管理员身份运行终端'],
  ['Run Terminal as administrator from the main row and move the standard Terminal to the submenu when the separate administrator command is enabled', 'Запускать Терминал от имени администратора из главного меню. Если отдельная команда администратора включена, обычный Терминал появится в подменю', 'Ejecuta Terminal como administrador desde el menú principal. Si el comando de administrador está activado, Terminal normal aparece en el submenú', 'Startet Terminal über das Hauptmenü als Administrator. Ist der separate Administratorbefehl aktiviert, erscheint das normale Terminal im Untermenü', 'Executa o Terminal como administrador no menu principal. Se o comando de administrador estiver ativado, o Terminal padrão aparece no submenu', '从主菜单以管理员身份运行终端。如果启用了单独的管理员命令，则在子菜单中显示普通终端']);
const power = blocks.find((group) => group.id === 'commands').rows.find((row) => row.action === 'power:menu');
addOption('primaryPowerAction', 'shutdown',
  ['Power action', 'Завершение работы', 'Acción de apagado', 'Aktion beim Herunterfahren', 'Ação de energia', '电源操作'],
  ['Replace the main power menu item with the selected command. Choose None to keep the general item', 'Заменяет общий пункт завершения работы выбранной командой. Вариант «Нет» оставляет общий пункт', 'Sustituye el elemento principal de apagado por el comando elegido. Selecciona Ninguna para conservar el elemento general', 'Ersetzt den Haupteintrag für Energieoptionen durch den ausgewählten Befehl. Mit „Keine“ bleibt der allgemeine Eintrag erhalten', 'Substitui o item principal de energia pelo comando selecionado. Escolha Nenhuma para manter o item geral', '用所选命令替换主电源菜单项。选择“无”可保留原有菜单项'],
  [['none', ['None', 'Нет', 'Ninguna', 'Keine', 'Nenhuma', '无']], ...power.children.map(({ id, names }) => [id, names])]);
addOption('createTask', false,
  ['Create new task', 'Создание задачи', 'Crear nueva tarea', 'Neuen Task erstellen', 'Criar nova tarefa', '创建新任务'],
  ['Open the Create new task dialog from Run to start a program with administrator privileges when needed', 'Открывает из пункта «Выполнить» окно «Создание задачи», в котором программу можно запустить от имени администратора', 'Abre el cuadro Crear nueva tarea desde Ejecutar para iniciar un programa como administrador si es necesario', 'Öffnet über „Ausführen“ den Dialog „Neuen Task erstellen“, um ein Programm bei Bedarf als Administrator zu starten', 'Abre Criar nova tarefa em Executar para iniciar um programa como administrador quando necessário', '从“运行”打开“创建新任务”窗口，以便在需要时以管理员身份启动程序']);
labels(settings, '  ', '$name', ['Command settings', 'Настройки команд', 'Configuración de comandos', 'Befehlseinstellungen', 'Configurações de comandos', '命令设置']);
labels(settings, '  ', '$description', ['Choose how individual commands work', 'Эти настройки меняют действие отдельных команд', 'Elige cómo funcionan los comandos individuales', 'Legt fest, wie einzelne Befehle ausgeführt werden', 'Define como comandos individuais funcionam', '选择各个命令的运行方式']);

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
console.log(`Quick Link Menu: ${blocks.length} command groups, ${definitions.length} item switches, ${locales.length} languages`);
