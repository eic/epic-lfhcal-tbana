#include "FitCurveDrawing.h"
#include <TCanvas.h>
#include <TROOT.h>
#include <iostream>
#include <memory>
#include <string>

// --legacy is a manual negative control: reproduces the pre-fix dangling
// source-model legend entry and fails before attempting its unsafe repaint.
int main(int argc, char** argv) {
  gROOT->SetBatch(true);
  const bool legacy = argc > 1 && std::string(argv[1]) == "--legacy";
  TCanvas canvas("legend_lifetime", "", 600, 400);
  for (int layer = 0; layer < 10; ++layer) {
    canvas.Clear();
    canvas.cd();
    auto* legend = new TLegend(.5, .5, .9, .9);
    legend->SetBit(TObject::kCanDelete);
    TLegendEntry* entry = nullptr;
    {
      TF1 fit("temporary_layer_fit", "[0]+[1]*x+[2]+[3]", 0., 20.);
      fit.SetParameters(2., 3., 4., 5.);
      fit.SetLineColor(kRed+3); fit.SetLineStyle(7); fit.SetLineWidth(3);
      fit.SetNpx(23);
      auto* curve = lfhcal::DrawMipFit(fit, 0., 20.);
      if (!curve || curve->GetN() < 1000 || fit.GetNpx() != 23) return 1;
      for (int i = 0; i < curve->GetN(); ++i) {
        double x, y; curve->GetPoint(i, x, y);
        if (y != fit.Eval(x)) return 2;
      }
      entry = legacy ? legend->AddEntry(&fit, "Landau-Gauss fit", "l")
                     : lfhcal::AddMipFitLegendEntry(*legend, fit, "Landau-Gauss fit");
      legend->Draw();
      canvas.Modified(); canvas.Update();
    } // The source TF1 is gone, exactly as after a by-value layer helper.
    if (entry->GetObject() != nullptr) {
      std::cerr << "FAIL: legend retains a destroyed source TF1\n";
      return 3;
    }
    if (entry->GetLineColor() != kRed+3 || entry->GetLineStyle() != 7 ||
        entry->GetLineWidth() != 3 || std::string(entry->GetLabel()) != "Landau-Gauss fit") return 4;
    canvas.Modified(); canvas.Update(); // Former next-layer redraw crash.
  }
  canvas.Clear();
  std::cout << "Fit-legend lifetime, style and live-curve values passed across 10 layers\n";
  return 0;
}
