#ifndef MULTICANVAS_H
#define MULTICANVAS_H

#include "TLegend.h"
#include "TAxis.h"
#include "TColor.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TBox.h"
#include "TPad.h"
#include "TFrame.h"
#include "TLatex.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TGaxis.h"
#include "TSystem.h"
#include "TStyle.h"
#include <TH3.h>
#include "Caen.h"
#include "HGCROC.h"
#include "Setup.h"
#include "Calib.h"
#include "TileSpectra.h"  
#include "TileTrend.h"  
#include "CalibSummary.h"  
#include "AnaSummary.h"
#include "CommonHelperFunctions.h"
#include "PlotHelper.h"
// #include "PlotHelper_general.h"
// #include "PlotHelper_8MLayer.h"
// #include "PlotHelper_2MLayer.h"
// #include "PlotHelper_1MLayer.h"
// #include "PlotHelper_2ModLayer.h"
// #include "PlotHelper_AsicLFHCal.h"

class MultiCanvas{

 public:
  MultiCanvas(){}
  MultiCanvas(DetConf::Type det, TString name){
    detType = det;
    addName = name;
  }
  ~MultiCanvas(){}
  

  bool    Initialize(int);
  
  TCanvas*  GetCanvas() {if (init) return canvasMulti; else return nullptr;}
  TPad**  GetPad() {if (init) return padMulti; else return nullptr;}
  Double_t* GetLegPlace1D(int opt){ if (init) {if (opt==0) return legPlace_X; else return legPlace_Y;} else return nullptr;}
  Double_t* GetRelTextSize1D(){ if (init) return relTextSize; else return nullptr;}
  
  
  void ReturnCorrectValuesForCanvasScaling(   Int_t sizeX, Int_t sizeY, Int_t nCols, Int_t nRows, Double_t  leftMargin, Double_t rightMargin, Double_t upperMargin, Double_t lowerMargin, Double_t* arrayBoundariesX, Double_t* arrayBoundariesY, Double_t* relativeMarginsX, Double_t* relativeMarginsY, int verbose = 1);
  
  void ReturnCorrectValuesTextSize(   TPad * pad, Double_t &textsizeLabels, Double_t &textsizeFac, Int_t textSizeLabelsPixel, Double_t dummyWUP);
  
  void CreateCanvasAndPadsFor8PannelTBPlot(Int_t textSizePixel = 30, Double_t marginLeft = 0.03, TString add = "", bool rightCorner = true, int debug = 0);
  
  void CreateCanvasAndPadsFor2PannelTBPlot(Int_t textSizePixel = 30, Double_t marginLeft = 0.03, TString add = "", bool rightCorner = true, int debug = 0);
  
  void CreateCanvasAndPadsForDualModTBPlot(Int_t textSizePixel = 30, Double_t marginLeft = 0.03, TString add = "", bool rightCorner = true, int debug = 0);

  void CreateCanvasAndPadsForAsicLFHCalTBPlot( Int_t textSizePixel = 30, Double_t marginLeft = 0.03, TString add = "", bool rightCorner = true, int debug = 0);

  void DefaultCanvasSettings( TCanvas* c1, Double_t leftMargin, Double_t rightMargin, Double_t topMargin, Double_t bottomMargin);

  void DefaultPadSettings( TPad* pad1, Double_t leftMargin, Double_t rightMargin, Double_t topMargin, Double_t bottomMargin);

  void PlotNoiseWithFits( std::map<int,TileSpectra> spectra, int option, 
                          Double_t xPMin, Double_t xPMax, Double_t scaleYMax, 
                          TString nameOutputBase, TString suffix,  RunInfo currRunInfo, Calib* calib, int debug = 1 );
  void PlotNoiseAdvWithFits(  std::map<int,TileSpectra> spectra, std::map<int,TileSpectra> spectraNoise, int option, 
                              Double_t xPMin, Double_t xPMax, Double_t scaleYMax, 
                              TString nameOutputBase, TString suffix,  RunInfo currRunInfo, Calib* calib, int debug = 1);
  
  void PlotCorr2DLayer(   std::map<int,TileSpectra> spectra, int option, 
                          Double_t xPMin, Double_t xPMax, Double_t minY, Double_t maxY, 
                          TString nameOutputBase, TString suffix,  RunInfo currRunInfo, Calib* calib, 
                          int noCalib = 0, int triggerCha = -1, int debug = 1);
  void PlotCorrWithFits(   std::map<int,TileSpectra> spectra, int option, 
                            Double_t xPMin, Double_t xPMax, Double_t minY, Double_t maxY, 
                            TString nameOutputBase, TString suffix,  RunInfo currRunInfo, Calib* calib, 
                            int debug = 1);
  
  void PlotSpectra(  std::map<int,TileSpectra> spectra, int option, 
                     Double_t xPMin, Double_t xPMax, Double_t scaleYMax, 
                     TString nameOutputBase, TString suffix,  RunInfo currRunInfo, Calib* calib, 
                     int debug = 1);
  
  void PlotMipWithFits( std::map<int,TileSpectra> spectra, std::map<int,TileSpectra> spectraTrigg, 
                        int option, Double_t xPMin, Double_t xPMax, Double_t scaleYMax, 
                        TString nameOutputBase, TString suffix,  RunInfo currRunInfo, Calib* calib, int debug= 1 );

  void PlotTriggerPrim(  std::map<int,TileSpectra> spectra, 
                         double avMip, double facLow, double facHigh,
                         Double_t xPMin, Double_t xPMax, Double_t scaleYMax, 
                         TString nameOutputBase, TString suffix,  RunInfo currRunInfo, Calib* calib, 
                         int debug = 1);
  
 private:
  TCanvas* canvasMulti;
  TPad* padMulti[64];
  Double_t legPlace_X[64];
  Double_t legPlace_Y[64];
  Double_t relTextSize[64];
  bool init                = false;
  DetConf::Type detType    = DetConf::Type::Undef; 
  int maxPads              = 0;
  int textSize             = 30;
  // int maxRow               = 0;
  // int maxColumn            = 0;
  TString addName          = "";
  
 protected:

};


#endif
