# stdx

*My version of the C++ Standard Namespace*

---

## Table of Contents
- [Prerequisites](#prerequisites)
  - [Minimum](#minimum)
  - [Nice to have](#nice-to-have)
- [Getting Started](#getting-started)
  - [1. Clone the repository](#1-clone-the-repository)
  - [2. Configure the project](#2-configure-the-project)
  - [3. Build the project](#3-build-the-project)
  - [4. Run the test suite](#4-run-the-test-suite)
  - [5. Install the project](#5-install-the-project)
- [Developing](#developing)
  - [Development tools](#development-tools)
  - [Formatting and static analysis](#formatting-and-static-analysis)
  - [Generating a coverage report](#generating-a-coverage-report)
  - [compile_commands.json and clangd](#compile_commandsjson-and-clangd)

---

## Prerequisites

### Minimum
- **CMake ≥ 3.24** – required to configure the project.
- **C++ compiler**:
  - Windows: MSVC cl **or** LLVM Clang/clang-cl.
  - Linux: GCC **or** LLVM Clang.
  - macOS: LLVM Clang.
- **Git** – needed to clone the repository.

> If you plan on contributing, see [Developing](#developing) for the full toolchain used for day-to-day work on *stdx*.

---

## Getting Started
These steps will get you building and testing *stdx* on any supported platform.

### 1. Clone the repository
```shell
git clone https://github.com/andrewstef10/stdx.git
cd stdx
```

### 2. Configure the project
The repository ships with a **CMakePresets.json** file. Using the `default` preset configures CMake to use an appropriate generator and compiler for your platform:
```shell
cmake --preset default
```
> The `default` preset selects the right generator and compiler based on what is installed.

The `default` preset can be replaced with any other predefined CMake preset depending on your desired configuration and C/C++ compiler.

> To view all available presets, run `cmake --list-presets`.

### 3. Build the project
```shell
cmake --build --preset default
```

> To view all available build presets, run `cmake --build --list-presets`.

### 4. Run the test suite
To test, this project creates unit tests using Google Test (GTest). To run the unit tests, run the following:
```shell
ctest --preset default
```

> To view all available test presets, run `ctest --list-presets`.

### 5. Install the project
If you would like, after building you can install the project. To install to a default location (REPO_ROOT/install/) run the following: 
```shell
cmake --install build/default --config Debug
```
> `--config Debug` is only needed for multi config generators, but does not hurt on single config generators.

To install to a custom location of your choosing, run the following:
```shell
cmake --install build/default --config Debug --prefix "/your/custom/path"
```


> Again, `debug` can be replaced with a CMakePreset of your choosing defined in [CMakePresets.json](./CMakePresets.json)

---

## Developing
When developing, it is recommended to use the `dev` CMake preset together with **VS Code**. The `dev` preset builds with the LLVM Clang compiler in Debug, and integrates clang-tidy and code coverage (llvm-cov) directly into the build/test process — matching what CI runs.

```shell
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
```

### Development tools
The quickest way to get set up is to run the setup script, which checks for (and can install) everything below:
```shell
python scripts/setup.py            # report which tools/extensions are missing
python scripts/setup.py --install  # also install the missing ones (via winget/apt/Homebrew, where supported)
```

If the script doesn't work for your platform or package manager, here's what to install manually, and why:

- **[CMake](https://cmake.org/download/) ≥ 3.24** – required to configure the project.
- **[Ninja](https://github.com/ninja-build/ninja/releases)** – `dev` (and most other presets) configure Ninja as the generator.
- **[LLVM](https://releases.llvm.org/)** – a single toolchain/compiler suite that bundles everything below; install it once and you get all of these tools:
  - `clang` / `clang++` – the compiler.
  - `clang-tidy` – static analysis, enabled by the `dev` preset (`ENABLE_CLANG_TIDY`).
  - `clang-format` – code formatting, checked in CI via [scripts/format.py](./scripts/format.py).
  - `llvm-cov` / `llvm-profdata` – used by the `coverage` build target ([CMakeLists.txt](./CMakeLists.txt)) to generate HTML/lcov/text coverage reports.
- **Python 3** – required to run [scripts/format.py](./scripts/format.py) and [scripts/clang_tidy.py](./scripts/clang_tidy.py).
- **[VS Code](https://code.visualstudio.com/)** with:
  - **[CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)** – configure/build/test presets from the editor.
  - **[clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd)** – IntelliSense, diagnostics, and formatting powered by the same Clang toolchain as CI. The workspace disables the Microsoft C/C++ extension's IntelliSense engine (`.vscode/settings.json`) in favor of clangd, so this extension is required to get code completion and inline diagnostics.
  - **[C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)** – its IntelliSense engine is disabled in favor of clangd (above), but it still provides the debugger used to debug *stdx* from VS Code.
  - **[Code Spell Checker](https://marketplace.visualstudio.com/items?itemName=streetsidesoftware.code-spell-checker)** – spell-checks comments/identifiers against the project word list (`cSpell.words` in `.vscode/settings.json`).
  - The full list of recommended extensions lives in [.vscode/extensions.json](./.vscode/extensions.json); VS Code will prompt to install them when you open the repo.

### Formatting and static analysis
Both of these already happen automatically if you're using the `dev` preset and VS Code: clang-tidy is integrated into the CMake build and runs as part of every build, and format-on-save is enabled in [.vscode/settings.json](./.vscode/settings.json) via the clangd extension. You shouldn't need to run either manually — but if you want to run them yourself (e.g. to format/check the whole repo at once):
```shell
python scripts/format.py               # format all sources in-place
python scripts/clang_tidy.py -p build/dev   # run clang-tidy over the sources
```

### Generating a coverage report
The `dev-coverage` build preset builds the `coverage` target, which runs the test suite and generates an HTML/text/lcov coverage report with llvm-cov:
```shell
cmake --preset dev
cmake --build --preset dev-coverage
```
> The report is written to `coverage/`. Open `coverage/index.html` in a browser for the annotated, browsable report.

### compile_commands.json and clangd
Every CMake configure creates (or refreshes) a symlink at `compile_commands.json` in the repo root, pointing at whichever preset you most recently configured (see the top of [CMakeLists.txt](./CMakeLists.txt)). `.clangd` points at the repo root rather than a specific build directory, so clangd automatically follows whichever preset is active without any manual setup.

> **Windows:** creating this symlink requires **Developer Mode**. If it's off, CMake prints a `WARNING` during configure and clangd won't have a compilation database to work with. Enable it under **System → Advanced → For developers**, then reconfigure.

clangd only reloads the database when it's notified of a file change, so after switching presets it can occasionally lag behind. If IntelliSense looks stale or broken after a fresh configure, run **clangd: Restart language server** from the Command Palette (Ctrl+shift+P) to force it to pick up the change immediately.
