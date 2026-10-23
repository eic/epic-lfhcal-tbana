# Reviewable adaptive Landau–Gaussian integration

This is an isolated numerical revision. It ports the existing `AdaptiveLangau.h`
unchanged from
`paulnord/epic-lfhcal-tbana` commit
`c986e5a5f2e010ad4e778f307216d4dad9e3e0a0` and wires it into current upstream
`eic/epic-lfhcal-tbana` base `8c9de5ee5b85c30c3a266dd22dd12443284054ef`.
The earlier helper was extracted from `LangauNumerics.h` at `3a643646`.

## Upstream prerequisites

[PR #86](https://github.com/eic/epic-lfhcal-tbana/pull/86) and
[PR #87](https://github.com/eic/epic-lfhcal-tbana/pull/87) merged on October 23,
2026 and are included in the upstream base above. This branch contains only the
adaptive integration changes relative to that base. Buffer-lifetime cleanup and
checked output handling belong to those merged prerequisites, not this diff.

## Switch and intended adoption

The review patch defaults to **legacy**. Build two independent directories to
compare identical inputs without rebuilding or overwriting a running campaign:

```sh
git submodule update --init --recursive
cmake -S NewStructure -B build-legacy -DCMAKE_BUILD_TYPE=Release \
  -DLFHCAL_USE_ADAPTIVE_LANGAU=OFF
cmake --build build-legacy --target DataPrep Convert -j 4
cmake -S NewStructure -B build-adaptive -DCMAKE_BUILD_TYPE=Release \
  -DLFHCAL_USE_ADAPTIVE_LANGAU=ON
cmake --build build-adaptive --target DataPrep Convert -j 4
```

Run the same DataPrep command with the respective executable and separate output
paths. There is no new command-line argument. CMake reports the chosen mode;
adaptive HGCROC TF1s retain their names and get an explicit evaluator title.
The definition is PRIVATE to the LFHCAL library, so all three Analyses.cc callers
of FitMipHG use the same selection without mutable global state or extra API.

The intended endpoint is adoption as the default after review and real-data
comparison. That needs only `OFF` → `ON` in the CMake option declaration. Keep
an explicit `-DLFHCAL_USE_ADAPTIVE_LANGAU=OFF` for reproducibility. CMake caches
existing values, so changing the source default affects fresh build directories,
not an existing cache explicitly or implicitly set to OFF.

## Deliberately narrow scope

- HGCROC HG MIP fits alone select adaptive quadrature when enabled.
- Peak and FWHM use the same continuous adaptive curve as the fit, not the
  undersampled legacy callback. This extraction is part of the correction.
- CAEN HG, the newer CAEN pedestal model, LG, and the legacy callbacks stay intact.
- Four parameters, MPV shift, normalization, and ±5 Gaussian-sigma span stay the
  same. The kernel uses dimensionless Landau coordinates, explicit peak/tail
  subdivisions, a 16-point Gauss-Legendre rule, and relative tolerance `1e-10`.
- Fit windows, seeds, parameter limits, bins/errors, likelihood flags, optimizer,
  fit-status acceptance, bad-channel handling, and iteration count are unchanged.
- No FFT, extra Gaussian floor, rebinning, or fit-range correction is
  bundled here. Campaigns, plotting revisions, and unrelated bug fixes are excluded.
- No data schema or ROOT class version change, TileSpectra.h change, or new library
  dependency. Merged PR #86 supplies automatic fit-buffer lifetime, including the
  seven-parameter HG buffers; this adaptive change relies on it for safe exits.
- An adaptive numerical failure rejects that fit, clears its saved-fit flag, and
  leaves previous calibration and caller outputs untouched. It does not silently
  fall back to legacy. Unrelated CAEN exceptions keep their previous behavior.

`AdaptiveLangau.h` is the only new production header; the only existing production
files changed are TileSpectra.cc and CMakeLists.txt. A compiled TF1 callback copied
in the same process remains live; a streamed/reloaded TF1 is not promised to be a
live refittable callback. Separate faithful-plotting work is not included here.

Tests are maintained separately and are not included in this change. Before
adopting the adaptive default, compare frozen beam spectra and real calibration
chains with identical fit windows and iteration counts. Keep per-build commits,
CMake caches, toolchain versions, and separate output paths for reproducibility.
