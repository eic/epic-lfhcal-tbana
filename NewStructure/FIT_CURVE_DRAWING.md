# Faithful MIP fit plots

`FitCurveDrawing.h` samples the live fitted TF1 callback into straight graph
segments without changing its parameters, evaluation settings or original range.
The existing drawing extent, including extrapolation, is preserved without
changing the fit range. The graph keeps the
existing line color, style and width and is owned by the current pad.

Existing fit selection, legends and component overlays are unchanged. Those
legacy component overlays are not asserted to be an exact decomposition of every
combined or adaptive callback. No new markers or annotations are added.

The existing uniform sampler is still limited to 200,000 intervals. This can
omit valid narrow-width fits, especially over the restored drawing extent
(for example, width 0.02 over [70,650] needs 464,000 intervals). A warning is
logged if that budget is exceeded or an invalid value is encountered; no
annotation or silently coarsened curve is drawn. Resolving this sampling limit
remains separate work. Fitting and calibration are unchanged.

Regenerate plots with live fits. A streamed callback TF1 may retain only an
interpolation table; dense sampling cannot recover its original evaluator.
Works with legacy and adaptive callbacks; it does not repair the integration.
