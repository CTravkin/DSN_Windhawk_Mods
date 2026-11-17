# Cursor Scheme Extension tools

| Tool | Purpose |
| --- | --- |
| [New-Comparison.ps1](New-Comparison.ps1) | Builds the native renderer and creates English and Russian comparison images in `Assets/Previews` |
| [Extract-Comparison.cpp](Extract-Comparison.cpp) | Reads installed Windows, Office, and theme resources and renders their original or transformed pixels using the mod's implementation |

Run from the repository root in PowerShell on Windows x64:

```powershell
.\Cursor_Scheme_Extension\Tools\New-Comparison.ps1
```

Requires the Windhawk compiler, desktop Office, and the illustrated cursor packs already installed. The default inputs use `C:\Windows\Cursors\Bibata Original Ice Regular`, `C:\Windows\Cursors\W11 Concept Light Classic`, and the standard 64-bit Office16 Excel location. Override `-WindowsCursors`, `-BibataDirectory`, `-W11Directory`, `-OfficeExecutable`, or `-CompilerRoot` for other locations; theme directories must contain the native role filenames used by this repository (`link.cur`, `up.cur`, `ns.cur`, `ew.cur`, `move.cur`, `cross.cur`).

`-OutputDirectory` selects another destination. Intermediate sprites, compiler output, and rendering logs stay under ignored `Builds/Cursor_Scheme_Extension/Comparison`. The tool neither captures the screen nor launches Office, changes the active cursor scheme, or installs the mod. If a required resource cannot be identified, generation fails rather than substituting another shape.
