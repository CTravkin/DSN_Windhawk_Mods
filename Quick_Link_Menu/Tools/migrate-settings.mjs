import { readFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const movedKeys = [
  ['submenuHover', 'general.submenuHover'],
  ['hideAdminCommands', 'general.hideAdminCommands'],
  ['showHandCursor', 'general.showHandCursor'],
  ['windowsSettings.sectionEnabled', 'windowsSettingsEnabled'],
  ['controlPanel.sectionEnabled', 'controlPanelEnabled'],
  ['administration.sectionEnabled', 'administrationEnabled'],
  ['commands.sectionEnabled', 'commandsEnabled'],
];

function validateSettings(settings, label) {
  if (!settings || typeof settings !== 'object' || Array.isArray(settings)
      || Object.values(settings).some(value => typeof value !== 'string' && !Number.isInteger(value))) {
    throw new Error(`Expected ${label} Windhawk settings as a JSON object of strings and integers`);
  }
}

export function migrateSettings(previous, current) {
  validateSettings(previous, 'pre-update');
  validateSettings(current, 'post-update');

  const settings = { ...current, ...previous };
  for (const [oldKey, currentKey] of movedKeys) {
    if (Object.hasOwn(previous, oldKey) && !Object.hasOwn(previous, currentKey)) {
      settings[currentKey] = previous[oldKey];
    }
    delete settings[oldKey];
  }
  return settings;
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) {
  try {
    if (process.argv.length !== 4)
      throw new Error('Usage: node migrate-settings.mjs <prior-settings.json> <current-settings.json>');
    const readSettings = path => JSON.parse(readFileSync(path, 'utf8').replace(/^\uFEFF/, ''));
    const previous = readSettings(process.argv[2]);
    const current = readSettings(process.argv[3]);
    console.log(JSON.stringify(migrateSettings(previous, current), null, 2));
  } catch (error) {
    console.error(error.message);
    process.exitCode = 1;
  }
}
