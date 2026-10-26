#ifndef LFHCAL_FIT_CURVE_DRAWING_H
#define LFHCAL_FIT_CURVE_DRAWING_H

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>
#include <TF1.h>
#include <TGraph.h>
#include <TH1.h>
#include <TLatex.h>
#include <TError.h>

namespace lfhcal {

// Sample the LIVE fitted evaluator. Never Clone()/stream a callback TF1 here:
// a reloaded callback may only evaluate ROOT's saved interpolation table.
// The four Langau parameters are width, MP, area, Gaussian sigma.
inline TGraph SampleMipFit(const TF1& fit, double visibleLow, double visibleHigh) {
  const double lo = std::max(visibleLow, fit.GetXmin());
  const double hi = std::min(visibleHigh, fit.GetXmax());
  TGraph graph;
  graph.SetName((std::string(fit.GetName()) + "_evaluated_curve").c_str());
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
  // Refuse to draw a falsely smooth line if a corrupt/extreme fit needs more.
  if (!std::isfinite(intervals) || intervals > 200000.)
    throw std::runtime_error("fit drawing needs more than 200000 intervals");
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

// DrawClone is safe for the already evaluated TGraph and gives the pad its own
// points. The source TF1, fit range, parameters, Npx, and calibration stay intact.
inline TGraph* DrawMipFit(const TF1& fit, const TH1& data, double lo, double hi,
                         Color_t color, bool binPredictions=true) {
  try {
    TGraph curve = SampleMipFit(fit,lo,hi);
    if (curve.GetN() == 0) return nullptr;
    curve.SetLineColor(color);
    curve.SetLineWidth(1);
    curve.SetLineStyle(1);
    auto* drawn = static_cast<TGraph*>(curve.DrawClone("L SAME"));
    if (binPredictions) {
      TGraph bins;
      bins.SetName((std::string(fit.GetName()) + "_bin_predictions").c_str());
      for (int i=1; i<=data.GetNbinsX(); ++i) {
        const double x=data.GetBinCenter(i);
        if (x < std::max(lo,fit.GetXmin()) || x > std::min(hi,fit.GetXmax())) continue;
        const double y=fit.Eval(x);
        if (!std::isfinite(y)) throw std::runtime_error("nonfinite bin prediction");
        bins.SetPoint(bins.GetN(),x,y);
      }
      // The production QRL[NM]0 fits do not use ROOT's bin-integral option I.
      bins.SetMarkerColor(color);
      bins.SetMarkerStyle(24);
      bins.SetMarkerSize(.35);
      bins.DrawClone("P SAME");
    }
    return drawn;
  } catch (const std::exception& e) {
    Warning("DrawMipFit", "%s: %s", fit.GetName(), e.what());
    TLatex text;
    text.SetNDC(); text.SetTextSize(.035); text.SetTextColor(color);
    text.DrawLatex(.13,.72,"Fit curve could not be resolved (see log)");
    return nullptr;
  }
}

} // namespace lfhcal
#endif
