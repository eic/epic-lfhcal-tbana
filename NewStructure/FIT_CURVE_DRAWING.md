# Faithful MIP fit plots

`FitCurveDrawing.h` samples the live fitted TF1 callback into straight graph
segments. It never substitutes a different convolution or smooths the graph.
The legacy 100-step integration can itself produce narrow ripples; the old
0–2000 ADC drawing range and 1000 display samples concealed or aliased them.

The four MIP plot helpers now draw only the visible original fit window, with
no extrapolation. Open markers show predictions at histogram bin centers:
current HG/LG MIP fits omit ROOT option `I`. Bin-integrated fits would require
separate bin-average prediction logic before using this helper.

Parameters, fit range, optimizer, fit acceptance and calibration are unchanged.
Rejected last-constructed attempts are labeled separately; they are not used
for calibration. Invalid or excessively fine curves report a visible warning.
Combined seven-parameter fits use the complete fitted callback, including its
pedestal and penalty behavior, rather than reconstructed component overlays.

Rebuild DataPrep and regenerate plots with live fits. Existing images do not
change. A streamed callback TF1 may contain only an interpolation table;
dense sampling cannot recover its original evaluator. Reconstruct the exact
original callback before drawing saved parameters, never a replacement model.

Rebased on upstream `8c9de5ee`, including the merged changes from PRs #86/#87.
Works with live legacy or adaptive callbacks; it does not require PR #89 or
change the fit or the integration method itself.
Plotting regression/comparison routines are maintained in a separate dependent
PR, not the baseline smoke suite. Release DataPrep and ROOT 6.40 plotting checks
passed. [Preserved sample evidence](https://github.com/paulnord/epic-lfhcal-tbana/tree/3b73f2e4ece1e7ee9d7a5e12b97b2a566743d928/NewStructure/tests/fit-curve-evidence)
includes synthetic same-histogram examples and a historical rounded-parameter
plotting audit. Neither is an exact current full-chain beam replay.
