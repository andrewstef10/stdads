#!/usr/bin/env python3
"""Format (or check formatting of) all C/C++ sources in the repo using clang-format."""

import argparse
import shutil
import sys
from pathlib import Path

from utils.run_command import REPO_ROOT, run_command

SOURCE_DIRS = ["include", "src", "tests"]
EXTENSIONS = {".h", ".hpp", ".hh", ".c", ".cpp", ".cc", ".cxx"}


def find_sources_in(directory):
    files = []
    for path in directory.rglob("*"):
        if path.is_file() and path.suffix in EXTENSIONS:
            files.append(path)
    return files


def resolve_paths(paths):
    if not paths:
        directories = [REPO_ROOT / dir_name for dir_name in SOURCE_DIRS]
        files = []
        for directory in directories:
            if directory.is_dir():
                files.extend(find_sources_in(directory))
        return sorted(files)

    files = []
    for path in paths:
        if path.is_dir():
            files.extend(find_sources_in(path))
        else:
            files.append(path)
    return sorted(files)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "paths",
        nargs="*",
        type=Path,
        help="Specific file(s) or directories to format/check. Defaults to the following directories: " + str(SOURCE_DIRS),
    )
    parser.add_argument(
        "-c",
        "--check",
        action="store_true",
        help="Don't modify files; exit non-zero if any file is not formatted (for CI).",
    )
    parser.add_argument(
        "-e",
        "--echo",
        action="store_true",
        help="Echo each command as it is run.",
    )
    args = parser.parse_args()

    clang_format = shutil.which("clang-format")
    if clang_format is None:
        print("error: clang-format not found on PATH", file=sys.stderr)
        return 1

    files = resolve_paths(args.paths)
    if not files:
        print("No C++ source files found.")
        return 0

    # Run version command
    version_result = run_command([clang_format, "--version"], args.echo)
    if version_result.returncode != 0:
        return version_result.returncode

    if args.check:
        command = [clang_format, "--style=file", "--dry-run", "--Werror"]
    else:
        command = [clang_format, "--style=file", "-i"]

    full_command = command + [str(f) for f in files]
    result = run_command(full_command, args.echo)
    if result.returncode != 0:
        if args.check:
            print("clang-format found unformatted files.", file=sys.stderr)
        return result.returncode

    if not args.check:
        print(f"Formatted {len(files)} file(s).")
    else:
        print(f"Checked {len(files)} file(s); all formatted correctly.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
