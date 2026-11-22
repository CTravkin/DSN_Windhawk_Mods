# Quick Link Menu tools

`package-mod.mjs` assembles the standalone Windhawk source from
`Mod/Source/QuickLinkMenu.cpp`, `Mod/Source/Availability.h`, and
`Mod/Source/Catalog.json`. The catalog owns menu
order, action targets, display names for Windhawk, default visibility, and glyph
assignments.

The `settings:` references point to page titles in the Settings PRI.
The `app:` references use the registered app's localized shell display name.
For icons, match the glyphs in the installed Settings view model against
Segoe Fluent Icons. Main rows can reuse a glyph when Windows uses it for
the same concept. Within each submenu, the packager checks glyph uniqueness.
For two entries that intentionally share an icon, the later entry can declare
`sameGlyphAs` with the ID of the earlier entry that uses the same glyph.

Each main row has a labeled visibility switch. Related shortcuts are grouped
immediately below that switch. A single space as the group name keeps the
internal setting key out of the visual layout in Windhawk 1.7.3.

From the repository root, run `node Quick_Link_Menu/Tools/package-mod.mjs` after
editing an input. Add `--check` to verify that the standalone source matches
the inputs.

Run `node --test Quick_Link_Menu/Tests/menu-settings.test.mjs` to verify the
general and command settings, defaults, and localization in the packaged
source. Build `Tests/availability-tests.cpp`
with Windhawk clang++ for Windows x64 and link `advapi32` to test the availability
rules and background refresh.
