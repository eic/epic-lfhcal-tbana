# LFHCal baseline smoke tests

A quick, explicit check that the **already-built** LFHCal stack starts and
performs a few known operations correctly. The same pattern can support small
checks of other components as the software develops. Detailed numerical and
scientific regressions complement these checks with broader validation of
accuracy, detector behavior and analysis results.

## One command after compilation

Run in the same ROOT/EIC environment used for the build, from the repository root:

```sh
python3 tests/smoke/run.py --build-dir NewStructure/build
```

For an in-source CMake build, use `--build-dir NewStructure`. No extra CMake option
is needed. Each command has a 30-second limit; `--timeout 60` changes that limit.
Any required missing artifact, failing command, timeout, or wrong numerical/data
result returns a nonzero exit status. All fixture output is temporary and removed
on success or failure. The suite does not silently skip a required check.

### Checks in the baseline

1. The explicitly selected `Convert` and `DataPrep` executables exist and actually
   start with `-h`, return zero, and print their usage text. This exercises startup
   and shared-library discovery, not a detector conversion.
2. `root-config --version` runs. ROOT then explicitly loads the selected
   `libLFHCAL` with its generated dictionary artifacts. No ROOT startup macros
   or working-directory autoload are relied upon.
3. The real `Calib` implementation calculates a known low-gain scale:
   high-gain scale 120 divided by correlation slope 4 gives 30, with absolute
   tolerance `1e-12`. This is a tiny deterministic numerical control, not a MIP fit.
4. ROOT writes a real `Calib` object and a one-entry `TTree` containing a real
   `Tile`. A second ROOT process reopens the file without recovery, checks the
   object names, entry count and values, and repeats the calibration calculation.

The inputs are generated in code: one channel, one event, no random numbers and
no beam-data dependency. `driver.C` propagates interpreter and payload failures
as nonzero status; the Python runner also requires an explicit success marker.
Checks do not use C++ `assert`, so release builds cannot disable them.

## Philosophy for component smoke tests

A useful smoke test answers a small, concrete question: can this component
perform a representative operation through its real public interface?

- **Use a tiny deterministic fixture.** Generate a few known values or commit a
  small, documented input. Keep expected results understandable from the input;
  prefer directly specified values to random generation and require no remote data.
- **Exercise the real component.** Call the production executable or public
  class interface. Mocks belong in tests of runner mechanics, not in evidence
  that the component itself works.
- **Check an explicit result.** Choose one or a few meaningful outputs: a count,
  an ID, a calculated value or a round trip. Compare discrete values exactly;
  justify numerical tolerances from the calculation and precision involved.
  The current `120 / 4 = 30` calibration control is one example.
- **Make failures actionable.** Name the stage and report the failed expectation
  with useful observed values. Checks must remain active in Release builds and
  propagate a nonzero exit status for wrong results, missing dependencies,
  interpreter errors and timeouts. A printed error alone is insufficient.
- **Keep execution bounded and isolated.** Declare required build artifacts,
  use short per-command timeouts and temporary output directories, and clean up
  on success or failure. Use a fresh process when testing disk state or startup
  independently of retained in-memory objects. Running smoke tests should not
  install software, rebuild, download data or submit jobs.
- **Keep stages identifiable.** Startup, decoding, calibration and serialization
  checks should report their own outcomes. A startup pass establishes readiness
  for the next stage; a conversion check must inspect converted results.

For example, a **future HGCROC conversion smoke case** could use a tiny complete
capture with documented provenance, invoke the real conversion interface, then
verify a known event count, event IDs and selected ADC values. This is a proposed
extension, not a case shipped by this baseline. Broader stream coverage and
scientific validation would remain in the detailed regression suite.

## How to add a small check

Today this is **one explicitly wired baseline**, with no automatic macro or
case discovery. `run.py` names `Convert`, `DataPrep`, the library artifacts and
`payload.C`, then runs the fixed `write` and `read` modes. `driver.C` loads the
selected library and payload and calls the fixed `lfhcal_smoke_payload()` entry
point. Adding a `.C` file to this directory does not make it run.

1. State the component operation, minimal input, expected result and tolerance
   before writing the check. Include the required artifacts and runtime budget.
2. For a small check sharing the current calibration/serialization fixture, add
   a readable helper in `payload.C` and call it from the appropriate mode.
   Use release-active checks such as `smoke_require` and preserve failure
   propagation through the driver.
3. For an independent component, add a clearly named case and explicitly wire
   its artifacts, invocation, timeout and success marker into `run.py`. A ROOT
   payload must follow the driver's fixed entry-point contract, or the driver
   dispatch must be deliberately extended. This requires code changes; there
   is no existing generic case registry to populate.
4. Update `test_runner.py` for any new command sequence or failure path, then
   run both the runner tests and the real component check. Fake-command tests
   establish orchestration behavior; the real run establishes component behavior.
5. Document a copyable run command, prerequisites and expected result here.
   Ensure the case is invoked by the existing runner or explicitly register its
   command in `.github/workflows/root-smoke.yml`; check that the workflow's path
   filters cover its files. Verify the real check in CI as well as locally.

Keep the next addition small enough to understand and diagnose on its own.
A shared fixture is useful when it keeps checks simple; a new test framework is
not a prerequisite for adding another component check.

## Optional Yall readiness, not a Yall test campaign

If Yall is relevant to this environment, explicitly request only its version:

```sh
python3 tests/smoke/run.py --build-dir NewStructure/build --yall /path/to/yall-run
```

This does **not** validate a Yallfile, create/start a campaign, or contact Condor.
Without `--yall`, the report says Yall readiness was not requested. A shell
function is not an executable: if EIC setup exposes only the `yall-run` function,
run `yall-run --version` separately in that activated shell. The runner does not
source environment files or install another copy of Yall.

## Build-tree versus installed artifacts

The current upstream CMake project has no `install()` rules. Its bootstrap builds
in `NewStructure/build`; that is a post-build check, not proof of a relocatable
installation. This suite does not introduce or claim a new install layout.

For an existing separately deployed layout, point at the actual artifacts rather
than falling back to build-tree binaries:

```sh
python3 tests/smoke/run.py --bin-dir /deployment/bin \
  --library /deployment/lib/libLFHCAL.so
```

The library's directory must also contain nonempty `libLFHCAL_rdict.pcm` and
`libLFHCAL.rootmap`. Use the test checkout matching those artifacts, because the
ROOT payload includes its `NewStructure` class headers. This is not a standalone
binary-only package or a test that build-tree headers are absent. The runner
prints the selected binary/library/ROOT paths and does not change
`LD_LIBRARY_PATH`. Within ROOT it adds the selected library directory to the
dictionary autoload path. Activate the intended runtime environment first. `--root` and
`--root-config` can select exact matching ROOT executables when needed.

## Provenance and boundaries

The readiness checks are consolidated from the actual upstream
`tools/bootstrap-yall-integration.sh`: ROOT version, executable presence and
optional Yall availability. Here executable presence is strengthened to startup.
The write/read pattern follows the existing `NewStructure/Tests/dummy.cc` and
`dummyRead.cc` examples, but uses **current production classes**, checks the
results, and does not reuse their committed binaries or old `Dummy.root` file.
The numerical and checked production-object fixture are new coverage.

The complementary `tools/tests` suite exercises bootstrap/environment scripts;
`examples/yall/check_shared_conversions.py` checks campaign graphs. The baseline
runs explicitly after compilation or through CI; bootstrap and installation
behavior are unchanged.

## Maintainer checks of the runner

```sh
python3 -m unittest discover -s tests/smoke -p 'test_runner.py' -v
python3 -m unittest discover -s tools/tests -v
```

The first command uses fake executables to test failure propagation, timeouts,
path quoting, temporary-directory cleanup, optional-Yall behavior and the validation
helper's error-detection guards (17 tests).
Passing it is **not** evidence that the production ROOT smoke checks passed. Run the real
one-command check in a compiled LFHCal environment as well.

### Optional error-detection validation

Run the normal checks and then deliberately replace ROOT with `/bin/false` to
verify that a ROOT write failure is detected:

```bash
tests/smoke/validate.sh --build-dir NewStructure/build --timeout 60
```

The helper labels the normal and deliberately failing phases clearly. It captures
only the injected run and reports `PASS: expected error detected` only when the
runner exits 1 with the exact expected ROOT-write rejection. A genuine baseline
failure stops before injection and remains visible; unexpected success, another
failure stage, a different exit status, or a timeout prints the injected log and
returns nonzero. This offline injection checks the runner's error detection, not
the behavior of real ROOT.

## GitHub Actions

`ROOT smoke tests` builds `Convert` and `DataPrep` and runs the runner unit tests
and real ROOT smoke checks on GitHub-hosted Ubuntu 24.04. It uses the official
`rootproject/root:6.34.00-ubuntu24.04` image pinned by digest, with read-only
repository permissions and the decoder revision recorded in the submodule.
It runs for relevant pull-request changes, pushes to main, or manual dispatch.
The workflow runs the explicit baseline command above. Contributors adding a
case should keep its invocation and path coverage visible in that workflow.
The separate `Yall examples` workflow checks workflow graphs and environment
scripts; its result complements the ROOT build and smoke result.
