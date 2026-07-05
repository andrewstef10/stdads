#!/usr/bin/env python3
"""Run clang-tidy (via run-clang-tidy) over the C++ sources in the repo."""

import argparse
import shutil
import sys
from pathlib import Path

from utils.run_command import REPO_ROOT, run_command

PATHS = ["stdx.src"]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "-p",
        "--build-dir",
        default=REPO_ROOT,
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
