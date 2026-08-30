#ifndef TILETREND_H
#define TILETREND_H

#include "TObject.h"
#include "TString.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TPad.h" 
#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h"
#include "Calib.h"
#include "Setup.h"
#include "Tile.h"
#include "CommonHelperFunctions.h"

class TileTrend: public TObject{

 public:
 TileTrend():TObject(){}
 TileTrend(int id, int deb=0, int ext=0):TObject()
    {
    CellID         = id;
    debug          = deb;
    extended       = ext;
    
    // full filling for calib monitoring
    if (ext < 3){
      gTrendLGped    = TGraphErrors();
      gTrendLGped    .SetName(Form("TrendLGpedCellID%d",CellID));
      gTrendLGped    .GetYaxis()->SetTitle("#mu_{PED, LG} (arb. units)");
      gTrendLGped    .SetLineColor(kRed);
      gTrendLGped    .SetMarkerColor(kRed);
      gTrendLGped    .SetMarkerStyle(kFullCircle);
      
      gTrendHGped    = TGraphErrors();
      gTrendHGped    .SetName(Form("TrendHGpedCellID%d",CellID));
      gTrendHGped    .GetYaxis()->SetTitle("#mu_{PED, HG} (arb. units)");
      gTrendHGped    .SetLineColor(kRed);
      gTrendHGped    .SetMarkerColor(kRed);
      gTrendHGped    .SetMarkerStyle(kFullCircle);
      
      gTrendLGpedwidth  = TGraphErrors();
      gTrendLGpedwidth  .SetName(Form("TrendLGpedwidthCellID%d",CellID));
      gTrendLGpedwidth  .GetYaxis()->SetTitle("#sigma_{PED, LG} (arb. units)");
      gTrendLGpedwidth  .SetLineColor(kRed);
      gTrendLGpedwidth  .SetMarkerColor(kRed);

      gTrendHGpedwidth  = TGraphErrors();
      gTrendHGpedwidth  .SetName(Form("TrendHGpedwidthCellID%d",CellID));
      gTrendHGpedwidth  .GetYaxis()->SetTitle("#sigma_{PED, HG} (arb. units)");
      gTrendHGpedwidth  .SetLineColor(kRed);
      gTrendHGpedwidth  .SetMarkerColor(kRed);

      gTrendLGscale  = TGraphErrors();
      gTrendLGscale  .SetName(Form("TrendLGscaleCellID%d",CellID));
      gTrendLGscale  .GetYaxis()->SetTitle("Max_{LG} (arb. units)");
      gTrendLGscale  .SetLineColor(kRed);
      gTrendLGscale  .SetMarkerColor(kRed);
      gTrendLGscale  .SetMarkerStyle(kFullCircle);

      gTrendHGscale  = TGraphErrors();
      gTrendHGscale  .SetName(Form("TrendHGscaleCellID%d",CellID));
      gTrendHGscale  .GetYaxis()->SetTitle("Max_{HG} (arb. units)");
      gTrendHGscale  .SetLineColor(kRed);
      gTrendHGscale  .SetMarkerColor(kRed);
      gTrendHGscale  .SetMarkerStyle(kFullCircle);

      gTrendHGLGcorr = TGraphErrors();
      gTrendHGLGcorr .SetName(Form("TrendHGLGcorrCellID%d",CellID));
      gTrendHGLGcorr .GetYaxis()->SetTitle("a_{HG-LG} (arb. units)");
      gTrendHGLGcorr .SetLineColor(kRed);
      gTrendHGLGcorr .SetMarkerColor(kRed);
      gTrendHGLGcorr .SetMarkerStyle(kFullCircle);

      gTrendLGHGcorr = TGraphErrors();    
      gTrendLGHGcorr .SetName(Form("TrendLGHGcorrCellID%d",CellID));
      gTrendLGHGcorr .GetYaxis()->SetTitle("a_{LG-HG} (arb. units)");
      gTrendLGHGcorr .SetLineColor(kRed);
      gTrendLGHGcorr .SetMarkerColor(kRed);
      gTrendLGHGcorr .SetMarkerStyle(kFullCircle);    

      gTrendHGLGOffset = TGraphErrors();
      gTrendHGLGOffset .SetName(Form("TrendHGLGOffsetCellID%d",CellID));
      gTrendHGLGOffset .GetYaxis()->SetTitle("b_{HG,LG} (arb. units)");
      gTrendHGLGOffset .SetLineColor(kRed);
      gTrendHGLGOffset .SetMarkerColor(kRed);
      gTrendHGLGOffset .SetMarkerStyle(kFullCircle);

      gTrendLGHGOffset = TGraphErrors();
      gTrendLGHGOffset .SetName(Form("TrendLGHGOffsetCellID%d",CellID));
      gTrendLGHGOffset .GetYaxis()->SetTitle("b_{LG,HG} (arb. units)");
      gTrendLGHGOffset .SetLineColor(kRed);
      gTrendLGHGOffset .SetMarkerColor(kRed);
      gTrendLGHGOffset .SetMarkerStyle(kFullCircle);
    }
    
    // also monitoring triggers
    if (ext == 1 || ext == 2){
      // std::cout << "entering expanded option" << std::endl;
      gTrendTrigger = TGraphErrors();
      gTrendTrigger .SetName(Form("TrendTriggerCellID%d",CellID));
      gTrendTrigger .GetYaxis()->SetTitle("#mu triggers");
      gTrendTrigger .SetLineColor(kRed);
      gTrendTrigger .SetMarkerColor(kRed);
      gTrendTrigger .SetMarkerStyle(kFullCircle);
      
      gTrendSBNoise = TGraphErrors();
      gTrendSBNoise .SetName(Form("TrendSBNoiseCellID%d",CellID));
      gTrendSBNoise .GetYaxis()->SetTitle("S/B noise region");
      gTrendSBNoise .SetLineColor(kRed);
      gTrendSBNoise .SetMarkerColor(kRed);
      gTrendSBNoise .SetMarkerStyle(kFullCircle);
      
      gTrendSBSignal = TGraphErrors();
      gTrendSBSignal .SetName(Form("TrendSBSignalCellID%d",CellID));
      gTrendSBSignal .GetYaxis()->SetTitle("S/B signal region");
      gTrendSBSignal .SetLineColor(kRed);
      gTrendSBSignal .SetMarkerColor(kRed);
      gTrendSBSignal .SetMarkerStyle(kFullCircle);
    } 
    // monitor extended trending
    if (ext == 1){
      gTrendHGLMPV = TGraphErrors();
      gTrendHGLMPV .SetName(Form("TrendHGLMPVCellID%d",CellID));
      gTrendHGLMPV .GetYaxis()->SetTitle("MPV_{HG} (arb. units)");
      gTrendHGLMPV .SetLineColor(kRed);
      gTrendHGLMPV .SetMarkerColor(kRed);
      gTrendHGLMPV .SetMarkerStyle(kFullCircle);

      gTrendHGLSigma = TGraphErrors();
      gTrendHGLSigma .SetName(Form("TrendHGLSigmaCellID%d",CellID));
      gTrendHGLSigma .GetYaxis()->SetTitle("#sigma_{L,HG} (arb. units)");
      gTrendHGLSigma .SetLineColor(kRed);
      gTrendHGLSigma .SetMarkerColor(kRed);
      gTrendHGLSigma .SetMarkerStyle(kFullCircle);

      gTrendHGGSigma = TGraphErrors();
      gTrendHGGSigma .SetName(Form("TrendHGGSigmaCellID%d",CellID));
      gTrendHGGSigma .GetYaxis()->SetTitle("#sigma_{G,HG} (arb. units)");
      gTrendHGGSigma .SetLineColor(kRed);
      gTrendHGGSigma .SetMarkerColor(kRed);
      gTrendHGGSigma .SetMarkerStyle(kFullCircle);
      
      gTrendLGLMPV = TGraphErrors();
      gTrendLGLMPV .SetName(Form("TrendLGLMPVCellID%d",CellID));
      gTrendLGLMPV .GetYaxis()->SetTitle("MPV_{LG} (arb. units)");
      gTrendLGLMPV .SetLineColor(kRed);
      gTrendLGLMPV .SetMarkerColor(kRed);
      gTrendLGLMPV .SetMarkerStyle(kFullCircle);
      
      gTrendLGLSigma = TGraphErrors();
      gTrendLGLSigma .SetName(Form("TrendLGLSigmaCellID%d",CellID));
      gTrendLGLSigma .GetYaxis()->SetTitle("#sigma_{L,LG} (arb. units)");
      gTrendLGLSigma .SetLineColor(kRed);
      gTrendLGLSigma .SetMarkerColor(kRed);
      gTrendLGLSigma .SetMarkerStyle(kFullCircle);
      
      gTrendLGGSigma = TGraphErrors();
      gTrendLGGSigma .SetName(Form("TrendLGGSigmaCellID%d",CellID));
      gTrendLGGSigma .GetYaxis()->SetTitle("#sigma_{G,LG} (arb. units)");
      gTrendLGGSigma .SetLineColor(kRed);
      gTrendLGGSigma .SetMarkerColor(kRed);
      gTrendLGGSigma .SetMarkerStyle(kFullCircle);
    }
    // mode for injection plotting
    if (ext == 3){
      gTrendHGped    = TGraphErrors();
      gTrendHGped    .SetName(Form("TrendHGpedCellID%d",CellID));
      gTrendHGped    .GetYaxis()->SetTitle("#mu_{PED, 0^{th} sample} (arb. units)");
      gTrendHGped    .SetLineColor(kRed);
      gTrendHGped    .SetMarkerColor(kRed);
      gTrendHGped    .SetMarkerStyle(kFullCircle);      
      
      gTrendHGpedwidth  = TGraphErrors();
      gTrendHGpedwidth  .SetName(Form("TrendHGpedwidthCellID%d",CellID));
      gTrendHGpedwidth  .GetYaxis()->SetTitle("#sigma_{PED, 0^{th} sample} (arb. units)");
      gTrendHGpedwidth  .SetLineColor(kRed);
      gTrendHGpedwidth  .SetMarkerColor(kRed);

    }
    // mode for injection dac plotting
    if (ext == 4){
      gTrendHGped    = TGraphErrors();
      gTrendHGped    .SetName(Form("TrendHGpedCellID%d",CellID));
      gTrendHGped    .GetYaxis()->SetTitle("#mu_{PED, 0^{th} sample} (arb. units)");
      gTrendHGped    .SetLineColor(kRed);
      gTrendHGped    .SetMarkerColor(kRed);
      gTrendHGped    .SetMarkerStyle(kFullCircle);      

      gTrendHGpedwidth  = TGraphErrors();
      gTrendHGpedwidth  .SetName(Form("TrendHGpedwidthCellID%d",CellID));
      gTrendHGpedwidth  .GetYaxis()->SetTitle("#sigma_{PED, 0^{th} sample} (arb. units)");
      gTrendHGpedwidth  .SetLineColor(kRed);
      gTrendHGpedwidth  .SetMarkerColor(kRed);

      gTrendADCMax    = TGraphErrors();
      gTrendADCMax    .SetName(Form("TrendADCMaxCellID%d",CellID));
      gTrendADCMax    .GetYaxis()->SetTitle("ADC_{max} (arb. units)");
      gTrendADCMax    .SetLineColor(kRed);
      gTrendADCMax    .SetMarkerColor(kRed);
      gTrendADCMax    .SetMarkerStyle(kFullCircle);      
  
      gTrendADCSaturated    = TGraphErrors();
      gTrendADCSaturated    .SetName(Form("TrendADCSaturatedCellID%d",CellID));
      gTrendADCSaturated    .GetYaxis()->SetTitle("# samples ADC_{sat}");
      gTrendADCSaturated    .SetLineColor(kRed);
      gTrendADCSaturated    .SetMarkerColor(kRed);
      gTrendADCSaturated    .SetMarkerStyle(kFullCircle);      

      gTrendTOA    = TGraphErrors();
      gTrendTOA    .SetName(Form("TrendTOACellID%d",CellID));
      gTrendTOA    .GetYaxis()->SetTitle("TOA (arb. units)");
      gTrendTOA    .SetLineColor(kRed);
      gTrendTOA    .SetMarkerColor(kRed);
      gTrendTOA    .SetMarkerStyle(kFullCircle);      

      gTrendNSampTOA    = TGraphErrors();
      gTrendNSampTOA    .SetName(Form("TrendNSampleTOACellID%d",CellID));
      gTrendNSampTOA    .GetYaxis()->SetTitle("# sample TOA fired");
      gTrendNSampTOA    .SetLineColor(kRed);
      gTrendNSampTOA    .SetMarkerColor(kRed);
      gTrendNSampTOA    .SetMarkerStyle(kFullCircle);      
      
      gTrendNTOA    = TGraphErrors();
      gTrendNTOA    .SetName(Form("TrendNTOACellID%d",CellID));
      gTrendNTOA    .GetYaxis()->SetTitle("# TOA fired");
      gTrendNTOA    .SetLineColor(kRed);
      gTrendNTOA    .SetMarkerColor(kRed);
      gTrendNTOA    .SetMarkerStyle(kFullCircle);      

      gTrendTOT    = TGraphErrors();
      gTrendTOT    .SetName(Form("TrendTOTCellID%d",CellID));
      gTrendTOT    .GetYaxis()->SetTitle("TOT (arb. units)");
      gTrendTOT    .SetLineColor(kRed);
      gTrendTOT    .SetMarkerColor(kRed);
      gTrendTOT    .SetMarkerStyle(kFullCircle);      
      
      gTrendTOTSaturated    = TGraphErrors();
      gTrendTOTSaturated    .SetName(Form("TrendTOTSaturatedCellID%d",CellID));
      gTrendTOTSaturated    .GetYaxis()->SetTitle("# samples TOT_{sat}");
      gTrendTOTSaturated    .SetLineColor(kRed);
      gTrendTOTSaturated    .SetMarkerColor(kRed);
      gTrendTOTSaturated    .SetMarkerStyle(kFullCircle);      
  
    }
    
  }
  ~TileTrend(){}

  // Fill objects 
  bool Fill           (double, const TileCalib&, int, double, int, double, double, double, double);
  bool FillExtended   (double, int, int, TH1D*, TH1D*, TProfile*, TProfile* wave = nullptr);
  void FillMPV        (double, double, double, double, double);
  void FillLSigma     (double, double, double, double, double);
  void FillGSigma     (double, double, double, double, double);
  void FillSB         (double, double, double);
  void FillCorrOffset (double, double, double, double, double);  
  void FillHGCROCSetting (double val_rf = -1., double val_cf= -1., double val_cfcomp= -1., double val_cc= -1., double val_inj = -1.);  
  
  bool FillInjection  ( double x, double ped, int runNr, 
                        TProfile* wave, TProfile* toa, TProfile* tot, 
                        double val_rf = -1., double val_cf= -1., double val_cfcomp= -1., double val_cc= -1., double val_inj = -1.);

  bool FillInjectionDACVal  ( double x, double ped, double adc, double toa, double tot, int adcSatN = 0, int totSatN = 0, int nTOA = 0, int nSampToA = 0) ;
  
  
  
  // Drawing functions for graphs
  bool DrawLGped      (TString);
  bool DrawHGped      (TString);
  bool DrawLGpedwidth (TString);
  bool DrawHGpedwidth (TString);
  bool DrawLGscale    (TString);
  bool DrawHGscale    (TString);
  bool DrawHGLGcorr   (TString);
  bool DrawLGHGcorr   (TString);
  bool DrawTrigger    (TString);
  bool DrawSBNoise    (TString);
  bool DrawSBSignal   (TString);
  bool DrawHGLMPV     (TString);
  bool DrawLGLMPV     (TString);
  bool DrawHGLSigma   (TString);
  bool DrawLGLSigma   (TString);
  bool DrawHGGSigma   (TString);
  bool DrawLGGSigma   (TString);
  bool DrawHGLGOffset (TString);
  bool DrawLGHGOffset (TString);
  bool DrawADCmax     (TString);
  bool DrawADCsat     (TString);
  bool DrawTOT        (TString);
  bool DrawTOTsat     (TString);
  bool DrawTOA        (TString);
  bool DrawNSampTOA   (TString);
  bool DrawNTOA       (TString);
  TString GetLabelLegend(RunInfo , int, int );
  
  // Set default drawing options for all graphs
  bool SetLineColor   (uint);
  bool SetMarkerColor (uint);
  bool SetMarkerStyle (uint);
  bool SetXAxisTitle  (TString);
  void SetLabelPerRun (TString);
  
  // Sort graph according to voltage or run number
  void Sort          ();
  // Write graphs to file
  bool Write          (TFile*);
  bool Write          ();
  void PrintMinMaxRanges ();
  
  inline double GetExtOpt()       {return extended;};

  // Get minima and maxima for different graphs
  inline double GetMinLGped()     {return MinLGped;};
  inline double GetMaxLGped()     {return MaxLGped;};
  inline double GetMinHGped()     {return MinHGped;};
  inline double GetMaxHGped()     {return MaxHGped;};
  inline double GetMinLGpedwidth(){return MinLGpedwidth;};
  inline double GetMaxLGpedwidth(){return MaxLGpedwidth;};
  inline double GetMinHGpedwidth(){return MinHGpedwidth;};
  inline double GetMaxHGpedwidth(){return MaxHGpedwidth;};
  inline double GetMinLGscale()   {return MinLGscale;};
  inline double GetMaxLGscale()   {return MaxLGscale;};
  inline double GetMinHGscale()   {return MinHGscale;};
  inline double GetMaxHGscale()   {return MaxHGscale;};
  inline double GetMinHGLGcorr()  {return MinHGLGcorr;};
  inline double GetMaxHGLGcorr()  {return MaxHGLGcorr;};
  inline double GetMinLGHGcorr()  {return MinLGHGcorr;};
  inline double GetMaxLGHGcorr()  {return MaxLGHGcorr;};

  
  // Get minima and maxima for different graphs extended graphs
  inline double GetMinTrigg()     {return MinTrigg;};
  inline double GetMaxTrigg()     {return MaxTrigg;};
  inline double GetMinHGSpec()    {return MinHGSpec;};
  inline double GetMaxHGSpec()    {return MaxHGSpec;};
  inline double GetMinLGSpec()    {return MinLGSpec;};
  inline double GetMaxLGSpec()    {return MaxLGSpec;};
  inline double GetMinLGMPV()     {return MinLGMPV;};
  inline double GetMaxLGMPV()     {return MaxLGMPV;};
  inline double GetMinHGMPV()     {return MinHGMPV;};
  inline double GetMaxHGMPV()     {return MaxHGMPV;};
  inline double GetMinHGLSigma()  {return MinHGLSigma;};
  inline double GetMaxHGLSigma()  {return MaxHGLSigma;};
  inline double GetMinLGLSigma()  {return MinLGLSigma;};
  inline double GetMaxLGLSigma()  {return MaxLGLSigma;};
  inline double GetMinHGGSigma()  {return MinHGGSigma;};
  inline double GetMaxHGGSigma()  {return MaxHGGSigma;};
  inline double GetMinLGGSigma()  {return MinLGGSigma;};
  inline double GetMaxLGGSigma()  {return MaxLGGSigma;};
  inline double GetMinSBSignal()  {return MinSBSignal;};
  inline double GetMaxSBSignal()  {return MaxSBSignal;};
  inline double GetMinSBNoise()   {return MinSBNoise;};
  inline double GetMaxSBNoise()   {return MaxSBNoise;};
  inline double GetMinLGHGOffset(){return MinLGHGOff;};
  inline double GetMaxLGHGOffset(){return MaxLGHGOff;};
  inline double GetMaxHGLGOffset(){return MaxHGLGOff;};
  inline double GetMinHGLGOffset(){return MinHGLGOff;};
  
  
  // Get minima and maxima for different graphs injection related
  inline double GetMaxADCmax()    {return MaxADCmax;};
  inline double GetMaxADCsat()    {return MaxADCsat;};
  inline double GetMaxTOT()       {return MaxTOT;};
  inline double GetMaxTOTsat()    {return MaxTOTsat;};
  inline double GetMaxTOA()       {return MaxTOA;};
  inline double GetMaxNSampTOA()  {return MaxNSampTOA;};
  inline double GetMaxNTOA()      {return MaxNTOA;};
  inline double GetMinADCmax()    {return MinADCmax;};
  inline double GetMinADCsat()    {return MinADCsat;};
  inline double GetMinTOT()       {return MinTOT;};
  inline double GetMinTOTsat()    {return MinTOTsat;};
  inline double GetMinTOA()       {return MinTOA;};
  inline double GetMinNSampTOA()  {return MinNSampTOA;};
  inline double GetMinNTOA()      {return MinNTOA;};
  
  inline double GetMaxInjADC(){return MaxInjADC;};
  inline double GetMaxInjTOT(){return MaxInjTOT;};
  
  
  inline int GetNRuns()           {return (int)runNrs.size();};
  inline int GetFirstRun()        {if (runNrs.size()> 0) return runNrs[0]; else return -1;};
  inline int GetLastRun()         {if (runNrs.size()> 0) return runNrs[runNrs.size()-1]; else return -1;};
  inline int GetRunNr(int i)      {if (runNrs.size()> 0 && i < (int)runNrs.size()) return runNrs[i]; else return -1;}
  inline TString GetLabel(int i)      {if (labels.size()> 0 && i < (int)labels.size()) return labels[i]; else return "";}
  inline int GetPdg(int i)        {if (pdgs.size()> 0 && i < (int)pdgs.size()) return pdgs[i]; else return -1;}
  inline int GetVoltage(int i)    {if (voltages.size()> 0 && i < (int)voltages.size()) return voltages[i]; else return -1;}
  inline int GetRF(int i)         {if (rf.size()> 0 && i < (int)rf.size()) return rf[i]; else return -1;}
  inline int GetCF(int i)         {if (cf.size()> 0 && i < (int)cf.size()) return cf[i]; else return -1;}
  inline int GetCFComp(int i)     {if (cfcomp.size()> 0 && i < (int)cfcomp.size()) return cfcomp[i]; else return -1;}
  inline int GetCC(int i)         {if (cc.size()> 0 && i < (int)cc.size()) return cc[i]; else return -1;}
  inline double GetInj(int i)     {if (inj.size()> 0 && i < (int)inj.size()) return inj[i]; else return -1;}
  inline double GetEnergy(int i)  {if (energy.size()> 0 && i < (int)energy.size()) return energy[i]; else return -1;}
  inline double GetTemp(int i)  {if (temp.size()> 0 && i < (int)temp.size()) return temp[i]; else return -1;}
  
  // Getters for graphs
  inline TGraphErrors* GetHGped()     {return &gTrendHGped;};
  inline TGraphErrors* GetLGped()     {return &gTrendLGped;};
  inline TGraphErrors* GetHGpedwidth(){return &gTrendHGpedwidth;};
  inline TGraphErrors* GetLGpedwidth(){return &gTrendLGpedwidth;};
  inline TGraphErrors* GetHGScale()   {return &gTrendHGscale;};
  inline TGraphErrors* GetLGScale()   {return &gTrendLGscale;};
  inline TGraphErrors* GetLGHGcorr()  {return &gTrendLGHGcorr;};
  inline TGraphErrors* GetHGLGcorr()  {return &gTrendHGLGcorr;};
  // Getters for extended graphs
  inline TGraphErrors* GetTrigger()   {return &gTrendTrigger;};
  inline TGraphErrors* GetSBNoise()   {return &gTrendSBNoise;};
  inline TGraphErrors* GetSBSignal()  {return &gTrendSBSignal;};
  inline TGraphErrors* GetHGLMPV()    {return &gTrendHGLMPV;};
  inline TGraphErrors* GetLGLMPV()    {return &gTrendLGLMPV;};
  inline TGraphErrors* GetHGLSigma()  {return &gTrendHGLSigma;};
  inline TGraphErrors* GetLGLSigma()  {return &gTrendLGLSigma;};
  inline TGraphErrors* GetHGGSigma()  {return &gTrendHGGSigma;};
  inline TGraphErrors* GetLGGSigma()  {return &gTrendLGGSigma;};
  inline TGraphErrors* GetLGHGOff()   {return &gTrendLGHGOffset;};
  inline TGraphErrors* GetHGLGOff()   {return &gTrendHGLGOffset;};
  inline TGraphErrors* GetADCmax()    {return &gTrendADCMax;};
  inline TGraphErrors* GetADCsat()    {return &gTrendADCSaturated;};
  inline TGraphErrors* GetTOT()       {return &gTrendTOT;};
  inline TGraphErrors* GetTOTsat()    {return &gTrendTOTSaturated;};
  inline TGraphErrors* GetTOA()       {return &gTrendTOA;};
  inline TGraphErrors* GetNSampTOA()  {return &gTrendNSampTOA;};
  inline TGraphErrors* GetNTOA()      {return &gTrendNTOA;};
  
  // trending options dependencies
  TGraphErrors* GetTrendingBasedOnOption(int option);
  void GetMinMaxBasedOnOptionAndCompare(int, Double_t &, Double_t &);
  
  // Getters for individual graph histgrams
  TH1D* GetHGTriggRun(int);
  TH1D* GetLGTriggRun(int);
  TProfile* GetLGHGTriggRun(int);
  TProfile* GetWave1DRun(int);
  TProfile* GetTOTRun(int);
  TProfile* GetTOARun(int);
  
 protected:
  int CellID;
  int debug;
  int extended;
  TGraphErrors gTrendLGped    ;
  TGraphErrors gTrendHGped    ;
  TGraphErrors gTrendLGpedwidth ;
  TGraphErrors gTrendHGpedwidth ;
  TGraphErrors gTrendLGscale  ;
  TGraphErrors gTrendHGscale  ;
  TGraphErrors gTrendHGLGcorr ;
  TGraphErrors gTrendLGHGcorr ;
  TGraphErrors gTrendTrigger  ;

  TGraphErrors gTrendSBNoise  ;
  TGraphErrors gTrendSBSignal ;
  TGraphErrors gTrendHGLMPV   ;
  TGraphErrors gTrendHGLSigma ;
  TGraphErrors gTrendHGGSigma ;
  TGraphErrors gTrendLGLMPV   ;
  TGraphErrors gTrendLGLSigma ;
  TGraphErrors gTrendLGGSigma ;
  
  TGraphErrors gTrendLGHGOffset ;
  TGraphErrors gTrendHGLGOffset ;

  TGraphErrors gTrendADCMax;
  TGraphErrors gTrendADCSaturated;
  TGraphErrors gTrendTOA;
  TGraphErrors gTrendNSampTOA;
  TGraphErrors gTrendNTOA;
  TGraphErrors gTrendTOT;
  TGraphErrors gTrendTOTSaturated;
  
  double MaxLGped    =0.;
  double MaxHGped    =0.;
  double MaxLGpedwidth =0.;
  double MaxHGpedwidth =0.;
  double MaxLGscale  =0.;
  double MaxHGscale  =0.;
  double MaxHGLGcorr =0.;
  double MaxLGHGcorr =0.;
  double MaxTrigg    =0.;
  double MaxHGSpec   =0.;
  double MaxLGSpec   =0.;
  double MaxHGMPV    =0.;
  double MaxLGMPV    =0.;
  double MaxHGLSigma =0.;
  double MaxLGLSigma =0.;
  double MaxHGGSigma =0.;
  double MaxLGGSigma =0.;
  double MaxSBNoise  =0.;
  double MaxSBSignal =0.;
  double MaxLGHGOff   =-10000.;
  double MaxHGLGOff   =-10000.;
  double MaxInjADC    =-10000.;
  double MaxInjTOT    =-10000.;
  double MaxADCmax    =0.;
  double MaxADCsat    =0.;
  double MaxTOT       =0.;
  double MaxTOTsat    =0.;
  double MaxTOA       =0.;
  double MaxNSampTOA  =0.;
  double MaxNTOA      =0.;

  double MinLGped    =9999.;
  double MinHGped    =9999.;
  double MinLGpedwidth =9999.;
  double MinHGpedwidth =9999.;
  double MinLGscale  =9999.;
  double MinHGscale  =9999.;
  double MinHGLGcorr =9999.;
  double MinLGHGcorr =9999.;
  double MinTrigg    =9999.;
  double MinHGSpec   =9999.;
  double MinLGSpec   =9999.;
  double MinHGMPV    =9999.;
  double MinLGMPV    =9999.;
  double MinHGLSigma =9999.;
  double MinLGLSigma =9999.;
  double MinHGGSigma =9999.;
  double MinLGGSigma =9999.;
  double MinSBNoise  =9999.;
  double MinSBSignal =9999.;
  double MinLGHGOff  =9999.;
  double MinHGLGOff  =9999.;
  double MinADCmax    =9999.;
  double MinADCsat    =9999.;
  double MinTOT       =9999.;
  double MinTOTsat    =9999.;
  double MinTOA       =9999.;
  double MinNSampTOA  =9999.;
  double MinNTOA      =9999.;
  
  std::vector<TString> labels;
  std::vector<int> runNrs;
  std::vector<int> pdgs;
  std::vector<double> voltages;
  std::vector<double> rf;
  std::vector<double> cf;
  std::vector<double> cfcomp;
  std::vector<double> cc;
  std::vector<double> inj;
  std::vector<double> energy;
  std::vector<double> temp;
  std::map<int, TH1D> HGTriggRuns;
  std::map<int, TH1D> LGTriggRuns;
  std::map<int, TProfile> LGHGTriggRuns;
  std::map<int, TProfile> Wave1DProf;
  std::map<int, TProfile> TOAProf;
  std::map<int, TProfile> TOTProf;
  
  ClassDef(TileTrend,11);
};

#endif
