"""Shared helper for running (and optionally echoing) subprocess commands."""

import subprocess
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent.parent
BOLD = "\033[1m"
RESET = "\033[0m"


def run_command(command, echo):
    if echo:
        print(f"{BOLD}{' '.join(command)}{RESET}", flush=True)
    return subprocess.run(command, cwd=REPO_ROOT)
