# Quick Link Menu tools

`package-mod.mjs` assembles the standalone Windhawk source from
`Mod/Source/QuickLinkMenu.cpp` and `Mod/Source/Catalog.json`. The catalog owns menu
order, action targets, display names for Windhawk, default visibility, and glyph
assignments.

From the repository root, run `node Quick_Link_Menu/Tools/package-mod.mjs` after
editing either input. Add `--check` to verify that the standalone source matches
the inputs.
