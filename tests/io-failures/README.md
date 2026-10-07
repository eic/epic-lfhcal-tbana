# Output failure regression test

Run with Python 3, a C++ compiler, and ROOT (including rootcling):

```sh
python3 tests/io-failures/run_fixture.py \
    --source NewStructure --root "$(root-config --prefix)" --out /tmp/lfhcal-io-tests
```

The fixture compiles the real Convert and DataPrep entry points and Calib
implementation. It replaces Analyses::CheckAndOpenIO and Analyses::Process with
controlled failures; it does not run the decoder, detector event loop, or fits.
ROOT handles quota and short-write errors injected through TFile::SysWrite.

Each executable must return 1 for an input-check failure, a false Process result,
quota/short/final-close ROOT write errors, calibration text open/write failures,
and a failed PNG SaveAs. Healthy output, ordinary warnings, and fit-only error
diagnostics must return 0. Detailed logs and results.json are saved under --out.
The /dev/full and EDQUOT cases require Linux.

Tested with GCC 13.3 and ROOT 6.40.00: all 22 patched cases passed. The modified
Analyses.cc also passed a syntax-only compile. This is I/O and exit-status
validation, not an end-to-end calibration test.

Optional --baseline DIR compares earlier versions of DataPrep.cc, Convert.cc,
Analyses.h, and Calib.cc; unchanged dependencies come from --source. Against
upstream 92df3e48, DataPrep already rejects Process failures and ROOT write bits,
but returns success for failed text/PNG output. Convert also returns success
for Process failures, quota errors, and short writes. Both baseline executables
abort when the fixture defers a write failure until final close.
