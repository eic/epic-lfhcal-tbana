# Faithful MIP fit plots

`FitCurveDrawing.h` samples the live fitted TF1 callback without replacing its
convolution or smoothing its numerical wiggles. It preserves the parameters,
original fit range and evaluation settings. Fitting and calibration are unchanged.

The four MIP plot helpers draw within the original fit window. Open markers
show predictions at histogram bin centers, matching the current fits without
ROOT option `I`; bin-integrated fits need separate bin-average logic. Combined
seven-parameter fits use the complete callback, including pedestal and penalty
behavior, rather than reconstructed components. Unresolvable curves show a warning.

Rebuild DataPrep and regenerate plots with live fits. A streamed callback TF1
may retain only an interpolation table; reconstruct its exact original evaluator
before drawing saved parameters. Dense sampling cannot recover a lost callback.

Works with legacy and adaptive callbacks. This reveals integration distortions;
it does not fix the integration or require the adaptive-integration PR.
