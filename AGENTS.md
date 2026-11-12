# DSN Windhawk Mods

Keep root READMEs to a short description and an index of mods, following DSN Tools. Keep versioning and publication conventions here.

Use impersonal wording in user-facing documentation and interface text. Use formal address in Russian where direct address is necessary.

Use `Mod/` for a mod's source and `Tests/` for its checks. Keep local builds under the ignored repository `Builds/<mod directory>/`, using the display name, version and build number in artifact names.

## Versioning

- Each mod has an independent version and build number. The mod's `.wh.cpp` records the version in Windhawk's numeric `@version X.Y.Z` field and the positive integer build in a `// Build: N` comment immediately after the metadata block.
- Increase `N` once per source revision of that mod. For a new capability increment `Y`; for a fix increment `Z`. The owner assigns `X`. Keep `N` increasing across version changes.
- Documentation-only edits, repository reorganization without mod changes, and unchanged rebuilds keep the same version and build.
- Use `X.Y.Z (N)` at the start of a commit message that changes the mod version.

## Publication

Maintain the sources in this repository. Submit each mod and its updates to the Windhawk catalog through a pull request containing its standalone `mods/<mod-id>.wh.cpp` file.
