#!/usr/bin/env python3
"""Run black-box match tests against the spark-compiler CLI."""

from __future__ import annotations

import argparse
import difflib
import os
import shlex
import subprocess
import sys
import tempfile
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class MatchTest:
    path: Path
    cli: list[str]
    source: str
    expected: str
    expect_failure: bool


def normalize(text: str) -> str:
    """Ignore platform line endings and insignificant trailing blank lines."""
    return text.replace("\r\n", "\n").replace("\r", "\n").strip()


def status_label(status: str, stream: object) -> str:
    if not getattr(stream, "isatty")() or "NO_COLOR" in os.environ:
        return status

    color = "\033[32m" if status == "PASS" else "\033[31m"
    return f"{color}{status}\033[0m"


def load_test(path: Path) -> MatchTest:
    cli_lines: list[str] = []
    source_lines: list[str] = []
    expected_lines: list[str] = []
    section: str | None = None
    expect_failure = False

    for line in path.read_text(encoding="utf-8").splitlines():
        if line == "#cli":
            section = "cli"
        elif line == "#src":
            section = "src"
        elif line == "#expect":
            section = "expect"
        elif line == "#expect_failure":
            expect_failure = True
        elif section == "cli":
            cli_lines.append(line)
        elif section == "src":
            source_lines.append(line)
        elif section == "expect":
            expected_lines.append(line)

    if not cli_lines:
        raise ValueError("missing #cli section")
    if not source_lines:
        raise ValueError("missing #src section")
    if not expect_failure and section != "expect":
        raise ValueError("missing #expect section")

    return MatchTest(
        path=path,
        cli=shlex.split("\n".join(cli_lines)),
        source="\n".join(source_lines).strip(),
        expected="\n".join(expected_lines).strip(),
        expect_failure=expect_failure,
    )


def run_test(test: MatchTest, compiler: Path) -> str | None:
    if any(argument in {"--src", "--code"} or argument.startswith(("--src=", "--code=")) for argument in test.cli):
        return "#cli must not specify --src or --code; the runner supplies --code"

    with tempfile.TemporaryDirectory(prefix="sparkc-match-") as directory:
        result = subprocess.run(
            [str(compiler), "--code", test.source, *test.cli],
            capture_output=True,
            text=True,
            encoding="utf-8",
            cwd=directory,
        )

    if test.expect_failure:
        if result.returncode != 0:
            return None
        return "expected compilation to fail, but it exited with code 0"

    if result.returncode != 0:
        return f"compiler exited with code {result.returncode}:\n{result.stdout}{result.stderr}"

    actual = normalize(result.stdout)
    expected = normalize(test.expected)
    if actual == expected:
        return None

    diff = difflib.unified_diff(
        expected.splitlines(),
        actual.splitlines(),
        fromfile="expected",
        tofile="actual",
        lineterm="",
    )
    return "output differs:\n" + "\n".join(diff)


def test_paths(target: Path) -> list[Path]:
    if target.is_file():
        return [target]
    return sorted(target.rglob("*.test"))


def main() -> int:
    root = Path(__file__).resolve().parent
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--compiler",
        type=Path,
        default=root.parent / "build" / "spark-compiler.exe",
        help="path to the built spark-compiler executable",
    )
    parser.add_argument(
        "target",
        nargs="?",
        type=Path,
        default=root / "cases",
        help="a test file or directory (default: test/cases)",
    )
    args = parser.parse_args()

    compiler = args.compiler.resolve()
    if not compiler.is_file():
        parser.error(f"compiler not found: {compiler}")

    target = args.target.resolve()
    if not target.exists():
        parser.error(f"test target not found: {target}")

    paths = test_paths(target)
    if not paths:
        parser.error(f"no .test files found in: {target}")

    failed = 0
    total = len(paths)
    for number, path in enumerate(paths, start=1):
        try:
            error = run_test(load_test(path), compiler)
        except (OSError, ValueError) as exc:
            error = str(exc)

        relative_path = path.relative_to(root.parent)
        prefix = f"[{number}/{total}]"
        if error is None:
            print(f"{prefix} {status_label('PASS', sys.stdout)} {relative_path}")
        else:
            failed += 1
            print(
                f"{prefix} {status_label('FAIL', sys.stderr)} {relative_path}\n{error}",
                file=sys.stderr,
            )

    print(f"{len(paths) - failed}/{len(paths)} tests passed")
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
