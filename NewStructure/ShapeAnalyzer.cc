#include "ShapeAnalyzer.h"
#include <vector>
#include "TROOT.h"
//#include <unistd.h> // Add for use on Mac OS -> Same goes for Analyse.cc
#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3F.h"
#include "TPolyLine3D.h"
#include "TPaletteAxis.h"
#include "TProfile.h"
#include "TChain.h"
#include "CommonHelperFunctions.h"
#include "TileSpectra.h"
#include "PlotHelper.h"
#include "MultiCanvas.h"
#include "TRandom3.h"
#include "TMath.h"
#include "Math/Minimizer.h"
#include "Math/MinimizerOptions.h"
#include "TLegend.h"
#include "TLatex.h"
#include <chrono>
#include <algorithm>

namespace {
  // upper edge of all cluster-energy axes
  const double kMaxClusterE = 1000.;
  // clusters with fewer cells per hit layer than this are classified as muon-like
  const double kMuonCellsPerLayer = 1.5;

  inline bool IsMuonLikeCluster(const Cluster& cl){ return cl.hitTowers < kMuonCellsPerLayer; }

  // In muon runs a real track leaves cells in many z-bins of one tower. Clusters whose deepest tower has
  // fewer hit z-bins than this are treated as background, and all their cells as background cells.
  const int kMinTrackTowerSizeZ = 5;
  inline bool IsBackgroundCluster(const Cluster& cl){ return cl.maxTowerSizeZ < kMinTrackTowerSizeZ; }

  // ---------------------------------------------------------------------------------------------
  // Common style for every plot produced by ShapeAnalyzer
  // ---------------------------------------------------------------------------------------------
  namespace PlotStyle {
    const Font_t  kFont       = 42;
    const double  kTitleSize  = 0.045;   // axis titles
    const double  kLabelSize  = 0.040;   // axis labels
    const double  kTextSize   = 0.035;   // header, panel labels, legends
    const double  kLineDist   = 0.045;   // spacing of the two header lines
    const int     kCanvasW    = 800;     // single-panel canvas
    const int     kCanvasH    = 700;
    const int     kPanelW     = 700;     // width of one panel in side-by-side canvases
    const double  kMarginL    = 0.14;
    const double  kMarginB    = 0.12;
    const double  kMarginT    = 0.12;    // room for the two header lines
    const double  kMarginR1D  = 0.04;
    const double  kMarginR2D  = 0.19;    // room for the z palette and its title
    const int     kNSamples   = 3;                           // max. samples overlaid on one plot
    const Color_t kColor[kNSamples]  = {kBlue+1, kRed+1, kGreen+2};   // first / second / third sample
    const Style_t kMarker[kNSamples] = {20, 24, 21};

    inline double RightMargin(bool is2D){ return is2D ? kMarginR2D : kMarginR1D; }

    inline void ApplyGlobal(){
      gStyle->SetOptStat(0);
      gStyle->SetOptTitle(0);
      //gStyle->SetPalette(kViridis);
      gStyle->SetNumberContours(255);
      gStyle->SetPadTickX(1);
      gStyle->SetPadTickY(1);
      gStyle->SetEndErrorSize(0);
    }

    inline void StylePad(TVirtualPad* pad, bool is2D){
      pad->SetFillColor(0);
      pad->SetBorderMode(0);
      pad->SetLeftMargin(kMarginL);
      pad->SetBottomMargin(kMarginB);
      pad->SetTopMargin(kMarginT);
      pad->SetRightMargin(RightMargin(is2D));
      pad->SetTickx(1);
      pad->SetTicky(1);
    }

    inline TCanvas* MakeCanvas(const TString& name, bool is2D){
      TCanvas* c = new TCanvas(name, name, kCanvasW, kCanvasH);
      StylePad(c, is2D);
      return c;
    }

    inline void StyleAxes(TH1* h){
      h->SetStats(0);
      for(TAxis* ax : {h->GetXaxis(), h->GetYaxis(), h->GetZaxis()}){
        ax->SetTitleFont(kFont);
        ax->SetLabelFont(kFont);
        ax->SetTitleSize(kTitleSize);
        ax->SetLabelSize(kLabelSize);
      }
      h->GetXaxis()->SetTitleOffset(1.15);
      h->GetYaxis()->SetTitleOffset(1.45);
      h->GetZaxis()->SetTitleOffset(1.25);
      h->GetXaxis()->SetNdivisions(505);
    }

    // sample index 0 .. kNSamples-1
    inline void StyleMarkers(TH1* h, int i){
      i = i % kNSamples;
      h->SetMarkerStyle(kMarker[i]);
      h->SetMarkerSize(1.0);
      h->SetMarkerColor(kColor[i]);
      h->SetLineColor(kColor[i]);
      h->SetLineWidth(1);
    }

    // largest bin content and smallest positive bin content, ignoring any user-set range
    inline void ContentRange(const std::vector<TH1*>& hs, double& minPos, double& max){
      max = 0; minPos = 0;
      for(TH1* h : hs){
        max = std::max(max, h->GetBinContent(h->GetMaximumBin()));
        double m = h->GetMinimum(0.);
        if(m > 0 && (minPos == 0 || m < minPos)) minPos = m;
      }
      if(max <= 0) max = 1;
      if(minPos <= 0 || minPos > max) minPos = max*1e-3;
    }

    // common y-range for 1D histograms drawn on the same pad, with extra headroom when a legend is drawn
    inline void SetYRange(const std::vector<TH1*>& hs, bool logY, bool withLegend){
      double minPos, max;
      ContentRange(hs, minPos, max);
      for(TH1* h : hs){
        h->SetMinimum(logY ? std::max(0.5*minPos, max*1e-5) : 0.);
        h->SetMaximum(logY ? max*(withLegend ? 30. : 3.) : max*(withLegend ? 1.35 : 1.15));
      }
    }

    // common log z-range for 2D histograms
    inline void SetZRange(const std::vector<TH1*>& hs){
      double minPos, max;
      ContentRange(hs, minPos, max);
      for(TH1* h : hs){
        h->SetMinimum(std::max(0.8*minPos, max*1e-5));
        h->SetMaximum(max*1.5);
      }
    }

    // run header (line 1) and optional label (line 2) in the top margin
    inline void DrawHeader(const TString& header, const TString& label){
      TString text = header;
      if(label.Length()) text += ";" + label;
      if(text.Length()) DrawLatex(kMarginL, 1.-kMarginT+0.065, text, false, kTextSize, kFont, kLineDist);
    }

    inline TLegend* MakeLegend(int nEntries, bool is2D){
      const double x2 = 1. - RightMargin(is2D) - 0.03;
      const double y2 = 1. - kMarginT - 0.03;
      TLegend* l = new TLegend(x2-0.48, y2-0.055*nEntries, x2, y2);
      l->SetBorderSize(0);
      l->SetFillStyle(0);
      l->SetTextFont(kFont);
      l->SetTextSize(kTextSize);
      return l;
    }

    inline const char* SampleDir(int sample){
      switch(sample){
        case 1:  return "Muon";
        case 2:  return "nonMuon";
        default: return "Inclusive";
      }
    }
  }

  // One set of cluster histograms. Booked once per sample (all, muon-like, non-muon-like, window) so that
  // every sample uses exactly the same observables, binning and axis titles.
  struct ClusterHists{
    TString tag;
    TString label;   // printed on the plots of this sample
    // 1D
    TH1D* energy;
    TH1D* firstZ;
    TH1D* lastZ;
    TH1D* maxZ;
    TH1D* sizeZ;
    TH1D* cellsPerLayer;
    TH1D* towerImbalance;
    TH1D* cellImbalance;
    TH1D* zImbalance;
    TH1D* seedCellE;
    TH1D* seedTowerE;
    // position maps
    TH2D* xy;
    TH2D* subXY;
    TH2D* maxCellXY;
    // correlations with cluster energy
    TH2D* e_towerImbalance;
    TH2D* e_seedCellE;
    TH2D* e_firstZ;
    TH2D* e_lastZ;
    TH2D* e_maxZ;
    TH2D* e_sizeZ;
    TH2D* e_cellsPerLayer;

    // eUnit: unit of the cell values (e.g. "mip eq.")
    ClusterHists(TString t, TString lab, TString eUnit, int nx, int ny, int nz, int nLayers) : tag(t), label(lab) {
      auto n = [&](const char* base){ return TString(base)+tag; };
      const TString tE      = Form("#it{E}_{cluster} (%s)", eUnit.Data());
      const TString tFirstZ = "first layer of cluster";
      const TString tLastZ  = "last layer of cluster";
      const TString tMaxZ   = "layer with max. energy";
      const TString tSizeZ  = "#it{N}_{layers} in cluster";
      const TString tCells  = "cells per hit layer";
      const TString tTowImb = "#it{E}_{max tower}/#it{E}_{cluster}";
      const TString tSeedC  = Form("#it{E}_{max cell} (%s)", eUnit.Data());
      const TString tSeedT  = Form("#it{E}_{max tower} (%s)", eUnit.Data());
      auto t1 = [](const TString& x){ return TString(";")+x+";counts"; };
      auto tE2 = [&](const TString& y){ return TString(";")+tE+";"+y+";counts"; };

      energy           = new TH1D(n("Clusters_energy"),         t1(tE),      50, 0, kMaxClusterE);
      firstZ           = new TH1D(n("Clusters_firstZ"),         t1(tFirstZ), nz, 0.5, nz+0.5);
      lastZ            = new TH1D(n("Clusters_lastZ"),          t1(tLastZ),  nz, 0.5, nz+0.5);
      maxZ             = new TH1D(n("ClustersZ_max"),           t1(tMaxZ),   nz, 0.5, nz+0.5);
      sizeZ            = new TH1D(n("Clusters_sizeZ"),          t1(tSizeZ),  nLayers, 0.5, nLayers+0.5);
      cellsPerLayer    = new TH1D(n("Clusters_spreadX"),        t1(tCells),  12, -0.5, 11.5);
      towerImbalance   = new TH1D(n("Clusters_imbalance"),      t1(tTowImb), 50, 0, 1);
      cellImbalance    = new TH1D(n("Clusters_cell_imbalance"), t1("#it{E}_{max cell}/#it{E}_{cluster}"),  50, 0, 1);
      zImbalance       = new TH1D(n("Clusters_z_imbalance"),    t1("#it{E}_{max layer}/#it{E}_{cluster}"), 50, 0, 1);
      seedCellE        = new TH1D(n("SeedCells_energy"),        t1(tSeedC),  100, 0, 300);
      seedTowerE       = new TH1D(n("SeedTowers_energy"),       t1(tSeedT),  100, 0, 300);

      xy               = new TH2D(n("ClustersXY"),    ";#it{x}_{cluster} (tower);#it{y}_{cluster} (tower);counts",
                                  nx*10, 0.5, nx+0.5, ny*10, 0.5, ny+0.5);
      subXY            = new TH2D(n("SubClustersXY"), ";#it{x}_{cluster} - #it{x}_{max cell} (tower);#it{y}_{cluster} - #it{y}_{max cell} (tower);counts",
                                  100, -3, 3, 100, -3, 3);
      maxCellXY        = new TH2D(n("MaxCellXY"),     ";#it{x}_{max cell} (tower);#it{y}_{max cell} (tower);counts",
                                  nx, 0.5, nx+0.5, ny, 0.5, ny+0.5);

      e_towerImbalance = new TH2D(n("Clusters_Energy_Imbalance"),       tE2(tTowImb), 50, 0, kMaxClusterE, 50, 0, 1);
      e_seedCellE      = new TH2D(n("Clusters_Energy_SeedCellsEnergy"), tE2(tSeedC),  50, 0, kMaxClusterE, 100, 0, 300);
      e_firstZ         = new TH2D(n("Clusters_Energy_FirstZ"),          tE2(tFirstZ), 50, 0, kMaxClusterE, nz, 0.5, nz+0.5);
      e_lastZ          = new TH2D(n("Clusters_Energy_LastZ"),           tE2(tLastZ),  50, 0, kMaxClusterE, nz, 0.5, nz+0.5);
      e_maxZ           = new TH2D(n("Clusters_Energy_MaxZ"),            tE2(tMaxZ),   50, 0, kMaxClusterE, nz, 0.5, nz+0.5);
      e_sizeZ          = new TH2D(n("Clusters_Energy_SizeZ"),           tE2(tSizeZ),  50, 0, kMaxClusterE, nLayers, 0.5, nLayers+0.5);
      e_cellsPerLayer  = new TH2D(n("Clusters_Energy_SpreadX"),         tE2(tCells),  50, 0, kMaxClusterE, 12, -0.5, 11.5);
    }

    std::vector<TH2D*> EnergyCorrelations() const {
      return {e_towerImbalance, e_seedCellE, e_firstZ, e_lastZ, e_maxZ, e_sizeZ, e_cellsPerLayer};
    }

    void Fill(const Cluster& cl){
      const double E = cl.energy;
      energy->Fill(E);
      firstZ->Fill(cl.firstz);
      lastZ->Fill(cl.lastz);
      maxZ->Fill(cl.highestz);
      sizeZ->Fill(cl.sizez);
      cellsPerLayer->Fill(cl.hitTowers);
      towerImbalance->Fill(cl.highesttowerenergy/E);
      cellImbalance->Fill(cl.highestcellenergy/E);
      zImbalance->Fill(cl.highestzE/E);
      seedCellE->Fill(cl.highestcellenergy);
      seedTowerE->Fill(cl.highesttowerenergy);

      xy->Fill(cl.x, cl.y);
      subXY->Fill(cl.x-cl.highestcellx, cl.y-cl.highestcelly);
      maxCellXY->Fill(cl.highestcellx, cl.highestcelly);

      e_towerImbalance->Fill(E, cl.highesttowerenergy/E);
      e_seedCellE->Fill(E, cl.highestcellenergy);
      e_firstZ->Fill(E, cl.firstz);
      e_lastZ->Fill(E, cl.lastz);
      e_maxZ->Fill(E, cl.highestz);
      e_sizeZ->Fill(E, cl.sizez);
      e_cellsPerLayer->Fill(E, cl.hitTowers);
    }
  };
}

// ****************************************************************************
// Checking and opening input and output files
// ****************************************************************************
bool ShapeAnalyzer::CheckAndOpenIO(void){
  int matchingbranch;

  //Need to check first input to get the setup...I do not think it is necessary
  std::cout<<"Input name set to: '"<<RootInputName.Data() <<std::endl;  
  if(!RootInputName.IsNull()){
    //File exist?
    RootInput=new TFile(RootInputName.Data(),"READ");
    if(RootInput->IsZombie()){
      std::cout<<"Error opening '"<<RootInputName<<"', does the file exist?"<<std::endl;
      return false;
    }

    //Retrieve info, start with setup
    TsetupIn = (TTree*) RootInput->Get("Setup");
    if(!TsetupIn){
      std::cout<<"Could not retrieve the Setup tree, leaving"<<std::endl;
      return false;
    }
    setup=Setup::GetInstance();
    std::cout<<"Setup add "<<setup<<std::endl;
    //matchingbranch=TsetupIn->SetBranchAddress("setup",&setup);
    matchingbranch=TsetupIn->SetBranchAddress("setup",&rswptr);
    if(matchingbranch<0){
      std::cout<<"Error retrieving Setup info from the tree"<<std::endl;
      return false;
    }
    std::cout<<"Entries "<<TsetupIn->GetEntries()<<std::endl;
    TsetupIn->GetEntry(0);
    setup->Initialize(*rswptr);
    std::cout<<"Reading "<<RootInput->GetName()<<std::endl;
    std::cout<<"Setup Info "<<setup->IsInit()<<"  and  "<<setup->GetCellID(0,0)<<std::endl;
    //std::cout<<"Setup add now "<<setup<<std::endl;

    //Retrieve info, existing data?
    TdataIn = (TTree*) RootInput->Get("Data");
    if(!TdataIn){
      std::cout<<"Could not retrieve the Data tree, leaving"<<std::endl;
      return false;
    }
    matchingbranch=TdataIn->SetBranchAddress("event",&eventptr);
    if(matchingbranch<0){
      std::cout<<"Error retrieving Event info from the tree"<<std::endl;
      return false;
    }
    
    TcalibIn = (TTree*) RootInput->Get("Calib");
    if(!TcalibIn){
      std::cout<<"Could not retrieve Calib tree, leaving"<<std::endl;
      return false;
    }
    else {
      matchingbranch=TcalibIn->SetBranchAddress("calib",&calibptr);
      if(matchingbranch<0){
        std::cout<<"Error retrieving calibration info from the tree"<<std::endl;
        TcalibIn=nullptr;
      } else {
        std::cout<<"Loaded the calib object"<<std::endl;
      }
    }
  } else {
    std::cout<<"An input file is required, aborting"<<std::endl;
    return false;
  }  

  return true;
}

// ****************************************************************************
// Primary process function to start all calibrations
// ****************************************************************************
bool ShapeAnalyzer::Process(void){
  bool status;
  ROOT::EnableImplicitMT();
  
  // copy full calibration to different file and calculate energy
  status=Plot();

  return status;
}


TH2D* ShapeAnalyzer::Normalize2D(TH2D* hist){

  int xnbins = hist->GetXaxis()->GetNbins();
  int ynbins = hist->GetYaxis()->GetNbins();
  double factor = 0;
  double bincontent = 0;
  const double entries = hist->GetEntries();   // SetBinContent increments the entry count
  for(int x = 1; x <= xnbins; x++){
    factor = hist->Integral(x,x,1,-1);
    if(factor <= 0) continue;   // empty column
    for(int y = 1; y <= ynbins; y++){
      bincontent = hist->GetBinContent(x,y);
      hist->SetBinContent(x,y,bincontent/factor);
    }
  }
  hist->SetEntries(entries);
  hist->GetZaxis()->SetTitle("fraction per #it{x} bin");

  return hist;
}

/*
bool ShapeAnalyzer::PassMuonCuts(Cluster clust){

  if(clust.hitTowers < 1.1){
  return true;
  }
  return false;

    
}
*/

// Single 1D distribution, saved as <sample dir>/EventDisplay_<Name>.
void ShapeAnalyzer::Plot1D(TH1D* hist, std::string Name, int sample, bool logY, std::string label){
  std::cout << "plotting 1D: " << Name << std::endl;
  using namespace PlotStyle;

  TCanvas* canvas = MakeCanvas(Form("c1D_%s", Name.c_str()), false);
  canvas->SetLogy(logY);
  StyleAxes(hist);
  StyleMarkers(hist, 0);
  SetYRange({hist}, logY, false);
  hist->Draw("p e1");
  DrawHeader(plotHeader, label);

  canvas->SaveAs(Form("%s/%s/EventDisplay_%s.%s", GetPlotOutputDir().Data(), SampleDir(sample), Name.c_str(), plotSuffix.Data()));
  delete canvas;
}

// Two 1D distributions overlaid without normalisation, saved as Comparisons/EventDisplay_<Name>.
void ShapeAnalyzer::Plot1D_Compare(TH1D* hist, std::string label, TH1D* hist2, std::string label2, std::string Name){
  std::cout << "plotting 1D overlay: " << Name << std::endl;
  using namespace PlotStyle;

  TCanvas* canvas = MakeCanvas(Form("c1DCmp_%s", Name.c_str()), false);
  canvas->SetLogy();
  StyleAxes(hist);
  StyleMarkers(hist, 0);
  StyleMarkers(hist2, 1);
  SetYRange({hist, hist2}, true, true);
  hist->Draw("p e1");
  hist2->Draw("p e1 same");

  TLegend* legend = MakeLegend(2, false);
  legend->AddEntry(hist,  label.c_str(),  "pl");
  legend->AddEntry(hist2, label2.c_str(), "pl");
  legend->Draw();
  DrawHeader(plotHeader, "");

  canvas->SaveAs(Form("%s/Comparisons/EventDisplay_%s.%s", GetPlotOutputDir().Data(), Name.c_str(), plotSuffix.Data()));
  delete legend;
  delete canvas;
}
// Shape comparison of two samples (e.g. muon-like vs non-muon-like, seeded vs window clustering),
// both normalised to unit area. Saved as Comparisons/<prefix>_<Name>.
void ShapeAnalyzer::Plot1D_Comparison(TH1D* h1, TH1D* h2, std::string label1, std::string label2,
                                      std::string prefix, std::string Name, bool logY, std::string label){
  Plot1D_Comparison(std::vector<TH1D*>{h1, h2}, std::vector<std::string>{label1, label2}, prefix, Name, logY, label);
}

// Shape comparison of several samples, all normalised to unit area. Saved as Comparisons/<prefix>_<Name>.
void ShapeAnalyzer::Plot1D_Comparison(std::vector<TH1D*> hists, std::vector<std::string> labels,
                                      std::string prefix, std::string Name, bool logY, std::string label){
  std::cout << "plotting " << prefix << ": " << Name << std::endl;
  using namespace PlotStyle;
  if(hists.empty() || hists.size() != labels.size()) return;

  std::vector<TH1*> clones;
  for(size_t i = 0; i < hists.size(); i++){
    TH1D* c = (TH1D*)hists[i]->Clone(Form("%s_cmp%zu", hists[i]->GetName(), i));
    c->SetDirectory(nullptr);
    if(c->Integral() > 0) c->Scale(1./c->Integral());
    StyleMarkers(c, i);
    clones.push_back(c);
  }
  clones[0]->GetYaxis()->SetTitle("normalized counts");

  TCanvas* canvas = MakeCanvas(Form("c%s_%s", prefix.c_str(), Name.c_str()), false);
  canvas->SetLogy(logY);
  StyleAxes(clones[0]);
  SetYRange(clones, logY, true);
  TLegend* legend = MakeLegend(clones.size(), false);
  for(size_t i = 0; i < clones.size(); i++){
    clones[i]->Draw(i == 0 ? "p e1" : "p e1 same");
    legend->AddEntry(clones[i], Form("%s (%.0f)", labels[i].c_str(), hists[i]->GetEntries()), "pl");
  }
  legend->Draw();
  DrawHeader(plotHeader, label);

  canvas->SaveAs(Form("%s/Comparisons/%s_%s.%s", GetPlotOutputDir().Data(), prefix.c_str(), Name.c_str(), plotSuffix.Data()));
  delete legend;
  delete canvas;
  for(TH1* c : clones) delete c;
}

// Two 2D distributions side by side with a common z-range. Saved as Comparisons/<prefix>2D_<Name>.
void ShapeAnalyzer::Plot2D_Comparison(TH2D* h1, TH2D* h2, std::string label1, std::string label2,
                                      std::string prefix, std::string Name, std::string countLabel){
  std::cout << "plotting " << prefix << " 2D: " << Name << std::endl;
  using namespace PlotStyle;

  TH2D* hists[2]        = {(TH2D*)h1->Clone(Form("%s_cmpA", h1->GetName())),
                           (TH2D*)h2->Clone(Form("%s_cmpB", h2->GetName()))};
  std::string labels[2] = {label1, label2};
  double entries[2]     = {h1->GetEntries(), h2->GetEntries()};
  SetZRange({hists[0], hists[1]});

  TCanvas* canvas = new TCanvas(Form("c%s2D_%s", prefix.c_str(), Name.c_str()), Name.c_str(), 2*kPanelW, kCanvasH);
  canvas->Divide(2,1,0,0);
  for(int i = 0; i < 2; i++){
    hists[i]->SetDirectory(nullptr);
    TVirtualPad* pad = canvas->cd(i+1);
    StylePad(pad, true);
    pad->SetLogz();
    StyleAxes(hists[i]);
    hists[i]->Draw("colz");
    DrawHeader(plotHeader, Form("%s (%.0f %s)", labels[i].c_str(), entries[i], countLabel.c_str()));
  }
  canvas->SaveAs(Form("%s/Comparisons/%s2D_%s.%s", GetPlotOutputDir().Data(), prefix.c_str(), Name.c_str(), plotSuffix.Data()));
  delete canvas;
  delete hists[0];
  delete hists[1];
}

// Event-by-event correlation between two quantities (e.g. seeded vs window), saved as Comparisons/<Name>.
void ShapeAnalyzer::Plot2D_Correlation(TH2D* hist, std::string Name, std::string label){
  std::cout << "plotting correlation: " << Name << std::endl;
  using namespace PlotStyle;

  TCanvas* canvas = MakeCanvas(Form("cCorr_%s", Name.c_str()), true);
  canvas->SetLogz();
  StyleAxes(hist);
  SetZRange({hist});
  hist->Draw("colz");
  DrawHeader(plotHeader, label);

  canvas->SaveAs(Form("%s/Comparisons/%s.%s", GetPlotOutputDir().Data(), Name.c_str(), plotSuffix.Data()));
  delete canvas;
}

// Single 2D distribution, saved as <sample dir>/Correlations/EventDisplay_<Name>.
void ShapeAnalyzer::Plot2D(TH2D* hist, std::string Name, int sample, std::string label){
  std::cout << "plotting 2D: " << Name << std::endl;
  using namespace PlotStyle;

  TCanvas* canvas = MakeCanvas(Form("c2D_%s", Name.c_str()), true);
  canvas->SetLogz();
  StyleAxes(hist);
  SetZRange({hist});
  hist->Draw("colz");
  DrawHeader(plotHeader, label);

  const TString subDir = (sample == 3) ? "Inclusive/Correlations_AltCluster" : Form("%s/Correlations", SampleDir(sample));
  canvas->SaveAs(Form("%s/%s/EventDisplay_%s.%s", GetPlotOutputDir().Data(), subDir.Data(), Name.c_str(), plotSuffix.Data()));
  delete canvas;
}


//***********************************************************************************************
//*********************** Plotting routine ***************************************************
//***********************************************************************************************
bool ShapeAnalyzer::Plot(){
  std::cout<<"Plotting events "<<plotEvt<<" to " << plotEvt+nEvts-1<<std::endl;
  if(plotMuonEvts) std::cout<<"Plotting muon-triggered events"<<std::endl;

  std::map<int,RunInfo> ri=readRunInfosFromFile(RunListInputName.Data(),debug,0);
  TdataIn->GetEntry(0);
  Int_t runNr = event.GetRunNumber();
  // Get run info object
  std::map<int,RunInfo>::iterator it=ri.find(runNr);

  // load calib
  TcalibIn->GetEntry(0);
  
  int species = -1;
  species = GetSpeciesIntFromRunInfo(it->second);
  if (species == -1){
      std::cout << "WARNING: species unknown: " << it->second.species.Data() << "  aborting"<< std::endl;
      return false;
  }
  
  std::cout << "debug level set to : " << debug << std::endl;
  
  // create 3D histo
  // int towersx         = setup->GetNMaxColumn()+1;/*GetNMAxColumn() returns */
  float towersx_min   = setup->GetMinX();
  float towersx_max   = setup->GetMaxX();
  int towersx         = (towersx_max-towersx_min)/setup->GetCellWidth();
  float towersy_min   = setup->GetMinY();
  float towersy_max   = setup->GetMaxY();
  int towersy         = (towersy_max-towersy_min)/setup->GetCellHeight();
  float towersz_min   = setup->GetMinZ();
  float towersz_max   = setup->GetMaxZ();
  int towersz         = (towersz_max-towersz_min)/setup->GetCellDepth();

  // clustering settings
  // The seeded clustering is the main method (Inclusive/, Muon/, nonMuon/ plots); the window clustering
  // is run on the same events for the seeded vs window comparisons (Comparisons/SeededVsWindow_*).
  // All thresholds come from clusterSettings (see ClusteringSettings in CommonHelperFunctions.h and the
  // SetCluster* methods in ShapeAnalyzer.h); the local names below are just shorthands.
  const ClusteringSettings& cs = clusterSettings;
  const double minwindow     = cs.minClusterE;     // minimum cluster energy for a cluster to be analysed
  const double seedValue     = cs.seedValue;       // minimum seed-cell energy
  const double cellThreshold = cs.cellThreshold;   // minimum cell energy (clustering and cell/tower spectra)
  const int    nLayers       = setup->GetNMaxLayer()+1;
  std::cout << "clustering settings: cell threshold " << cs.cellThreshold << ", seed " << cs.seedValue
            << ", grow threshold " << cs.growFraction << "*seed = " << cs.GrowThreshold()
            << ", min. cluster energy " << cs.minClusterE << ", window size " << cs.windowSize
            << ", max. clusters " << cs.maxClusters << std::endl;
  std::vector<Cluster> ClusterVec;
  std::vector<Cluster> ClusterVecWindow;
  double timeSeeded = 0, timeWindow = 0;   // total clustering time in us

  // axis titles: unit of the cell values depends on the plotted data type
  TString eUnit = "mip eq.";
  if(plotHGCROC && dataTypeHGCROC <= 1) eUnit = "ADC";
  if(plotHGCROC && dataTypeHGCROC == 2) eUnit = "TOT";
  auto eTitle = [&](const char* sub){ return TString(Form("#it{E}_{%s} (%s)", sub, eUnit.Data())); };
  auto t1     = [](const TString& x, const char* y = "counts"){ return TString(";")+x+";"+y; };
  const TString tNCl = Form("#it{N}_{clusters} (#it{E}_{cluster} > %g %s)", minwindow, eUnit.Data());

  // event-level histograms (inclusive only)
  TH1D* Cells_energy                    = new TH1D("Cells_energy",                    t1(eTitle("cell")),                100,0,300);
  TH1D* Towers_energy                   = new TH1D("Towers_energy",                   t1(eTitle("tower")),               100,0,300);
  TH1D* Clusters_NClusters              = new TH1D("Clusters_NClusters",              t1("#it{N}_{clusters}"),           11,-0.5,10.5);
  TH1D* Clusters_energy_first           = new TH1D("Clusters_energy_first",           t1(eTitle("1st cluster")),         50,0,kMaxClusterE);
  TH1D* Clusters_energy_second          = new TH1D("Clusters_energy_second",          t1(eTitle("2nd cluster")),         50,0,kMaxClusterE);
  TH1D* Clusters_eventenergy            = new TH1D("Clusters_eventenergy",            t1(eTitle("event")),               50,0,kMaxClusterE);
  TH1D* Clusters_windowminuseventenergy = new TH1D("Clusters_windowminuseventenergy", t1(Form("#it{E}_{event} - #it{E}_{1st cluster} (%s)", eUnit.Data())), 50,0,kMaxClusterE);
  TH1D* MuonPurity_Energy               = new TH1D("MuonPurity_energy",               t1(eTitle("cluster"), "muon-like fraction"), 50,0,kMaxClusterE);

  // Background (muon runs only, see IsBackgroundCluster) vs inclusive seeded clusters, both without the
  // minwindow cluster-energy cut. Cell energies are booked over 0-50 so that the shape comparison is
  // normalised in that range.
  const bool isMuonRun = (species == 2);
  TH1D* ClusterCells_energy       = new TH1D("ClusterCells_energy",       t1(eTitle("cell")),    100, 0, 10);
  TH1D* BackgroundCells_energy    = new TH1D("BackgroundCells_energy",    t1(eTitle("cell")),    100, 0, 10);
  // isolated cells (all runs): below seed threshold and no neighbouring cell with energy, see FindIsolatedCells
  TH1D* IsolatedCells_energy      = new TH1D("IsolatedCells_energy",      t1(eTitle("cell")),    100, 0, 10);
  TH1D* InclusiveClusters_energy  = new TH1D("InclusiveClusters_energy",  t1(eTitle("cluster")), 100, 0, kMaxClusterE);
  TH1D* BackgroundClusters_energy = new TH1D("BackgroundClusters_energy", t1(eTitle("cluster")), 100, 0, kMaxClusterE);
  int nBackgroundClusters = 0, nInclusiveClusters = 0;

  // cluster histograms, identical sets for all clusters, muon-like and non-muon-like clusters
  ClusterHists hAll    ("",         "seeded clustering, all clusters",  eUnit, towersx, towersy, towersz, nLayers);
  ClusterHists hMuon   ("_Muon",    "seeded clustering, muon-like",     eUnit, towersx, towersy, towersz, nLayers);
  ClusterHists hNonMuon("_nonMuon", "seeded clustering, non-muon-like", eUnit, towersx, towersy, towersz, nLayers);
  ClusterHists hWindow ("_Window",  "window clustering, all clusters",  eUnit, towersx, towersy, towersz, nLayers);

  // seeded vs window, per event (only clusters with energy > minwindow are counted)
  const TString tSumE  = eTitle("#Sigma clusters");
  const TString tLeadE = eTitle("lead cluster");
  TH1D* Clusters_NClusters_Seeded  = new TH1D("Clusters_NClusters_Seeded",  t1(tNCl),   11,-0.5,10.5);
  TH1D* Clusters_NClusters_Window  = new TH1D("Clusters_NClusters_Window",  t1(tNCl),   11,-0.5,10.5);
  TH1D* Clusters_SumEnergy_Seeded  = new TH1D("Clusters_SumEnergy_Seeded",  t1(tSumE),  50,0,kMaxClusterE);
  TH1D* Clusters_SumEnergy_Window  = new TH1D("Clusters_SumEnergy_Window",  t1(tSumE),  50,0,kMaxClusterE);
  TH1D* Clusters_LeadEnergy_Seeded = new TH1D("Clusters_LeadEnergy_Seeded", t1(tLeadE), 50,0,kMaxClusterE);
  TH1D* Clusters_LeadEnergy_Window = new TH1D("Clusters_LeadEnergy_Window", t1(tLeadE), 50,0,kMaxClusterE);
  TH2D* SeededVsWindow_NClusters   = new TH2D("SeededVsWindow_NClusters",
                                              ";#it{N}_{clusters}, seeded;#it{N}_{clusters}, window;events",
                                              11,-0.5,10.5,11,-0.5,10.5);
  TH2D* SeededVsWindow_LeadEnergy  = new TH2D("SeededVsWindow_LeadEnergy",
                                              ";"+tLeadE+", seeded;"+tLeadE+", window;events",
                                              100,0,kMaxClusterE,100,0,kMaxClusterE);
  TH2D* SeededVsWindow_SumEnergy   = new TH2D("SeededVsWindow_SumEnergy",
                                              ";"+tSumE+", seeded;"+tSumE+", window;events",
                                              100,0,kMaxClusterE,100,0,kMaxClusterE);
  TH2D* SeededVsWindow_LeadDist    = new TH2D("SeededVsWindow_LeadPosDiff",
                                              ";#it{x}_{lead, window} - #it{x}_{lead, seeded} (tower);#it{y}_{lead, window} - #it{y}_{lead, seeded} (tower);events",
                                              100,-3,3,100,-3,3);

  // per-event cell map, filled as (z, x, y)
  TH3F* hXYZMapEvt = new TH3F("hXYZMapEvt","",towersz, towersz_min, towersz_max, towersx, towersx_min, towersx_max, towersy, towersy_min, towersy_max);

  // common plot style and run header
  StyleSettingsBasics(plotSuffix);
  PlotStyle::ApplyGlobal();
  plotHeader = Form("#bf{LFHCal TB} %s, Run %d", GetStringFromRunInfo(it->second, 2).Data(), runNr);

  // creating plotting directory
  TString outputDirPlots = GetPlotOutputDir();
  gSystem->Exec("mkdir -p "+outputDirPlots);
  gSystem->Exec("mkdir -p "+outputDirPlots+"/Inclusive");
  gSystem->Exec("mkdir -p "+outputDirPlots+"/Comparisons");
  gSystem->Exec("mkdir -p "+outputDirPlots+"/Muon");
  gSystem->Exec("mkdir -p "+outputDirPlots+"/nonMuon");
  gSystem->Exec("mkdir -p "+outputDirPlots+"/Inclusive/Correlations");
  gSystem->Exec("mkdir -p "+outputDirPlots+"/Muon/Correlations");
  gSystem->Exec("mkdir -p "+outputDirPlots+"/nonMuon/Correlations");
  gSystem->Exec("mkdir -p "+outputDirPlots+"/Inclusive/Correlations_AltCluster");
   
  
  
  // processing events
  int evts=TdataIn->GetEntries();
  int evtsMuon= 0;
  
  // print calib info
  calib.PrintGlobalInfo();
  double averagePedMeanHG = calib.GetAveragePedestalMeanHigh();
  double averagePedSigHG  = calib.GetAveragePedestalSigHigh();
  double averagePedMeanLG = calib.GetAveragePedestalMeanLow();
  double averagePedSigLG  = calib.GetAveragePedestalSigLow();
  std::cout<< averagePedMeanHG << "\t" << averagePedSigHG << "\t" << averagePedMeanLG << "\t" << averagePedSigLG << std::endl;

  Double_t eCutoff = 0.3;
  if (plotHGCROC && dataTypeHGCROC==0)
    eCutoff = calib.GetAveragePedestalSigHigh()*5.;
  std::cout << "setting energy cut-off to: " << eCutoff << std::endl;
  double Etottot = 0;
  
  //TPolyLine3D* testline = new TPolyLine3D();
  for(int i=plotEvt; i<plotEvt+nEvts; i++){
    if(i > evts ) {
      std::cout<<"Requested event outside of the range. Aborting..."<<std::endl;
      return false;
    }
    TdataIn->GetEntry(i);
    // if( i != 258){continue;}
    if (i%1000 == 0 && debug > 0) std::cout << "Reading " <<  i << " / " << evts << " events" << std::endl;
    double Etot = 0;
    int nCells  = 0;
    bool muontrigg = false;
    int locMuon = 0;
    std::map<int,Layer> layers;
    std::map<int, Layer>::iterator ithLayer;
    
    if( debug>1 ) std::cout << "Event has " << event.GetNTiles() << " active tiles" << std::endl;
    for(int j=0; j<event.GetNTiles(); j++){
      
      if(plotHGCROC){
        Hgcroc* aTile = (Hgcroc*)event.GetTile(j);
        // no "energy" threshold for now - just plot everything
        nCells++;
        int currLayer   = setup->GetLayer(aTile->GetCellID());
        ithLayer        = layers.find(currLayer);
        double energy = 0;
        switch( dataTypeHGCROC ){
          case 0:
            energy = aTile->GetIntegratedADC(); 
            if( debug>1 ) std::cout << "CellID: " << aTile->GetCellID() << "\t ADC: " << aTile->GetIntegratedADC() << "\t energy " << energy << std::endl;
            break;
          case 1:
            energy = aTile->GetMaxSampleADC() - aTile->GetPedestal(); 
            if( debug>1 ) std::cout << "CellID: " << aTile->GetCellID() << "\t ADC: " << aTile->GetIntegratedADC() << "\t pedestal (calib) " << calib.GetPedestalMeanH( aTile->GetCellID() ) << "\t pedestal (aTile) " << aTile->GetPedestal() << "\t energy " << energy << std::endl;
            break;
          case 2:
            energy = aTile->GetRawTOT(); 
            if( debug>1 ) std::cout << "CellID: " << aTile->GetCellID() << "\t TOT: " << aTile->GetRawTOT() << "\t energy " << energy << std::endl;
            break;
          case 3:
            energy = aTile->GetIntegratedADC()/ calib.GetScaleHigh(aTile->GetCellID()); 
            if( debug>1 ) std::cout << "CellID: " << aTile->GetCellID() << "\t ADC: " << aTile->GetIntegratedADC() << "\t scale (calib) " << calib.GetScaleHigh( aTile->GetCellID() ) << "\t energy " << energy << std::endl;
            break;
          case 4:
            energy = aTile->GetE(); 
            if( debug>1 ) std::cout << "CellID: " << aTile->GetCellID() << "\t E: " << aTile->GetE() << std::endl;
            break;
        }

        if (dataTypeHGCROC == 4 && energy < energy) continue;
        
        if(ithLayer!=layers.end()){
          ithLayer->second.nCells+=1;
          ithLayer->second.energy+=energy;
          ithLayer->second.avX+=setup->GetX(aTile->GetCellID())*energy;
          ithLayer->second.avY+=setup->GetY(aTile->GetCellID())*energy;
        } else {
          layers[currLayer]=Layer();
          layers[currLayer].nCells+=1;
          layers[currLayer].energy+=energy;
          layers[currLayer].avX+=setup->GetX(aTile->GetCellID())*energy;
          layers[currLayer].avY+=setup->GetY(aTile->GetCellID())*energy;
        }
      } else {
        Caen* aTile=(Caen*)event.GetTile(j);
        if(aTile->GetE()>eCutoff ){ 
          nCells++;
          int currLayer = setup->GetLayer(aTile->GetCellID());
          ithLayer=layers.find(currLayer);
          if(ithLayer!=layers.end()){
            ithLayer->second.nCells+=1;
            ithLayer->second.energy+=aTile->GetE();
            ithLayer->second.avX+=setup->GetX(aTile->GetCellID())*aTile->GetE();
            ithLayer->second.avY+=setup->GetY(aTile->GetCellID())*aTile->GetE();
          } else {
            layers[currLayer]=Layer();
            layers[currLayer].nCells+=1;
            layers[currLayer].energy+=aTile->GetE();
            layers[currLayer].avX+=setup->GetX(aTile->GetCellID())*aTile->GetE();
            layers[currLayer].avY+=setup->GetY(aTile->GetCellID())*aTile->GetE();
          }
        }
        if (aTile->GetLocalTriggerBit() == 1){
          locMuon++;        
        }      
      }
    }
    //std::cout << "NCells hit: "  << nCells << "\t" << minTilesHit << std::endl;
    
    if (nCells > minTilesHit) {
      int nLayerSingleCell = 0;
      for(ithLayer=layers.begin(); ithLayer!=layers.end(); ++ithLayer){
        if (ithLayer->second.nCells == 1) 
            nLayerSingleCell++;
      }
      double fracLayer1Cell = (double)nLayerSingleCell/(int)layers.size();
      double fracLocMuon = (double)locMuon/nCells;
      if (fracLocMuon > 0.6){ 
        muontrigg = true;
	
	std::cout << "muon triggered " << std::endl;
        evtsMuon++;
      }
      if (muontrigg && debug > 1){
          std::cout << "Event: " << i << "\tMuon triggered:\t" <<  fracLayer1Cell << "\t" << fracLocMuon << std::endl;
      }
      Float_t minE = 1e10;
      Float_t maxE = 0;
      for(int j=0; j<event.GetNTiles(); j++){
        double energy  = 0;
        double x, y, z; 
        if(plotHGCROC){
          Hgcroc* aTile   = (Hgcroc*)event.GetTile(j);
          if( calib.GetBadChannel(aTile->GetCellID())!=3 && calib.GetBCCalib() == true) continue;
          switch( dataTypeHGCROC ){
            case 0:
              energy = aTile->GetIntegratedADC();
              break;
            case 1:
              energy = aTile->GetIntegratedADC()- calib.GetPedestalMeanL(aTile->GetCellID());
              break;
            case 2:
              energy = aTile->GetRawTOT();
              break;
            case 3:
              energy = aTile->GetIntegratedADC()/calib.GetScaleHigh(aTile->GetCellID());
              break;
            case 4:
              energy = aTile->GetE();
              break;
          }
          x = (double) aTile->GetX();
          y = (double) aTile->GetY();
          z = (double) aTile->GetZ();
          if( debug>2 ) std::cout << "CellID: " << aTile->GetCellID() << "\t value: " << energy  << "\tx,y,z: " <<x<< "\t" <<y<< "\t" << z << std::endl;
        } else {
          Caen* aTile=(Caen*)event.GetTile(j);
          if( calib.GetBadChannel(aTile->GetCellID())!=3) continue; 
          energy = (Float_t)aTile->GetE();
          x = (double) aTile->GetX();
          y = (double) aTile->GetY();
          z = (double) aTile->GetZ();
        }
	Etottot+=energy;
        Etot+=energy;
        if (energy < minE) minE = energy;
        if (energy > maxE) maxE = energy;
        
	
        if(energy>eCutoff){
          hXYZMapEvt->Fill(z,x,y,energy);
          if(debug > 2) std::cout << "Event, x, y, z, E: " << i << "\t" <<x<< "\t" <<y<< "\t" << z<< "\t" <<energy<<std::endl;
        }
	
      }
   
    
    
    }


    // run both clustering methods, each on its own copy: the clustering removes the clustered cells from its input
    TH3F* hXYZMapEvt_Clus = (TH3F*) hXYZMapEvt->Clone("hXYZMapEvt_Clus");
    auto t0 = std::chrono::high_resolution_clock::now();
    ClusterVec = DoClustering_Seeded_3D(hXYZMapEvt_Clus, cs);
    auto t1 = std::chrono::high_resolution_clock::now();
    delete hXYZMapEvt_Clus;

    hXYZMapEvt_Clus = (TH3F*) hXYZMapEvt->Clone("hXYZMapEvt_Clus");
    auto t2 = std::chrono::high_resolution_clock::now();
    ClusterVecWindow = DoClustering_Window(hXYZMapEvt_Clus, cs);
    auto t3 = std::chrono::high_resolution_clock::now();
    delete hXYZMapEvt_Clus;

    const double dtSeeded = std::chrono::duration<double, std::micro>(t1-t0).count();
    const double dtWindow = std::chrono::duration<double, std::micro>(t3-t2).count();
    timeSeeded += dtSeeded;
    timeWindow += dtWindow;
    if(debug > 1)
      std::cout << "Event " << i << ": seeded " << ClusterVec.size() << " clusters (" << dtSeeded << " us), window "
                << ClusterVecWindow.size() << " clusters (" << dtWindow << " us)" << std::endl;

    Clusters_NClusters->Fill(ClusterVec.size());

    // seeded vs window: same per-cluster observables, plus event-by-event correlations
    struct EventSummary{ int n = 0; double sumE = 0; const Cluster* lead = nullptr; };
    auto summarize = [&](const std::vector<Cluster>& clusters){
      EventSummary s;
      for(const Cluster& cl : clusters){
        if(cl.energy <= minwindow) continue;
        s.n++;
        s.sumE += cl.energy;
        if(!s.lead || cl.energy > s.lead->energy) s.lead = &cl;
      }
      return s;
    };
    for(const Cluster& cl : ClusterVecWindow)
      if(cl.energy > minwindow) hWindow.Fill(cl);
    const EventSummary evSeeded = summarize(ClusterVec);
    const EventSummary evWindow = summarize(ClusterVecWindow);
    Clusters_NClusters_Seeded->Fill(evSeeded.n);
    Clusters_NClusters_Window->Fill(evWindow.n);
    SeededVsWindow_NClusters->Fill(evSeeded.n, evWindow.n);
    if(evSeeded.n > 0){
      Clusters_SumEnergy_Seeded->Fill(evSeeded.sumE);
      Clusters_LeadEnergy_Seeded->Fill(evSeeded.lead->energy);
    }
    if(evWindow.n > 0){
      Clusters_SumEnergy_Window->Fill(evWindow.sumE);
      Clusters_LeadEnergy_Window->Fill(evWindow.lead->energy);
    }
    if(evSeeded.n > 0 && evWindow.n > 0){
      SeededVsWindow_SumEnergy->Fill(evSeeded.sumE, evWindow.sumE);
      SeededVsWindow_LeadEnergy->Fill(evSeeded.lead->energy, evWindow.lead->energy);
      SeededVsWindow_LeadDist->Fill(evWindow.lead->x-evSeeded.lead->x, evWindow.lead->y-evSeeded.lead->y);
    }
    ClusterVecWindow.clear();

    // isolated cells, from the full event map (they can never be part of a seeded cluster)
    for(float e : FindIsolatedCells(hXYZMapEvt, cs)) IsolatedCells_energy->Fill(e);

    // background vs inclusive clusters: every seeded cluster, no minimum cluster energy
    for(const Cluster& cl : ClusterVec){
      const bool isBackground = isMuonRun && IsBackgroundCluster(cl);
      nInclusiveClusters++;
      InclusiveClusters_energy->Fill(cl.energy);
      if(isBackground){
        nBackgroundClusters++;
        BackgroundClusters_energy->Fill(cl.energy);
      }
      for(float e : cl.cellEnergies){
        ClusterCells_energy->Fill(e);
        if(isBackground) BackgroundCells_energy->Fill(e);
      }
    }

    bool hasGoodCluster = false;
    for(size_t iCl = 0; iCl < ClusterVec.size(); iCl++){
      const Cluster& cl = ClusterVec[iCl];
      if(cl.energy <= minwindow) continue;
      hasGoodCluster = true;

      hAll.Fill(cl);
      if(IsMuonLikeCluster(cl)) hMuon.Fill(cl);
      else                      hNonMuon.Fill(cl);


      if(iCl == 0){
        Clusters_energy_first->Fill(cl.energy);
        Clusters_windowminuseventenergy->Fill(cl.eventenergy-cl.energy);
      }
      if(iCl == 1) Clusters_energy_second->Fill(cl.energy);
    }

    // event-level spectra, for events with at least one analysed cluster
    if(hasGoodCluster){
      Clusters_eventenergy->Fill(ClusterVec[0].eventenergy);
      for(int x = 1; x <= towersx; x++){
        for(int y = 1; y <= towersy; y++){
          double towerenergy = 0;
          for(int z = 1; z <= towersz; z++){
            double cellenergy = hXYZMapEvt->GetBinContent(z,x,y);
            if(cellenergy >= cellThreshold){   // same cell definition as the clustering
              towerenergy += cellenergy;
              Cells_energy->Fill(cellenergy);
            }
          }
          if(towerenergy > 0) Towers_energy->Fill(towerenergy);
        }
      }
    }

    ClusterVec.clear();
    hXYZMapEvt->Reset();
  }


  //DONE WITH EVENT

  // fraction of analysed clusters that are muon-like, vs cluster energy
  MuonPurity_Energy->Divide(hMuon.energy, hAll.energy, 1., 1., "B");

  // 2D correlations: normalise each energy column to unit integral
  for(ClusterHists* h : {&hAll, &hMuon, &hNonMuon, &hWindow})
    for(TH2D* h2 : h->EnergyCorrelations())
      Normalize2D(h2);

  // all per-cluster comparisons between two histogram sets, saved as Comparisons/<prefix>[2D]_<name>
  auto compareSets = [&](ClusterHists& a, ClusterHists& b, std::string labelA, std::string labelB, std::string prefix){
    Plot1D_Comparison(a.energy,         b.energy,         labelA, labelB, prefix, "WindowEnergy",         true);
    Plot1D_Comparison(a.towerImbalance, b.towerImbalance, labelA, labelB, prefix, "ClusterImbalance",     true);
    Plot1D_Comparison(a.cellImbalance,  b.cellImbalance,  labelA, labelB, prefix, "ClusterCellImbalance", true);
    Plot1D_Comparison(a.zImbalance,     b.zImbalance,     labelA, labelB, prefix, "ClusterImbalance_Z",   true);
    Plot1D_Comparison(a.firstZ,         b.firstZ,         labelA, labelB, prefix, "FirstHit_Z",           true);
    Plot1D_Comparison(a.lastZ,          b.lastZ,          labelA, labelB, prefix, "LastHit_Z",            true);
    Plot1D_Comparison(a.maxZ,           b.maxZ,           labelA, labelB, prefix, "MaxZ",                 true);
    Plot1D_Comparison(a.sizeZ,          b.sizeZ,          labelA, labelB, prefix, "ZHits",                false);
    Plot1D_Comparison(a.cellsPerLayer,  b.cellsPerLayer,  labelA, labelB, prefix, "XYHits",               true);
    Plot1D_Comparison(a.seedCellE,      b.seedCellE,      labelA, labelB, prefix, "SeedCellEnergy",       true);
    Plot1D_Comparison(a.seedTowerE,     b.seedTowerE,     labelA, labelB, prefix, "SeedTowerEnergy",      true);

    Plot2D_Comparison(a.e_towerImbalance, b.e_towerImbalance, labelA, labelB, prefix, "Imbalance");
    Plot2D_Comparison(a.e_seedCellE,      b.e_seedCellE,      labelA, labelB, prefix, "SeedCellsEnergy");
    Plot2D_Comparison(a.e_firstZ,         b.e_firstZ,         labelA, labelB, prefix, "FirstHit_Z");
    Plot2D_Comparison(a.e_lastZ,          b.e_lastZ,          labelA, labelB, prefix, "LastHit_Z");
    Plot2D_Comparison(a.e_maxZ,           b.e_maxZ,           labelA, labelB, prefix, "MaxZ");
    Plot2D_Comparison(a.e_sizeZ,          b.e_sizeZ,          labelA, labelB, prefix, "ZHits");
    Plot2D_Comparison(a.e_cellsPerLayer,  b.e_cellsPerLayer,  labelA, labelB, prefix, "XYHits");
    Plot2D_Comparison(a.xy,               b.xy,               labelA, labelB, prefix, "ClusterXY");
    Plot2D_Comparison(a.maxCellXY,        b.maxCellXY,        labelA, labelB, prefix, "MaxCellXY");
    Plot2D_Comparison(a.subXY,            b.subXY,            labelA, labelB, prefix, "SubClusterXY");
  };

  //
  // Muon-like vs non-muon-like (seeded clustering), Comparisons/MuonVsNonMuon_*
  //
  compareSets(hMuon, hNonMuon, "muon-like", "non-muon-like", "MuonVsNonMuon");

  //
  // Seeded vs window clustering on the same events, Comparisons/SeededVsWindow_*
  //
  compareSets(hAll, hWindow, "seeded", "window", "SeededVsWindow");
  Plot1D_Comparison(Clusters_NClusters_Seeded,  Clusters_NClusters_Window,  "seeded", "window", "SeededVsWindow", "NClusters",         false);
  Plot1D_Comparison(Clusters_SumEnergy_Seeded,  Clusters_SumEnergy_Window,  "seeded", "window", "SeededVsWindow", "SumClusterEnergy",  true);
  Plot1D_Comparison(Clusters_LeadEnergy_Seeded, Clusters_LeadEnergy_Window, "seeded", "window", "SeededVsWindow", "LeadClusterEnergy", true);
  const std::string evLabel = Form("seeded vs window, clusters with #it{E} > %g %s", minwindow, eUnit.Data());
  Plot2D_Correlation(SeededVsWindow_NClusters,  "SeededVsWindow_Corr_NClusters",         evLabel);
  Plot2D_Correlation(SeededVsWindow_SumEnergy,  "SeededVsWindow_Corr_SumClusterEnergy",  evLabel);
  Plot2D_Correlation(SeededVsWindow_LeadEnergy, "SeededVsWindow_Corr_LeadClusterEnergy", evLabel);
  Plot2D_Correlation(SeededVsWindow_LeadDist,   "SeededVsWindow_LeadClusterPosDiff",     evLabel);
  //
  // Background cells vs all cells in clusters (muon runs), Comparisons/BackgroundVsAll_CellEnergy
  //
  if(isMuonRun){
    std::cout << "background clusters (no tower with >= " << kMinTrackTowerSizeZ << " hit z-bins): "
              << nBackgroundClusters << " of " << nInclusiveClusters << " (no cluster-energy cut)" << std::endl;
    const std::string bgLabel = Form("background: no tower with #geq %d hit layers, no #it{E}_{cluster} cut", kMinTrackTowerSizeZ);
    Plot1D_Comparison(BackgroundCells_energy, ClusterCells_energy, "background clusters", "all clusters",
                      "BackgroundVsAll", "CellEnergy", true, bgLabel);
    Plot1D_Comparison(BackgroundClusters_energy, InclusiveClusters_energy, "background clusters", "all clusters",
                      "BackgroundVsAll", "ClusterEnergy", true, bgLabel);
    Plot1D_Comparison({ClusterCells_energy, BackgroundCells_energy, IsolatedCells_energy},
                      {"all clusters", "background clusters", Form("isolated cells, #it{E} < %g", seedValue)},
                      "BackgroundVsAll", "CellEnergy_WithIsolated", true, bgLabel);
  } else {
    std::cout << "not a muon run, skipping background-cluster comparison" << std::endl;
    Plot1D_Comparison(IsolatedCells_energy, ClusterCells_energy, Form("isolated cells, #it{E} < %g", seedValue),
                      "all clusters", "IsolatedVsAll", "CellEnergy", true);
  }
  std::cout << "isolated cells: " << IsolatedCells_energy->GetEntries() << std::endl;

  if(nEvts > 0)
    std::cout << "average clustering time per event: seeded " << timeSeeded/nEvts << " us, window " << timeWindow/nEvts << " us" << std::endl;

  //
  // Inclusive-only distributions
  //
  const std::string seededLabel = hAll.label.Data();
  Plot1D_Compare(Towers_energy, "all towers", hAll.seedTowerE, "max. tower of cluster", "Towers_Energy");
  Plot1D_Compare(Cells_energy,  "all cells",  hAll.seedCellE,  "max. cell of cluster",  "Cells_Energy");
  Plot1D(Cells_energy,                    "CellEnergy",                   0, true,  "events with a cluster");
  Plot1D(Towers_energy,                   "TowerEnergy",                  0, true,  "events with a cluster");
  Plot1D(Clusters_NClusters,              "NClusters",                    0, true,  seededLabel);
  Plot1D(Clusters_energy_first,           "WindowEnergy_FirstCluster",    0, true,  seededLabel);
  Plot1D(Clusters_energy_second,          "WindowEnergy_SecondCluster",   0, true,  seededLabel);
  Plot1D(Clusters_eventenergy,            "EventEnergy",                  0, true,  "events with a cluster");
  Plot1D(Clusters_windowminuseventenergy, "EventMinusFirstClusterEnergy", 0, true,  seededLabel);
  Plot1D(MuonPurity_Energy,               "MuonPurity",                   0, false, seededLabel);

  //
  // Per-sample distributions (Inclusive/, Muon/, nonMuon/)
  //
  auto plotSample = [&](ClusterHists& h, int sample){
    const std::string s = h.tag.Data();
    const std::string l = h.label.Data();
    Plot1D(h.energy,         "WindowEnergy"+s,         sample, true, l);
    Plot1D(h.towerImbalance, "ClusterImbalance"+s,     sample, true, l);
    Plot1D(h.cellImbalance,  "ClusterCellImbalance"+s, sample, true, l);
    Plot1D(h.zImbalance,     "ClusterImbalance_Z"+s,   sample, true, l);
    Plot1D(h.firstZ,         "FirstHit_Z"+s,           sample, true, l);
    Plot1D(h.lastZ,          "LastHit_Z"+s,            sample, true, l);
    Plot1D(h.maxZ,           "MaxZ"+s,                 sample, true, l);
    Plot1D(h.sizeZ,          "ZHits"+s,                sample, true, l);
    Plot1D(h.cellsPerLayer,  "XYHits"+s,               sample, true, l);
    Plot1D(h.seedCellE,      "SeedCellEnergy"+s,       sample, true, l);
    Plot1D(h.seedTowerE,     "SeedTowerEnergy"+s,      sample, true, l);

    Plot2D(h.e_towerImbalance, "Imbalance"+s,       sample, l);
    Plot2D(h.e_seedCellE,      "SeedCellsEnergy"+s, sample, l);
    Plot2D(h.e_firstZ,         "FirstHit_Z"+s,      sample, l);
    Plot2D(h.e_lastZ,          "LastHit_Z"+s,       sample, l);
    Plot2D(h.e_maxZ,           "MaxZ"+s,            sample, l);
    Plot2D(h.e_sizeZ,          "ZHits"+s,           sample, l);
    Plot2D(h.e_cellsPerLayer,  "XYHits"+s,          sample, l);
    Plot2D(h.xy,               "ClusterXY"+s,       sample, l);
    Plot2D(h.maxCellXY,        "MaxCellXY"+s,       sample, l);
    Plot2D(h.subXY,            "SubClusterXY"+s,    sample, l);
  };
  plotSample(hAll,     0);
  plotSample(hMuon,    1);
  plotSample(hNonMuon, 2);
  plotSample(hWindow,  0);   // window clustering, *_Window in Inclusive/

  //DONE

  delete hXYZMapEvt;



  std::cout << "done plotting!!" << std::endl;
  
  return true;
}
