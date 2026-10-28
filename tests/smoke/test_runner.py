"""Offline tests of runner mechanics only; fake commands do not validate ROOT."""
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import time
import unittest

RUNNER = Path(__file__).resolve().with_name("run.py")
VALIDATOR = Path(__file__).resolve().with_name("validate.sh")
EXPECTED_REJECTION = "FAIL: ROOT write / calibration: exit 1; expected marker 'LFHCAL_SMOKE_OK: write'"
STANDARD_EXECUTABLES = [
    "Convert", "DataPrep", "CalibrateData", "DataAna", "Display",
    "CompareCalib", "CompareAna", "CompareInjection", "HGCROCStudy",
    "CompareWaveform",
]
DECODER_EXECUTABLES = [
    "HGCROC_Conversion", "ParseCalibSamples", "CompareHGCROCCalib",
    "EvaluateRecoEffi", "h2g_run",
]


class RunnerTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="smoke runner tests ")
        self.addCleanup(self.temp.cleanup)
        self.directory = Path(self.temp.name)
        self.build = self.directory / "build artifacts"
        self.build.mkdir()
        self.calls = self.directory / "calls.jsonl"
        self.env = dict(os.environ, SMOKE_TEST_CALLS=str(self.calls))
        for name in ("libLFHCAL.so", "libLFHCAL_rdict.pcm", "libLFHCAL.rootmap"):
            (self.build / name).write_text("fixture artifact, not a real library\n")
        script = f"#!{sys.executable}\n" + '''import json, os, pathlib, subprocess, sys, time
name = pathlib.Path(sys.argv[0]).name
with open(os.environ["SMOKE_TEST_CALLS"], "a") as stream:
    stream.write(json.dumps({"name": name, "args": sys.argv[1:], "cwd": os.getcwd(),
                             "mode": os.environ.get("LFHCAL_SMOKE_MODE")}) + "\\n")
if name == os.environ.get("SMOKE_TEST_CHILD_HANG"):
    child_code = "import pathlib, time; time.sleep(0.8); pathlib.Path(" + repr(os.environ["SMOKE_TEST_CHILD_MARKER"]) + ").touch()"
    subprocess.Popen([sys.executable, "-c", child_code])
    time.sleep(5)
if name == os.environ.get("SMOKE_TEST_HANG"):
    time.sleep(5)
if name == os.environ.get("SMOKE_TEST_FAIL"):
    print("injected command failure")
    sys.exit(17)
if name == os.environ.get("SMOKE_TEST_EMPTY"):
    sys.exit(0)
if name in ''' + repr(STANDARD_EXECUTABLES + DECODER_EXECUTABLES) + ''':
    print("Usage: " + name)
elif name == "root":
    print("LFHCAL_SMOKE_OK: " + os.environ["LFHCAL_SMOKE_MODE"])
else:
    print("fixture-version")
'''
        for name in (*STANDARD_EXECUTABLES, *DECODER_EXECUTABLES,
                     "root", "root-config", "yall-run"):
            path = self.build / ("h2g_decode/h2g_run" if name == "h2g_run" else name)
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(script)
            path.chmod(0o755)

    def run_suite(self, *extra, layout=None):
        return subprocess.run([sys.executable, str(RUNNER),
                               *(layout or ["--build-dir", str(self.build)]),
                               "--root", str(self.build / "root"),
                               "--root-config", str(self.build / "root-config"), *extra],
                              env=self.env, text=True, capture_output=True)

    def recorded(self):
        return [json.loads(line) for line in self.calls.read_text().splitlines()]

    def test_default_checks_are_bounded_and_do_not_run_yall(self):
        result = self.run_suite()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        calls = self.recorded()
        self.assertEqual([row["name"] for row in calls],
                         ["root-config", *STANDARD_EXECUTABLES, "root", "root"])
        self.assertEqual(calls[0]["args"], ["--version"])
        standard_calls = calls[1:1 + len(STANDARD_EXECUTABLES)]
        self.assertTrue(all(row["args"] == ["-h"] for row in standard_calls))
        self.assertEqual([row["mode"] for row in calls[-2:]], ["write", "read"])
        self.assertEqual(calls[-1]["args"][:4], ["-n", "-l", "-b", "-q"])
        self.assertEqual(len({row["cwd"] for row in calls}), 1)
        self.assertFalse(Path(calls[0]["cwd"]).exists(), "temporary fixture directory leaked")
        self.assertIn("NOT REQUESTED: decoder-enabled", result.stdout)
        self.assertIn("NOT REQUESTED: Yall", result.stdout)

    def test_decoder_checks_are_explicit_and_bounded(self):
        result = self.run_suite("--decoder")
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        calls = self.recorded()
        self.assertEqual([row["name"] for row in calls],
                         ["root-config", *STANDARD_EXECUTABLES,
                          *DECODER_EXECUTABLES, "root", "root"])
        startup_calls = calls[1:1 + len(STANDARD_EXECUTABLES) + len(DECODER_EXECUTABLES)]
        self.assertTrue(all(row["args"] == ["-h"] for row in startup_calls))
        self.assertNotIn("NOT REQUESTED: decoder-enabled", result.stdout)

    def test_requested_decoder_component_must_exist(self):
        (self.build / "ParseCalibSamples").unlink()
        result = self.run_suite("--decoder")
        self.assertEqual(result.returncode, 1)
        self.assertIn("ParseCalibSamples", result.stderr)
        self.assertFalse(self.calls.exists())

    def test_yall_is_version_only(self):
        result = self.run_suite("--yall", str(self.build / "yall-run"))
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertEqual(self.recorded()[-1]["args"], ["--version"])
        self.assertEqual(self.recorded()[-1]["name"], "yall-run")

    def test_nonzero_command_fails_and_stops(self):
        self.env["SMOKE_TEST_FAIL"] = "Convert"
        result = self.run_suite()
        self.assertEqual(result.returncode, 1)
        self.assertIn("exit 17", result.stderr)
        self.assertEqual([row["name"] for row in self.recorded()], ["root-config", "Convert"])

    def test_zero_exit_without_help_text_fails_and_stops(self):
        self.env["SMOKE_TEST_EMPTY"] = "Display"
        result = self.run_suite()
        self.assertEqual(result.returncode, 1)
        self.assertIn("Display startup: exit 0; expected marker 'Usage:'", result.stderr)
        self.assertEqual([row["name"] for row in self.recorded()],
                         ["root-config", *STANDARD_EXECUTABLES[:5]])

    def test_zero_exit_without_root_success_marker_is_failure(self):
        self.env["SMOKE_TEST_EMPTY"] = "root"
        result = self.run_suite()
        self.assertEqual(result.returncode, 1)
        self.assertIn("expected marker", result.stderr)

    def test_empty_version_fails(self):
        for name, extra in (("root-config", []), ("yall-run", ["--yall", str(self.build / "yall-run")])):
            with self.subTest(name=name):
                self.env["SMOKE_TEST_EMPTY"] = name
                result = self.run_suite(*extra)
                self.assertEqual(result.returncode, 1)
                self.assertIn("empty output", result.stderr)

    def test_timeout_is_failure(self):
        self.env["SMOKE_TEST_HANG"] = "Convert"
        result = self.run_suite("--timeout", "0.3")
        self.assertEqual(result.returncode, 1)
        self.assertIn("timed out", result.stderr)

    @unittest.skipUnless(os.name == "posix", "process-group cleanup is POSIX-specific")
    def test_timeout_terminates_command_descendants(self):
        marker = self.directory / "child-survived"
        self.env["SMOKE_TEST_CHILD_HANG"] = "root"
        self.env["SMOKE_TEST_CHILD_MARKER"] = str(marker)
        result = self.run_suite("--timeout", "0.3")
        self.assertEqual(result.returncode, 1)
        self.assertIn("timed out", result.stderr)
        time.sleep(0.9)
        self.assertFalse(marker.exists(), "timed-out command left its child running")

    def test_missing_dictionary_fails_before_execution(self):
        (self.build / "libLFHCAL_rdict.pcm").unlink()
        result = self.run_suite()
        self.assertEqual(result.returncode, 1)
        self.assertIn("dictionary artifact", result.stderr)
        self.assertFalse(self.calls.exists())

    def test_missing_executable_fails(self):
        (self.build / "DataPrep").unlink()
        result = self.run_suite()
        self.assertEqual(result.returncode, 1)
        self.assertIn("Executable not found", result.stderr)

    def test_explicit_layout_does_not_guess_library(self):
        result = self.run_suite(layout=["--bin-dir", str(self.build)])
        self.assertEqual(result.returncode, 2)
        result = self.run_suite(layout=["--bin-dir", str(self.build),
                                       "--library", str(self.build / "libLFHCAL.so")])
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_invalid_timeout_rejected(self):
        for value in ("0", "-1", "nan", "inf"):
            with self.subTest(value=value):
                result = self.run_suite("--timeout", value)
                self.assertEqual(result.returncode, 2)


class ValidationHelperTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="smoke validator tests ")
        self.addCleanup(self.temp.cleanup)
        self.directory = Path(self.temp.name)
        self.state = self.directory / "calls"
        python = self.directory / "python3"
        python.write_text("""#!/bin/sh
if [ "$VALIDATE_EXPECT_DECODER" = 1 ]; then
  found=0
  for arg in "$@"; do
    if [ "$arg" = --decoder ]; then
      found=1
    fi
  done
  if [ "$found" -ne 1 ]; then
    echo "missing forwarded --decoder" >&2
    exit 9
  fi
fi
count=0
if [ -f "$VALIDATE_TEST_STATE" ]; then
  count=$(cat "$VALIDATE_TEST_STATE")
fi
count=$((count + 1))
echo "$count" >"$VALIDATE_TEST_STATE"
if [ "$count" -eq 1 ]; then
  echo "ordinary smoke output"
  if [ "$VALIDATE_TEST_CASE" = baseline_failure ]; then
    echo "genuine baseline failure" >&2
    exit 7
  fi
  exit 0
fi
case "$VALIDATE_TEST_CASE" in
  expected)
    echo "FAIL: ROOT write / calibration: exit 1; expected marker 'LFHCAL_SMOKE_OK: write'" >&2
    exit 1 ;;
  unexpected_success)
    echo "injected command unexpectedly passed"
    exit 0 ;;
  wrong_stage)
    echo "FAIL: Convert startup: exit 1; expected marker 'Usage:'" >&2
    exit 1 ;;
  wrong_exit)
    echo "FAIL: ROOT write / calibration: exit 1; expected marker 'LFHCAL_SMOKE_OK: write'" >&2
    exit 2 ;;
  timeout)
    echo "FAIL: ROOT write / calibration: timed out after 0.1 seconds" >&2
    exit 1 ;;
esac
""")
        python.chmod(0o755)

    def run_helper(self, case, *extra):
        env = dict(os.environ,
                   PATH=f"{self.directory}{os.pathsep}{os.environ.get('PATH', '')}",
                   VALIDATE_TEST_STATE=str(self.state),
                   VALIDATE_TEST_CASE=case,
                   VALIDATE_EXPECT_DECODER="1" if "--decoder" in extra else "0")
        return subprocess.run([str(VALIDATOR), "--build-dir", "build", "--timeout", "60", *extra],
                              env=env, text=True, capture_output=True)

    def test_expected_rejection_is_accepted(self):
        result = self.run_helper("expected")
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn("NORMAL SMOKE TESTS", result.stdout)
        self.assertIn("ERROR-DETECTION TEST: deliberately forcing ROOT to fail", result.stdout)
        self.assertIn("PASS: expected error detected", result.stdout)
        self.assertNotIn(EXPECTED_REJECTION, result.stdout + result.stderr)
        self.assertEqual(self.state.read_text().strip(), "2")

    def test_decoder_selection_is_forwarded_to_both_runs(self):
        result = self.run_helper("expected", "--decoder")
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertEqual(self.state.read_text().strip(), "2")

    def test_unexpected_success_is_rejected(self):
        result = self.run_helper("unexpected_success")
        self.assertEqual(result.returncode, 1)
        self.assertIn("got exit 0", result.stderr)
        self.assertIn("injected command unexpectedly passed", result.stderr)

    def test_wrong_stage_is_rejected(self):
        result = self.run_helper("wrong_stage")
        self.assertEqual(result.returncode, 1)
        self.assertIn("Convert startup", result.stderr)

    def test_wrong_exit_is_rejected(self):
        result = self.run_helper("wrong_exit")
        self.assertEqual(result.returncode, 1)
        self.assertIn("got exit 2", result.stderr)
        self.assertIn(EXPECTED_REJECTION, result.stderr)

    def test_timeout_is_rejected(self):
        result = self.run_helper("timeout")
        self.assertEqual(result.returncode, 1)
        self.assertIn("timed out after 0.1 seconds", result.stderr)

    def test_baseline_failure_stops_before_injection_and_stays_visible(self):
        result = self.run_helper("baseline_failure")
        self.assertEqual(result.returncode, 7)
        self.assertIn("ordinary smoke output", result.stdout)
        self.assertIn("genuine baseline failure", result.stderr)
        self.assertNotIn("ERROR-DETECTION TEST", result.stdout + result.stderr)
        self.assertEqual(self.state.read_text().strip(), "1")


if __name__ == "__main__":
    unittest.main()
