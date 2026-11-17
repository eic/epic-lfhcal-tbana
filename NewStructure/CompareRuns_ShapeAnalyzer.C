// Compare ShapeAnalyzer results across runs: mean cluster, cell, tower and event energies vs beam energy.
//
// Reads the per-run histogram files written by the Analyzer with -H (runShapeAnalyzer_*.sh puts them in
// <plotDir>/HistFiles/<mode>/ShapeAnalyzer_Run_<run>.root) and plots, for each quantity below, the histogram mean
// (+- error of the mean) vs the beam energy, one series per beam species, with a straight-line fit per species.
// A table of all values is written to <outDir>/MeanEnergies_<mode>.csv.
//
// Usage (or runCompareRuns_TBSPSH2_2026.sh):
//   root -l -b -q 'CompareRuns_ShapeAnalyzer.C("<plotDir>/HistFiles", "full", "444 445 446", "<outDir>", "pdf")'
//   mode: adc, full or tot_fit (the subdirectory of the histogram files); runs separated by spaces or commas.
//
// Means are taken from the histograms, i.e. within their axis range: values in the overflow are not included
// (a warning is printed if more than 1% of the entries are outside the range).

#include "TFile.h"
#include "TH1.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TParameter.h"
#include "TNamed.h"
#include "TObjArray.h"
#include "TObjString.h"
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>

namespace CompareRuns {
  struct Quantity{
    const char* dir;      // directory in the histogram file
    const char* key;      // histogram name in that directory
    const char* name;     // output plot name
    const char* yTitle;   // y-axis title (unit appended)
    const char* label;    // what was averaged, printed on the plot
  };
  // add further quantities here: any 1D histogram saved by ShapeAnalyzer can be used
  const std::vector<Quantity> kQuantities = {
    {"Inclusive", "EventDisplay_WindowEnergy",               "MeanClusterEnergy",        "#LT#it{E}_{cluster}#GT",               "all seeded clusters"},
    {"Inclusive", "EventDisplay_WindowEnergy_FirstCluster",  "MeanLeadClusterEnergy",    "#LT#it{E}_{1st cluster}#GT",           "leading seeded cluster"},
    {"Inclusive", "EventDisplay_WindowEnergy_Window",        "MeanClusterEnergy_Window", "#LT#it{E}_{cluster}#GT",               "all window clusters"},
    {"Inclusive", "EventDisplay_EventEnergy",                "MeanEventEnergy",          "#LT#it{E}_{event}#GT",                 "events with a cluster"},
    {"Inclusive", "EventDisplay_TowerEnergy",                "MeanTowerEnergy",          "#LT#it{E}_{tower}#GT",                 "towers in events with a cluster"},
    {"Inclusive", "EventDisplay_CellEnergy",                 "MeanCellEnergy",           "#LT#it{E}_{cell}/#it{N}_{layers}#GT",  "cells in events with a cluster"},
    {"Inclusive", "EventDisplay_SeedCellEnergy",             "MeanMaxCellEnergy",        "#LT#it{E}_{max cell}/#it{N}_{layers}#GT", "max. cell of seeded clusters"},
  };
  const Color_t kColors[]  = {kBlue+1, kRed+1, kGreen+2, kMagenta+1, kOrange+1, kCyan+2, kGray+2};
  const Style_t kMarkers[] = {20, 21, 33, 22, 23, 29, 34};

  struct RunData{
    int run = 0;
    double beamE = 0;
    std::string species, unit, type;
    std::vector<double> mean, err, overflow;   // per quantity (mean < 0: histogram missing)
  };

  void DrawText(double x, double y, const TString& text, double size = 0.035){
    TLatex l;
    l.SetNDC();
    l.SetTextFont(42);
    l.SetTextSize(size);
    l.DrawLatex(x, y, text);
  }
}

void CompareRuns_ShapeAnalyzer(TString histDir, TString mode, TString runList, TString outDir, TString suffix = "pdf"){
  using namespace CompareRuns;
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetEndErrorSize(0);

  // runs
  std::vector<TString> runs;
  TString list = runList;
  list.ReplaceAll(",", " ");
  TObjArray* tok = list.Tokenize(" ");
  for(int i = 0; i < tok->GetEntries(); i++) runs.push_back(((TObjString*)tok->At(i))->GetString());
  delete tok;
  if(runs.empty()){ std::cout << "no runs given" << std::endl; return; }

  // read means
  std::vector<RunData> data;
  for(const TString& r : runs){
    const TString fn = Form("%s/%s/ShapeAnalyzer_Run_%s.root", histDir.Data(), mode.Data(), r.Data());
    if(gSystem->AccessPathName(fn)){ std::cout << "WARNING: " << fn << " not found, run " << r << " skipped" << std::endl; continue; }
    TFile* f = TFile::Open(fn);
    if(!f || f->IsZombie()){ std::cout << "WARNING: cannot open " << fn << std::endl; continue; }
    RunData d;
    auto pRun = (TParameter<int>*)f->Get("runNumber");
    auto pE   = (TParameter<double>*)f->Get("beamEnergy");
    auto nSp  = (TNamed*)f->Get("species");
    auto nU   = (TNamed*)f->Get("energyUnit");
    auto nT   = (TNamed*)f->Get("energyType");
    if(!pE || !nSp){ std::cout << "WARNING: no run info in " << fn << ", skipped" << std::endl; f->Close(); continue; }
    d.run     = pRun ? pRun->GetVal() : r.Atoi();
    d.beamE   = pE->GetVal();
    d.species = nSp->GetTitle();
    d.unit    = nU ? nU->GetTitle() : "";
    d.type    = nT ? nT->GetTitle() : "";
    if(d.type.size() && d.type != mode.Data())
      std::cout << "WARNING: " << fn << " holds '" << d.type << "' energies, not '" << mode << "'" << std::endl;
    for(const Quantity& q : kQuantities){
      TH1* h = (TH1*)f->Get(Form("%s/%s", q.dir, q.key));
      if(!h || h->GetEntries() == 0){ d.mean.push_back(-1); d.err.push_back(0); d.overflow.push_back(0); continue; }
      const double inRange = h->Integral();
      const double out     = h->GetBinContent(0) + h->GetBinContent(h->GetNbinsX()+1);
      const double fracOut = (inRange+out) > 0 ? out/(inRange+out) : 0;
      if(fracOut > 0.01)
        std::cout << "WARNING: run " << d.run << ", " << q.key << ": " << 100*fracOut << "% of the entries outside the axis range, not in the mean" << std::endl;
      d.mean.push_back(h->GetMean());
      d.err.push_back(h->GetMeanError());
      d.overflow.push_back(fracOut);
    }
    std::cout << "run " << d.run << ": " << d.species << ", " << d.beamE << " GeV" << std::endl;
    data.push_back(d);
    f->Close();
  }
  if(data.empty()){ std::cout << "no histogram files found in " << histDir << "/" << mode << std::endl; return; }
  gSystem->mkdir(outDir, true);
  const std::string unit = data[0].unit;

  // species in order of appearance
  std::vector<std::string> species;
  for(const RunData& d : data)
    if(std::find(species.begin(), species.end(), d.species) == species.end()) species.push_back(d.species);

  // table
  std::ofstream csv(Form("%s/MeanEnergies_%s.csv", outDir.Data(), mode.Data()));
  csv << "run,species,beamEnergy_GeV";
  for(const Quantity& q : kQuantities) csv << "," << q.name << "," << q.name << "_err," << q.name << "_fracOutOfRange";
  csv << "\n";
  for(const RunData& d : data){
    csv << d.run << "," << d.species << "," << d.beamE;
    for(size_t i = 0; i < kQuantities.size(); i++) csv << "," << d.mean[i] << "," << d.err[i] << "," << d.overflow[i];
    csv << "\n";
  }
  csv.close();

  // one plot per quantity
  double eMax = 0;
  for(const RunData& d : data) eMax = std::max(eMax, d.beamE);
  for(size_t qi = 0; qi < kQuantities.size(); qi++){
    const Quantity& q = kQuantities[qi];
    std::vector<TGraphErrors*> graphs;
    std::vector<TF1*> fits;
    double yMax = 0;
    for(size_t s = 0; s < species.size(); s++){
      TGraphErrors* g = new TGraphErrors();
      for(const RunData& d : data){
        if(d.species != species[s] || d.mean[qi] < 0) continue;
        const int n = g->GetN();
        g->SetPoint(n, d.beamE, d.mean[qi]);
        g->SetPointError(n, 0, d.err[qi]);
        yMax = std::max(yMax, d.mean[qi] + d.err[qi]);
      }
      g->SetMarkerStyle(kMarkers[s % 7]);
      g->SetMarkerSize(1.4);
      g->SetMarkerColor(kColors[s % 7]);
      g->SetLineColor(kColors[s % 7]);
      graphs.push_back(g);
      // straight-line fit if the species has at least two beam energies
      std::vector<double> es;
      for(int i = 0; i < g->GetN(); i++) es.push_back(g->GetX()[i]);
      std::sort(es.begin(), es.end());
      es.erase(std::unique(es.begin(), es.end()), es.end());
      TF1* f = nullptr;
      if(es.size() >= 2){
        f = new TF1(Form("fit_%s_%zu", q.name, s), "pol1", 0, 1.1*eMax);
        f->SetLineColor(kColors[s % 7]);
        f->SetLineStyle(2);
        f->SetLineWidth(2);
        g->Fit(f, "Q0");
      }
      fits.push_back(f);
    }
    if(yMax <= 0) { for(auto g : graphs) delete g; for(auto f : fits) delete f; continue; }

    TCanvas* c = new TCanvas(Form("c_%s", q.name), q.name, 800, 700);
    c->SetLeftMargin(0.15);
    c->SetBottomMargin(0.12);
    c->SetTopMargin(0.12);
    c->SetRightMargin(0.04);
    TH1* frame = c->DrawFrame(0, 0, 1.1*eMax, 1.45*yMax);
    frame->GetXaxis()->SetTitle("#it{E}_{beam} (GeV)");
    frame->GetYaxis()->SetTitle(Form("%s (%s)", q.yTitle, unit.c_str()));
    for(TAxis* ax : {frame->GetXaxis(), frame->GetYaxis()}){
      ax->SetTitleFont(42); ax->SetLabelFont(42);
      ax->SetTitleSize(0.045); ax->SetLabelSize(0.040);
    }
    frame->GetYaxis()->SetTitleOffset(1.6);
    frame->GetXaxis()->SetTitleOffset(1.15);

    TLegend* leg = new TLegend(0.19, 0.86-0.05*species.size(), 0.75, 0.86);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextFont(42);
    leg->SetTextSize(0.033);
    for(size_t s = 0; s < species.size(); s++){
      if(graphs[s]->GetN() == 0) continue;
      graphs[s]->Draw("P same");
      TString entry = species[s].c_str();
      if(fits[s]){
        fits[s]->Draw("same");
        entry += Form(": %.3g + %.3g #upoint #it{E}_{beam}", fits[s]->GetParameter(0), fits[s]->GetParameter(1));
      }
      leg->AddEntry(graphs[s], entry, "p");
    }
    leg->Draw();
    DrawText(0.15, 0.94, Form("#bf{LFHCal TB}, %s energies", mode.Data()));
    DrawText(0.15, 0.895, q.label, 0.032);
    c->SaveAs(Form("%s/%s.%s", outDir.Data(), q.name, suffix.Data()));

    delete leg;
    delete c;
    for(auto g : graphs) delete g;
    for(auto f : fits) delete f;
  }
  std::cout << "plots and MeanEnergies_" << mode << ".csv written to " << outDir << std::endl;
}
