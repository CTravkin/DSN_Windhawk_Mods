# Comparison artwork

`Comparison.en.png` and `Comparison.ru.png` show selected application pointers before and after replacement by Cursor Scheme Extension. The images contain third-party cursor artwork, which is not covered by this repository's MIT license.

| Artwork | Author and source |
| --- | --- |
| Windows Aero, native precision selection, legacy autoscroll, and Office pointers | Microsoft, resources from the installed Windows 11 and Office LTSC 2024 |
| Bibata Original Ice, Regular | ful1e5, [Bibata Cursor](https://github.com/ful1e5/Bibata_Cursor), [GPL-3.0 license](https://github.com/ful1e5/Bibata_Cursor/blob/main/LICENSE) |
| Windows 11 Cursors Concept, Light Classic, Small | [jepriCreations](https://www.deviantart.com/jepricreations), [original cursor pack](https://www.deviantart.com/jepricreations/art/Windows-11-Cursors-Concept-HD-v2-890672103), [pack attribution and distribution terms](https://github.com/ShumTin/Windows-11-Cursors-Concept-v2/blob/main/How%20to%20install%20%26%20Important%20info.txt) |

## Rendering

The comparison is rendered offscreen, without capturing the desktop or changing the selected Windows scheme. Cursor files are loaded at 32 × 32 pixels. Transparent padding is cropped, and all pointers are enlarged by the same integer factor without altering their relative proportions. Windows AND/XOR masks are composited against the image's dark background with `DrawIconEx`.

The left half uses the same Office hand, column-resize pointer, and cell-selection cross in each theme column. Autoscroll uses native USER32 resources: vertical, horizontal, and both axes in theme order, followed by the upward and down-right pointers. These pointers remain independent of the selected Windows scheme.

The right half uses each theme's Hand, Size North–South / Size West–East / Move, Alternate Select, Size West–East, and Precision Select roles. It calls the mod's actual cursor-reading, transformation, and cursor-creation functions. Smoothing is enabled for the autoscroll origin and both direction arrows, subject to the mod's normal handling of monochrome cursors. Aero's empty Precision Select scheme entry uses the native USER32 cross, not a cursor from the active personal scheme.

The images illustrate enabled replacements; application-specific limitations remain as described in the mod README. Rendering tools and input requirements are listed in [Tools](../../Tools/TOOLS.md).
