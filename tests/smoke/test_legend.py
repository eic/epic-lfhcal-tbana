"""ROOT legend-lifetime regression; requires the production fix in upstream #94."""
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import tempfile
import unittest


class LegendLifetimeTest(unittest.TestCase):
    def test_temporary_fit_survives_repaint(self):
        source = Path(os.environ.get("LFHCAL_SOURCE_DIR", Path(__file__).resolve().parents[2]))
        header = source / "NewStructure" / "FitCurveDrawing.h"
        if not header.is_file() or "AddMipFitLegendEntry" not in header.read_text():
            self.skipTest("requires production legend-lifetime fix (upstream PR #94); not validated")
        root_config = shutil.which("root-config")
        self.assertIsNotNone(root_config, "root-config is required once the production fix is present")
        flags = subprocess.check_output([root_config, "--cflags", "--libs"], text=True, timeout=30)
        compiler = shlex.split(os.environ.get("CXX", "c++"))
        with tempfile.TemporaryDirectory(prefix="lfhcal-legend-") as directory:
            executable = Path(directory) / "test_fit_curve_legend"
            command = compiler + [str(Path(__file__).with_name("test_fit_curve_legend.cc")),
                                  "-I" + str(source / "NewStructure"), "-o", str(executable)] + shlex.split(flags)
            subprocess.run(command, check=True, timeout=120)
            fixed = subprocess.run([str(executable)], capture_output=True, text=True, timeout=60)
            self.assertEqual(fixed.returncode, 0, fixed.stdout + fixed.stderr)
            self.assertIn("passed across 10 layers", fixed.stdout)
            legacy = subprocess.run([str(executable), "--legacy"], capture_output=True, text=True, timeout=60)
            self.assertEqual(legacy.returncode, 3, legacy.stdout + legacy.stderr)
            self.assertIn("legend retains a destroyed source TF1", legacy.stderr)


if __name__ == "__main__":
    unittest.main(verbosity=2)
