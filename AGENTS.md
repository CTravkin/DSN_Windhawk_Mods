# DSN Windhawk Mods

Keep root READMEs to a short description and an index of mods, following DSN Tools. Keep versioning and release conventions here.

## Versioning

- Each mod has an independent version and build number. The mod's `.wh.cpp` records the version in Windhawk's numeric `@version X.Y.Z` field and the positive integer build in a `// Build: N` comment immediately after the metadata block.
- Increase `N` once per source revision of that mod. For a new capability increment `Y`; for a fix increment `Z`. The owner assigns `X`. Keep `N` increasing across version changes.
- Documentation-only edits, repository reorganization without mod changes, and unchanged rebuilds keep the same version and build.
- Use `X.Y.Z (N)` at the start of a commit message that changes the mod version.
- Release titles and archive names use `<mod name> X.Y.Z (build N)`. Release tags use `<mod-id>-vX.Y.Z`.
