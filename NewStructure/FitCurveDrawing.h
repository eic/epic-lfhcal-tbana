#ifndef LFHCAL_FIT_CURVE_DRAWING_H
#define LFHCAL_FIT_CURVE_DRAWING_H

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>
#include <TF1.h>
#include <TGraph.h>
#include <TError.h>
#include <TLegend.h>
#include <TLegendEntry.h>

namespace lfhcal {

// Sample the LIVE fitted evaluator. Never Clone()/stream a callback TF1 here:
// a reloaded callback may only evaluate ROOT's saved interpolation table.
// The four Langau parameters are width, MP, area, Gaussian sigma.
inline TGraph SampleMipFit(const TF1& fit, double visibleLow, double visibleHigh) {
  const double lo = visibleLow;
  const double hi = visibleHigh;
  TGraph graph;
  graph.SetName((std::string(fit.GetName()) + "_evaluated_curve").c_str());
  graph.SetLineColor(fit.GetLineColor());
  graph.SetLineStyle(fit.GetLineStyle());
  graph.SetLineWidth(fit.GetLineWidth());
  if (!(hi > lo)) return graph;
  if (fit.GetNpar() < 4 || !std::isfinite(fit.GetParameter(0)) ||
      !std::isfinite(fit.GetParameter(3)) || fit.GetParameter(0) <= 0. ||
      fit.GetParameter(3) <= 0.)
    throw std::runtime_error("invalid Langau widths in fit drawing");
  // Resolve both the Landau features and the legacy-100 quadrature ripple.
  // Its spacing is 0.1*sigma; when width is small these oscillations are real
  // features of the objective, so ROOT's default Npx is not a safe guide.
  double step = std::min((hi-lo)/1000., fit.GetParameter(0)/16.);
  // The optional seven-parameter fit also contains a narrow pedestal.
  if (fit.GetNpar() == 7 && std::isfinite(fit.GetParameter(6)) &&
      fit.GetParameter(6) > 0.)
    step = std::min(step, fit.GetParameter(6)/16.);
  if (fit.GetParameter(0) < .2*fit.GetParameter(3))
    step = std::min(step, fit.GetParameter(3)/320.);
  const double intervals = std::ceil((hi-lo)/step);
  // Bound rendering cost, never silently coarsen a narrow fitted function.
  // This existing budget can reject valid narrow-width fits; see FIT_CURVE_DRAWING.md.
  if (!std::isfinite(intervals) || intervals > 200000.)
    throw std::runtime_error("fit drawing exceeds 200000-interval rendering budget");
  const int n = static_cast<int>(intervals);
  graph.Set(n+1);
  for (int i=0; i<=n; ++i) {
    const double x = lo + (hi-lo)*i/n;
    const double y = fit.Eval(x);
    if (!std::isfinite(y)) throw std::runtime_error("nonfinite fit drawing value");
    graph.SetPoint(i,x,y);
  }
  return graph;
}

// Layer helpers receive TileSpectra maps by value: their fitted TF1 members
// disappear when the layer helper returns, while its pad/legend can be repainted.
// The sampled graph does not draw/register the source TF1 for ROOT cleanup.
// Keep only the line appearance in the legend, never a borrowed model pointer.
inline TLegendEntry* AddMipFitLegendEntry(TLegend& legend, const TF1& fit,
                                         const char* label) {
  auto* entry = legend.AddEntry(static_cast<TObject*>(nullptr), label, "l");
  entry->SetLineColor(fit.GetLineColor());
  entry->SetLineStyle(fit.GetLineStyle());
  entry->SetLineWidth(fit.GetLineWidth());
  return entry;
}

// The evaluated copy belongs to the current pad and is deleted on Clear().
// DrawClone would instead use the selected pad and does not give it ownership.
// The fitted TF1, its evaluation settings and its original range stay intact.
inline TGraph* DrawMipFit(const TF1& fit, double lo, double hi) {
  try {
    auto* drawn = new TGraph(SampleMipFit(fit,lo,hi));
    if (drawn->GetN() == 0) { delete drawn; return nullptr; }
    drawn->SetBit(TObject::kCanDelete);
    drawn->Draw("L SAME");
    return drawn;
  } catch (const std::exception& e) {
    Warning("DrawMipFit", "%s: %s", fit.GetName(), e.what());
    return nullptr;
  }
}

} // namespace lfhcal
#endif
