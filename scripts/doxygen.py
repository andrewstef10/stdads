#!/usr/bin/env python3
"""Generate the project's Doxygen documentation."""

import argparse
import shutil
import sys

from utils.run_command import run_command

DOXYFILE = "Doxyfile"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "-e",
        "--echo",
        action="store_true",
        help="Echo each command as it is run.",
    )
    args = parser.parse_args()

    doxygen = shutil.which("doxygen")
    if doxygen is None:
        print("error: doxygen not found on PATH", file=sys.stderr)
        return 1

    version_result = run_command([doxygen, "--version"], args.echo)
    if version_result.returncode != 0:
        return version_result.returncode

    return run_command([doxygen, DOXYFILE], args.echo).returncode


if __name__ == "__main__":
    sys.exit(main())
