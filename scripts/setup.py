#!/usr/bin/env python3
"""Check for (and optionally install) the tools recommended for developing stdx.

Detects the current OS's package manager (winget on Windows, apt on
Debian/Ubuntu, Homebrew on macOS), reports which recommended tools are
missing, and can install them on request. By default this only reports
what's missing; pass --install to actually run the install commands.
"""

import argparse
import json
import platform
import shutil
import subprocess
import sys

from utils.run_command import REPO_ROOT, run_command

EXTENSIONS_FILE = REPO_ROOT / ".vscode" / "extensions.json"


def load_recommended_extensions():
    if not EXTENSIONS_FILE.is_file():
        return []
    try:
        data = json.loads(EXTENSIONS_FILE.read_text())
    except json.JSONDecodeError:
        return []
    return data.get("recommendations") or []


class Dependency:
    def __init__(self, name, binaries, *, winget=None, apt=None, brew=None, brew_cask=False, manual_url=None):
        self.name = name
        self.binaries = binaries
        self.winget = winget
        self.apt = apt
        self.brew = brew
        self.brew_cask = brew_cask
        self.manual_url = manual_url

    def missing_binaries(self):
        return [b for b in self.binaries if shutil.which(b) is None]


DEPENDENCIES = [
    # git is probably installed because you cloned the repo
    Dependency(
        "Git",
        ["git"],
        winget=["Git.Git"],
        apt=["git"],
        brew=["git"],
        manual_url="https://git-scm.com/downloads",
    ),
    Dependency(
        "CMake",
        ["cmake"],
        winget=["Kitware.CMake"],
        apt=["cmake"],
        brew=["cmake"],
        manual_url="https://cmake.org/download/",
    ),
    Dependency(
        "Ninja",
        ["ninja"],
        winget=["Ninja-build.Ninja"],
        apt=["ninja-build"],
        brew=["ninja"],
        manual_url="https://github.com/ninja-build/ninja/releases",
    ),
    Dependency(
        "LLVM (clang, clang-tidy, clang-format, llvm-cov, llvm-profdata)",
        ["clang", "clang++", "clang-tidy", "clang-format", "llvm-cov", "llvm-profdata"],
        winget=["LLVM.LLVM"],
        apt=["llvm", "clang", "clang-tidy", "clang-format"],
        brew=["llvm"],
        manual_url="https://releases.llvm.org/",
    ),
    Dependency(
        "VS Code",
        ["code"],
        winget=["Microsoft.VisualStudioCode"],
        apt=None,
        brew=["visual-studio-code"],
        brew_cask=True,
        manual_url="https://code.visualstudio.com/download",
    ),
]


def detect_package_manager():
    system = platform.system()
    if system == "Windows":
        return "winget" if shutil.which("winget") else None
    if system == "Darwin":
        return "brew" if shutil.which("brew") else None
    if system == "Linux":
        return "apt" if shutil.which("apt-get") else None
    return None


def install_commands(dep, pm):
    if pm == "winget":
        if not dep.winget:
            return None
        return [["winget", "install", "--id", pkg, "-e", "--source", "winget"] for pkg in dep.winget]
    if pm == "apt":
        if not dep.apt:
            return None
        return [["sudo", "apt-get", "install", "-y"] + dep.apt]
    if pm == "brew":
        if not dep.brew:
            return None
        prefix = ["brew", "install"] + (["--cask"] if dep.brew_cask else [])
        return [prefix + dep.brew]
    return None


def confirm(prompt):
    return input(f"{prompt} [y/N] ").strip().lower() in ("y", "yes")


def handle_dependency(dep, pm, missing, install, yes, echo):
    print(f"- {dep.name}: missing {', '.join(missing)}")
    commands = install_commands(dep, pm)

    if not commands:
        hint = f" See {dep.manual_url}" if dep.manual_url else ""
        print(f"  no automatic install available for this platform; install manually.{hint}")
        return

    if not install:
        for command in commands:
            print(f"  would run: {' '.join(command)}")
        return

    if not yes and not confirm(f"  Install {dep.name}?"):
        print("  skipped.")
        return

    for command in commands:
        result = run_command(command, echo)
        if result.returncode != 0:
            print(f"  error: '{' '.join(command)}' failed (exit {result.returncode})", file=sys.stderr)
            if dep.manual_url:
                print(f"  see {dep.manual_url} to install manually.", file=sys.stderr)
            return

    if pm == "brew" and dep.name.startswith("LLVM"):
        print("  note: Homebrew's llvm formula is keg-only; add $(brew --prefix llvm)/bin to your PATH "
              "(see `brew info llvm`) so clang/clang-tidy/clang-format are found on PATH.")


def handle_vscode_extensions(install, yes, echo):
    extensions = load_recommended_extensions()
    if not extensions:
        print(f"No VS Code extension recommendations found in {EXTENSIONS_FILE.relative_to(REPO_ROOT)}; skipping.")
        return

    code = shutil.which("code")
    if code is None:
        print("VS Code not found on PATH; skipping extension check (install VS Code and re-run to add extensions).")
        return

    result = subprocess.run([code, "--list-extensions"], capture_output=True, text=True)
    installed_extensions = set(result.stdout.splitlines())

    print("Checking VS Code extensions:")
    any_missing = False
    for ext_id in extensions:
        if ext_id in installed_extensions:
            print(f"- {ext_id}: installed")
            continue

        any_missing = True
        command = [code, "--install-extension", ext_id]
        print(f"- {ext_id}: missing")
        if not install:
            print(f"  would run: {' '.join(command)}")
            continue
        if not yes and not confirm(f"  Install {ext_id}?"):
            print("  skipped.")
            continue
        run_command(command, echo)

    if not any_missing:
        print("All recommended VS Code extensions are already installed; no action needed.")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--install",
        action="store_true",
        help="Install missing tools instead of just reporting them.",
    )
    parser.add_argument(
        "-y",
        "--yes",
        action="store_true",
        help="Don't prompt for confirmation before installing each tool (implies --install).",
    )
    parser.add_argument(
        "-e",
        "--echo",
        action="store_true",
        help="Echo each command as it is run.",
    )
    args = parser.parse_args()
    install = args.install or args.yes

    pm = detect_package_manager()
    if pm is None:
        print(
            f"warning: no supported package manager found for {platform.system()} "
            "(supported: winget on Windows, apt on Debian/Ubuntu, Homebrew on macOS); "
            "commands will be printed for manual use.",
            file=sys.stderr,
        )

    print("Checking command-line tools:")
    missing_deps = []
    for dep in DEPENDENCIES:
        missing = dep.missing_binaries()
        if not missing:
            print(f"- {dep.name}: installed")
            continue
        missing_deps.append(dep)
        handle_dependency(dep, pm, missing, install, args.yes, args.echo)

    if not missing_deps:
        print("All recommended command-line tools are already installed; no action needed.")

    print()
    handle_vscode_extensions(install, args.yes, args.echo)

    if missing_deps and not install:
        print("\nRun with --install to install the missing tools (--yes to skip confirmation prompts).")

    return 0


if __name__ == "__main__":
    sys.exit(main())
