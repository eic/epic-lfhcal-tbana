#!/usr/bin/env python3
import hashlib
import os
import pathlib
import shlex
import shutil
import subprocess
import tempfile
import unittest


REPOSITORY = pathlib.Path(__file__).resolve().parents[2]
BUILD = pathlib.Path(os.environ.get(
    "LFHCAL_BUILD_DIR", REPOSITORY / "NewStructure/build")).resolve()
DATAANA = BUILD / "DataAna"
ROOT_CONFIG = shutil.which("root-config")


class DataAnaQACalibrationOutputTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if not DATAANA.is_file() or ROOT_CONFIG is None:
            raise unittest.SkipTest("DataAna and root-config are required")
        cls.temporary = tempfile.TemporaryDirectory(prefix="lfhcal-dataana-")
        cls.work = pathlib.Path(cls.temporary.name)
        cls.mapping = cls.work / "mapping.txt"
        cls.mapping.write_text("sumOpt\t0\n0\t0\t0\tA0\t0\t0\t0\t0\t0\t0\t1\n")
        cls.runlist = cls.work / "runs.csv"
        cls.runlist.write_text(
            "year 2024\nmonth 09\nreadout CAEN\nfacility TEST\n"
            "beam-line TEST\n42,Muon +,-13,5,44,41,50,50,0,0,20,25\n")
        cls.input = cls.work / "input.root"
        generator = cls.work / "make_dataana_fixture"
        flags = subprocess.check_output(
            [ROOT_CONFIG, "--cflags", "--libs"], text=True, timeout=30)
        compiler = shlex.split(os.environ.get("CXX", "c++"))
        command = compiler + [
            str(pathlib.Path(__file__).with_name("dataana_fixture.cc")),
            "-I" + str(REPOSITORY / "NewStructure"),
            "-L" + str(BUILD), "-lLFHCAL",
            "-Wl,-rpath," + str(BUILD), "-o", str(generator),
        ] + shlex.split(flags)
        subprocess.run(command, check=True, timeout=120)
        subprocess.run(
            [str(generator), str(cls.mapping), str(cls.input)],
            check=True, timeout=60)
        cls.input_hash = cls.sha256(cls.input)

    @classmethod
    def tearDownClass(cls):
        if hasattr(cls, "temporary"):
            cls.temporary.cleanup()

    @staticmethod
    def sha256(path):
        return hashlib.sha256(path.read_bytes()).hexdigest()

    def run_dataana(self, mode="-q", histogram=None, extra=(), timeout=180):
        histogram = histogram or self.work / "qa.root"
        plot_dir = self.work / (histogram.stem + "-plots")
        command = [
            str(DATAANA), mode, "-f", "-F", "root", "-n", "1",
            "-i", str(self.input), "-O", str(histogram),
            "-r", str(self.runlist), "-P", str(plot_dir),
        ]
        command[1:1] = list(extra)
        return subprocess.run(
            command, text=True, capture_output=True, timeout=timeout)

    def assert_input_unchanged(self):
        self.assertEqual(self.sha256(self.input), self.input_hash)

    def assert_healthy_root(self, path, require_keys=True):
        key_check = " || f.GetNkeys()==0" if require_keys else ""
        check = subprocess.run([
            "root", "-l", "-b", "-q", "-e",
            (f'TFile f("{path}"); gSystem->Exit('
             f'f.IsZombie() || f.TestBit(TFile::kRecovered){key_check});'),
        ], text=True, capture_output=True, timeout=60)
        self.assertEqual(check.returncode, 0, check.stdout + check.stderr)

    def test_documented_full_and_simple_qa_use_histogram_name(self):
        for mode, stem in (("-Q", "full"), ("-q", "simple")):
            with self.subTest(mode=mode):
                histogram = self.work / f"{stem}.root"
                result = self.run_dataana(mode, histogram, extra=("-a",))
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                calibration = self.work / f"{stem}_calib.txt"
                self.assertIn("#Calib info:", calibration.read_text())
                self.assert_healthy_root(histogram)
                self.assert_input_unchanged()

    def test_without_a_does_not_export_calibration(self):
        histogram = self.work / "baseline.root"
        result = self.run_dataana(histogram=histogram)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertFalse((self.work / "baseline_calib.txt").exists())
        self.assert_healthy_root(histogram)
        self.assert_input_unchanged()

    def test_tree_output_name_takes_precedence(self):
        histogram = self.work / "precedence_hist.root"
        tree = self.work / "precedence_tree.root"
        result = self.run_dataana(
            histogram=histogram, extra=("-a", "-o", str(tree)))
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertTrue((self.work / "precedence_tree_calib.txt").is_file())
        self.assertFalse((self.work / "precedence_hist_calib.txt").exists())
        self.assert_healthy_root(histogram)
        self.assert_healthy_root(tree, require_keys=False)
        self.assert_input_unchanged()

    def test_full_and_simple_modes_can_run_together(self):
        histogram = self.work / "dual-mode.root"
        result = self.run_dataana(
            "-Q", histogram, extra=("-a", "-q"))
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertEqual(result.stdout.count("QA data"), 2)
        self.assertIn(
            "#Calib info:",
            (self.work / "dual-mode_calib.txt").read_text())
        self.assert_healthy_root(histogram)
        self.assert_input_unchanged()

    def test_missing_parent_is_rejected_before_input_open(self):
        histogram = self.work / "missing" / "early.root"
        result = self.run_dataana(histogram=histogram, extra=("-a",))
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("calibration text output parent", result.stderr)
        self.assertNotIn("Input name set", result.stdout)
        self.assert_input_unchanged()

    def test_runtime_write_failure_names_operation_and_file(self):
        histogram = self.work / "failure.root"
        calibration = self.work / "failure_calib.txt"
        calibration.symlink_to("/dev/full")
        result = self.run_dataana(
            histogram=histogram, extra=("-a", "-Q"))
        self.assertNotEqual(result.returncode, 0)
        self.assertEqual(result.stdout.count("QA data"), 1)
        self.assertIn(
            f"writing calibration text output '{calibration}' failed",
            result.stderr)
        self.assert_input_unchanged()

    def test_no_qa_mode_is_rejected_before_input_open(self):
        result = subprocess.run([
            str(DATAANA), "-d", "0", "-i", str(self.input),
            "-O", str(self.work / "no-mode.root"),
        ], text=True, capture_output=True, timeout=20)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("select at least one QA mode (-Q or -q)", result.stderr)
        self.assertNotIn("Input name set", result.stdout)
        self.assert_input_unchanged()


if __name__ == "__main__":
    unittest.main(verbosity=2)
