#!/usr/bin/env python3
"""Run clang-tidy (via run-clang-tidy) over the C++ sources in the repo."""

import argparse
import re
import shutil
import sys
from pathlib import Path

from utils.run_command import REPO_ROOT, run_command

# run-clang-tidy treats each positional argument as a regex matched against the absolute file
# paths in compile_commands.json, not a directory to search. We anchor on this project's own
# src/ directory (by absolute path) rather than the bare word "src" so that vendored dependency
# sources (e.g. googletest, fetched into build/<preset>/_deps/googletest-src/... when unit tests
# are enabled) are never swept in just because they also happen to contain a "src" path segment.
PATHS = [re.escape((REPO_ROOT / "src").as_posix()).replace(r"/", r"[/\\]")]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "-p",
        "--build-dir",
        default=str(REPO_ROOT),
        help=f"Build directory containing compile_commands.json (default: {REPO_ROOT}).",
    )
    parser.add_argument(
        "-e",
        "--echo",
        action="store_true",
        help="Echo each command as it is run.",
    )
    args = parser.parse_args()

    clang_tidy = shutil.which("clang-tidy")
    if clang_tidy is None:
        print("error: clang-tidy not found on PATH", file=sys.stderr)
        return 1

    # run-clang-tidy ships as an extension-less Python script next to clang-tidy; shutil.which()
    # can't find it on Windows because it isn't a recognized PATHEXT extension.
    run_clang_tidy = Path(clang_tidy).parent / "run-clang-tidy"
    if not run_clang_tidy.is_file():
        print(f"error: {run_clang_tidy} not found", file=sys.stderr)
        return 1

    version_result = run_command([clang_tidy, "--version"], args.echo)
    if version_result.returncode != 0:
        return version_result.returncode

    command = ["python", str(run_clang_tidy), "-p", args.build_dir] + PATHS + ["-use-color"]

    return run_command(command, args.echo).returncode


if __name__ == "__main__":
    sys.exit(main())
