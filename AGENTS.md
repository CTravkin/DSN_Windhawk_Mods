---
project: "Windhawk"
type: "Guide"
summary: "Development, native Windows verification, and delivery of DSN Windhawk mods."
updated: 2026-11-15
incoming: false
---
# Windhawk

## Project

### Purpose

Develop and maintain Windhawk mods, verify them in the native Windows shell,
and prepare updates for the appropriate upstream repository or catalog.

### Structure

The project root is the Git root of `CTravkin/DSN_Windhawk_Mods`. The mod
directories are [Explorer Item Pins](Explorer_Item_Pins/README.md),
[Windows Border Switch](Windows_Border_Switch/README.md), and
[Refined Quick Settings and Sound](Refined_Quick_Settings_and_Sound/README.md).
Each mod owns its source under `Mod/`, checks under `Tests/`, and any shipped
tools, assets, and documentation. Root READMEs describe the repository and index
the mods. Mod-specific features and limitations belong in each mod's README and
catalog description.

## Files

### Storage

Local materials use `Work/` for epics, `Dev/` for implementation and handoffs,
`QA/` for verification, `Review/` for engineering reviews, `Ops/` for operations,
and `Backups/` for recovery. Use their registers and the `Windhawk.base` view in
the shared Projects Obsidian vault. Use `Local/` for private development state
and `Builds/<mod directory>/` for ignored build output. Name artifacts with the
display name, version, build number, and architecture where needed.

### Git

Use the existing `origin` for DSN sources. Preserve independent mod versioning.
Keep local materials, install backups, and generated binaries outside commits.
Before changing branches or publishing, verify the exact target and its head.
Public source changes, catalog submissions, and changes proposed to a third-party
upstream have distinct histories and release targets.

## Work

### Versioning

- Record `@version X.Y.Z` in the Windhawk metadata block and `// Build: N`
  immediately after it. Increase `N` once per source revision.
- Increment `Y` for a capability and `Z` for a fix. The owner assigns `X`.
  Documentation, moves, and rebuilding unchanged source keep the version and build.
- Start version-changing commit messages with `X.Y.Z (N)`.
- For Refined Quick Settings and Sound, edit the maintained inputs under
  `Mod/Source/`, regenerate the standalone `.wh.cpp` with the mod's packaging
  tool, and verify the export and GPL-3.0-only subtree notices.

### Local environment

- On Constantine's Windows machine, elevation is authorized for in-scope Windhawk
  engine, registry, service, and affected system-process work. Request scoped
  sandbox escalation when access is required. Use the native elevated Windows
  context where Windows access checks require it. Keep operations scoped to the
  affected mod and related host processes.
- After successfully building a new usable mod version, install it in the system
  Windhawk on this machine in the same task by default. Follow any explicit
  instruction to skip installation or use an isolated environment. Build-only
  harnesses and disposable diagnostics are not installable mod versions.
- Identify the installed mod ID, version, architecture, and enabled state before
  updating. Preserve the active settings, applicable persistent data, and a
  recoverable copy of the previous source, DLL, and settings. Use the system
  Windhawk editor or the established native installation path for that mod.
- Verify the installed source and registered DLL match the intended build, then
  check that the target process loads it and exercise the changed behavior where
  possible. Record any unobserved architecture or shell surface. If the update
  fails, recover the prior working installation and report the outcome.

### Publication

- Submit original DSN mods and their updates through a catalog PR containing
  exactly one standalone `mods/<mod-id>.wh.cpp` file. See the
  [catalog submission rules](https://github.com/ramensoftware/windhawk-mods#submitting-a-new-mod).
- Preserve the upstream title, ID, GitHub profile, separate third-party notices,
  and GPL-3.0-only terms for Refined Quick Settings and Sound. Propose changes in
  HeyOkay's upstream source repository. The upstream author owns catalog updates.
- Never name the AI model, add a generated-by signature, or include model metadata
  in external comments, pull requests, issues, review replies, or commit text.
  When an upstream requires model disclosure for AI-generated text, give
  Constantine the technical findings for his own reply and leave submission to him.
  Keep a project's required notices and existing authorship intact.
- Use neutral wording in public documentation and UI. If Russian copy requires
  direct address, use formal forms. Use `е` in external Russian text and omit the
  final period in short standalone labels and hints.
