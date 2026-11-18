const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
process.on('uncaughtException', error => { console.error(error.message); process.exit(1); });
const root = path.resolve(__dirname, '../..');
const source = fs.readFileSync(path.join(root, 'Cursor_Scheme_Extension/Mod/cursor-scheme-extension.wh.cpp'), 'utf8');
const bundlePath = 'C:/Program Files/Windhawk/UI/resources/app/extensions/windhawk/dist/extension.js';
let bundle = fs.readFileSync(bundlePath, 'utf8');
const at = bundle.indexOf('extractInitialSettingsRaw(');
const className = /^class (\w+)/.exec(bundle.slice(bundle.lastIndexOf('class ', at)))[1];
const marker = 'Ap.viewType="windhawk.sidebar"';
assert(bundle.includes(marker), 'Installed Windhawk bundle changed');
bundle = bundle.replace(marker, `${marker};globalThis.parserClass=${className}`);
const stub = new Proxy(function () {}, {
  get(target, key) {
    if (key === Symbol.toPrimitive) return () => '';
    if (key === 'prototype') return target.prototype;
    return stub;
  },
  apply() { return stub; }
});
const hostProcess = Object.create(process);
hostProcess.dlopen = module => { module.exports = stub; };
const context = { module: { exports: {} }, exports: {}, process: hostProcess, Buffer, console, setTimeout, clearTimeout, setImmediate, clearImmediate,
  URL, URLSearchParams, TextEncoder, TextDecoder,
  __dirname: path.dirname(bundlePath),
  require(name) { return name === 'vscode' ? stub : require(name); } };
context.global = context;
try { vm.runInNewContext(bundle, context, { filename: bundlePath, timeout: 10000 }); }
catch (error) { console.error(error.message); process.exit(1); }
const parser = new context.parserClass('C:/ProgramData/Windhawk');
const locales = ['en', 'ru-ru', 'es-es', 'de-de', 'pt-br', 'zh-cn'];
const expectedTargets = ['windows', 'office', 'chromium', 'gecko', 'qt', 'excludedApplications'];
const expectedCursors = ['link', 'resize', 'move', 'autoscrollOrigin', 'autoscrollArrows', 'selectionArrows', 'cell', 'marginSelection', 'verticalText'];
const expectedSmoothing = ['autoscrollOrigin', 'autoscrollArrows', 'selectionArrows'];
for (const locale of locales) {
  const settings = parser.extractInitialSettings(source, locale);
  assert.equal(JSON.stringify(settings.map(x => x.key)), JSON.stringify(['targets', 'cursors', 'smoothing']));
  for (const [group, expected] of [[settings[0], expectedTargets], [settings[1], expectedCursors], [settings[2], expectedSmoothing]]) {
    assert.equal(JSON.stringify(group.value.map(x => x.key)), JSON.stringify(expected));
  }
  const metadata = parser.extractMetadata(source, locale);
  assert.match(metadata.version, /^\d+\.\d+\.\d+$/);
  assert(metadata.description && metadata.description.length < 250);
}
const raw = parser.extractInitialSettingsRaw(source);
const suffixes = ['', ':ru-RU', ':es-ES', ':de-DE', ':pt-BR', ':zh-CN'];
let localizedItems = 0;
function check(items) {
  for (const item of items) {
    const key = Object.keys(item).find(x => !x.startsWith('$'));
    for (const field of ['$name', ...(Object.hasOwn(item, '$description') ? ['$description'] : [])]) {
      for (const suffix of suffixes) assert.equal(typeof item[field + suffix], 'string', `${key} ${field}${suffix}`);
    }
    localizedItems++;
    if (Array.isArray(item[key]) && typeof item[key][0] === 'object') check(item[key]);
  }
}
check(raw);
const engine = parser.extractInitialSettingsForEngine(source);
assert.equal(Object.keys(engine).length, 18);
assert(Object.hasOwn(engine, 'cursors.selectionArrows'));
assert(!Object.hasOwn(engine, 'cursors.downSelection'));
for (const [key, value] of Object.entries(engine)) {
  assert(key.endsWith('[0]') ? typeof value === 'string' : value === 0 || value === 1, key);
}
console.log(JSON.stringify({ locales: locales.length, localizedItems, engineSettings: Object.keys(engine).length, result: 'PASS' }));
