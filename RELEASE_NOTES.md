# wpy+ Interpiler — Release Notes

**Version:** 1.0.0  
**Date:** 2025-09-21

## Summary

This release packages the wpy+ interpiler — a small interpiler for the Python+ language (a Python-like teaching language). The folder contains sources, build artifacts, distribution packages for Windows, and a sample program `hello.pyp`.

## Contents

- Source files:
  - `main.c`, `lexer.c`, `parser.c`, `interpiler.c`, `REPL.c` and their headers
- Build artifacts:
  - Object files: `*.o`
  - Executable: `wpy+.exe` (Windows) — may also be produced as `wpy+` on Unix
- Packaging / installers:
  - `windows installer/` — Windows AIP/MSI installer and supporting files
  - `wpy+.portable/` — Chocolatey/NuGet portable package
- Docs / misc:
  - `README.md`, `LICENSE.md`, `build.ps1`, `Makefile`
- Sample program:
  - `hello.pyp` — simple example used in CI runs

## Build Instructions

### Windows (PowerShell)

1. Open PowerShell with Developer tools installed (Visual Studio build tools or mingw).
1. Use `build.ps1` or run the Makefile with a compatible make (e.g., `mingw32-make`) or MSBuild as configured.
1. Example (PowerShell):

```powershell
cd interpilers\wpy+
./build.ps1
# or use the packaged installer in `windows installer/` to create an .msi or install
```

## Artifacts

- `wpy+.exe` — Windows executable
- `wpy+.portable/` — NuGet/Chocolatey package for portable distribution
- `windows installer/` — MSI and installer assets
- `*.o` object files left from previous builds

## Compatibility

- Primary development and CI targets: Ubuntu (GitHub Actions) and Windows (portable packages / installers).
- The Makefile currently sets `TARGET = wpy+.exe`; CI uses Makefile on Linux and may produce a `wpy+.exe` binary — the CI workflow handles both `wpy+` and `wpy+.exe` at runtime.

## Known Issues & Troubleshooting

- Parse errors for `#include` directives: ensure your source uses `#include <pypstdio>` (the parser accepts a few forms). If you see "malformed #include directive" during parsing, run the lexer on the source and check emitted tokens.
- CI failures complaining about missing binary: ensure you run `make` in `interpilers/wpy+` and that the `wpy+` or `wpy+.exe` is present in that directory.
- If a build produces `wpy+.exe` on Linux and your environment expects `wpy+`, rename or update scripts to call the produced filename.

## Publishing / Tagging

- Tag this commit with `wpy+-1.0.0` and create a GitHub release including these notes and the `wpy+.portable` or `windows installer` artifacts.

## Contact / Maintainers

- Maintainer: WNU Project
- Repo: [WICC](https://github.com/WNU-Project/WICC)

## Appendix: Quick smoke test

1. Build: `cd interpilers/wpy+ && make`
2. Run: `./wpy+ hello.pyp` (or `./wpy+.exe hello.pyp` on Windows)
3. Expected output: Runs the interpiler and executes `hello.pyp` sample. If there are parser/runtime errors, see the "Known Issues" section above for guidance.

## Using a MSI File

Install The MSI File that comes with the Release On GitHub follow the rules of the installer, after it's installed make a .pyp file, and then run the interpiler to run it.
