#!/usr/bin/env python3
"""Bounded checks of an already-built LFHCal stack. Never installs or submits jobs."""
import argparse
import math
import os
from pathlib import Path
import shutil
import signal
import subprocess
import sys
import tempfile

HERE = Path(__file__).resolve().parent


class SmokeFailure(RuntimeError):
    pass


def executable(value):
    path = shutil.which(str(value))
    if path is None:
        raise SmokeFailure(f"Executable not found or not executable: {value}")
    return str(Path(path).resolve())


def run_check(label, command, *, cwd, timeout, env=None, marker=None):
    print(f"CHECK {label}", flush=True)
    try:
        with subprocess.Popen(command, cwd=cwd, env=env, text=True,
                              stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                              start_new_session=(os.name == "posix")) as process:
            try:
                output, _ = process.communicate(timeout=timeout)
            except subprocess.TimeoutExpired as error:
                # ROOT launches root.exe as a child. Kill the entire isolated
                # group so a timeout cannot leave that workload running.
                if os.name == "posix":
                    try:
                        os.killpg(process.pid, signal.SIGKILL)
                    except ProcessLookupError:
                        pass
                else:
                    process.kill()
                process.communicate()
                raise SmokeFailure(f"{label}: timed out after {timeout:g} seconds") from error
            returncode = process.returncode
    except OSError as error:
        raise SmokeFailure(f"{label}: {error}") from error
    if returncode != 0 or (marker and marker not in output):
        detail = output[-8000:]
        raise SmokeFailure(f"{label}: exit {returncode}; expected marker {marker!r}\n{detail}")
    print(f"PASS  {label}", flush=True)
    return output


def positive_timeout(value):
    number = float(value)
    if not math.isfinite(number) or number <= 0:
        raise argparse.ArgumentTypeError("timeout must be finite and positive")
    return number


def parse_args(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    layout = parser.add_mutually_exclusive_group(required=True)
    layout.add_argument("--build-dir", type=Path,
                        help="CMake output directory containing Convert, DataPrep and libLFHCAL")
    layout.add_argument("--bin-dir", type=Path,
                        help="explicit binary directory for a copied/deployed layout; requires --library")
    parser.add_argument("--library", type=Path,
                        help="exact libLFHCAL shared library (required with --bin-dir)")
    parser.add_argument("--root", default="root", help="ROOT executable, default: PATH root")
    parser.add_argument("--root-config", default="root-config", help="matching ROOT configuration executable")
    parser.add_argument("--yall", metavar="EXECUTABLE",
                        help="optional readiness check only: run this yall-run executable with --version")
    parser.add_argument("--timeout", type=positive_timeout, default=30,
                        help="seconds per command (default: 30); timeouts fail the suite")
    args = parser.parse_args(argv)
    if args.bin_dir and not args.library:
        parser.error("--bin-dir requires --library; the runner does not guess an installation layout")
    return args


def main(argv=None):
    args = parse_args(argv)
    try:
        binaries = (args.build_dir or args.bin_dir).resolve()
        if args.library:
            library = args.library.resolve()
        else:
            candidates = [binaries / name for name in ("libLFHCAL.so", "libLFHCAL.dylib")]
            present = [path for path in candidates if path.is_file()]
            if len(present) != 1:
                raise SmokeFailure(f"Expected exactly one libLFHCAL.so/.dylib in {binaries}")
            library = present[0]
        for path in (library, library.parent / "libLFHCAL_rdict.pcm",
                     library.parent / "libLFHCAL.rootmap"):
            if not path.is_file() or path.stat().st_size == 0:
                raise SmokeFailure(f"Missing or empty library/dictionary artifact: {path}")
        commands = {name: executable(binaries / name) for name in ("Convert", "DataPrep")}
        root = executable(args.root)
        root_config = executable(args.root_config)
        yall = executable(args.yall) if args.yall else None
        print(f"Artifacts: {binaries}\nLibrary:   {library}\nROOT:      {root}", flush=True)
        # A clean working directory avoids source/build-tree ROOT autoload and
        # ensures every fixture is temporary. No LD_LIBRARY_PATH is injected.
        with tempfile.TemporaryDirectory(prefix="lfhcal-smoke-") as directory:
            work = Path(directory)
            version = run_check("ROOT dependency", [root_config, "--version"],
                                cwd=work, timeout=args.timeout).strip()
            if not version:
                raise SmokeFailure("root-config --version returned empty output")
            print(f"ROOT version: {version}", flush=True)
            for name, command in commands.items():
                run_check(f"{name} startup", [command, "-h"], cwd=work,
                          timeout=args.timeout, marker="Usage:")
            env = dict(os.environ, LFHCAL_SMOKE_LIBRARY=str(library),
                       LFHCAL_SMOKE_PAYLOAD=str(HERE / "payload.C"),
                       LFHCAL_SMOKE_FILE=str(work / "fixture.root"))
            for mode in ("write", "read"):
                env["LFHCAL_SMOKE_MODE"] = mode
                run_check(f"ROOT {mode} / calibration", [root, "-n", "-l", "-b", "-q",
                          str(HERE / "driver.C")], cwd=work, env=env,
                          timeout=args.timeout, marker=f"LFHCAL_SMOKE_OK: {mode}")
            if yall:
                version = run_check("Yall readiness only", [yall, "--version"],
                                    cwd=work, timeout=args.timeout).strip()
                if not version:
                    raise SmokeFailure("yall-run --version returned empty output")
                print(f"Yall version: {version}", flush=True)
            else:
                print("NOT REQUESTED: Yall readiness (use --yall /path/to/yall-run)", flush=True)
        print("PASS: baseline LFHCal smoke checks. No campaigns or batch jobs were run.")
        return 0
    except (SmokeFailure, OSError) as error:
        print(f"FAIL: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
