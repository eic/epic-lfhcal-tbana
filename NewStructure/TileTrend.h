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

/**
 * Container for per-cell trending data and plotting metadata.
 *
 * TileTrend stores a set of ROOT TGraphErrors objects for pedestal, gain,
 * correlation, trigger and other calibration trends associated with one
 * logical cell. It acts as a small data holder and plotting helper for the
 * analysis code that assembles run-by-run trend plots.
 */
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
      InitTrendGraph(gTrendLGped, Form("TrendLGpedCellID%d",CellID), "#mu_{PED, LG} (arb. units)", kRed);
      InitTrendGraph(gTrendHGped, Form("TrendHGpedCellID%d",CellID), "#mu_{PED, HG} (arb. units)", kRed);
      InitTrendGraph(gTrendLGpedwidth, Form("TrendLGpedwidthCellID%d",CellID), "#sigma_{PED, LG} (arb. units)", kRed);
      InitTrendGraph(gTrendHGpedwidth, Form("TrendHGpedwidthCellID%d",CellID), "#sigma_{PED, HG} (arb. units)", kRed);
      InitTrendGraph(gTrendLGscale, Form("TrendLGscaleCellID%d",CellID), "Max_{LG} (arb. units)", kRed);
      InitTrendGraph(gTrendHGscale, Form("TrendHGscaleCellID%d",CellID), "Max_{HG} (arb. units)", kRed);
      InitTrendGraph(gTrendHGLGcorr, Form("TrendHGLGcorrCellID%d",CellID), "a_{HG-LG} (arb. units)", kRed);
      InitTrendGraph(gTrendLGHGcorr, Form("TrendLGHGcorrCellID%d",CellID), "a_{LG-HG} (arb. units)", kRed);
      InitTrendGraph(gTrendHGLGOffset, Form("TrendHGLGOffsetCellID%d",CellID), "b_{HG,LG} (arb. units)", kRed);
      InitTrendGraph(gTrendLGHGOffset, Form("TrendLGHGOffsetCellID%d",CellID), "b_{LG,HG} (arb. units)", kRed);
      InitTrendGraph(gTrendTOT, Form("TrendTOTCellID%d",CellID), "TOT (arb. units)", kRed);
    }
    
    
    // also monitoring triggers
    if (ext == 1 || ext == 2){
      InitTrendGraph(gTrendTrigger, Form("TrendTriggerCellID%d",CellID), "#mu triggers", kRed);
      InitTrendGraph(gTrendSBNoise, Form("TrendSBNoiseCellID%d",CellID), "S/B noise region", kRed);
      InitTrendGraph(gTrendSBSignal, Form("TrendSBSignalCellID%d",CellID), "S/B signal region", kRed);
    }
    // monitor extended trending
    if (ext == 1){
      InitTrendGraph(gTrendHGLMPV, Form("TrendHGLMPVCellID%d",CellID), "MPV_{HG} (arb. units)", kRed);
      InitTrendGraph(gTrendHGLSigma, Form("TrendHGLSigmaCellID%d",CellID), "#sigma_{L,HG} (arb. units)", kRed);
      InitTrendGraph(gTrendHGGSigma, Form("TrendHGGSigmaCellID%d",CellID), "#sigma_{G,HG} (arb. units)", kRed);
      InitTrendGraph(gTrendLGLMPV, Form("TrendLGLMPVCellID%d",CellID), "MPV_{LG} (arb. units)", kRed);
      InitTrendGraph(gTrendLGLSigma, Form("TrendLGLSigmaCellID%d",CellID), "#sigma_{L,LG} (arb. units)", kRed);
      InitTrendGraph(gTrendLGGSigma, Form("TrendLGGSigmaCellID%d",CellID), "#sigma_{G,LG} (arb. units)", kRed);
    }
    // mode for injection plotting
    if (ext == 3){
      InitTrendGraph(gTrendHGped, Form("TrendHGpedCellID%d",CellID), "#mu_{PED, 0^{th} sample} (arb. units)", kRed);
      InitTrendGraph(gTrendHGpedwidth, Form("TrendHGpedwidthCellID%d",CellID), "#sigma_{PED, 0^{th} sample} (arb. units)", kRed);
    }
    // mode for injection dac plotting
    if (ext == 4){
      InitTrendGraph(gTrendHGped, Form("TrendHGpedCellID%d",CellID), "#mu_{PED, 0^{th} sample} (arb. units)", kRed);
      InitTrendGraph(gTrendHGpedwidth, Form("TrendHGpedwidthCellID%d",CellID), "#sigma_{PED, 0^{th} sample} (arb. units)", kRed);
      InitTrendGraph(gTrendADCMax, Form("TrendADCMaxCellID%d",CellID), "ADC_{max} (arb. units)", kRed);
      InitTrendGraph(gTrendADCSaturated, Form("TrendADCSaturatedCellID%d",CellID), "# samples ADC_{sat}", kRed);
      InitTrendGraph(gTrendTOA, Form("TrendTOACellID%d",CellID), "TOA (arb. units)", kRed);
      InitTrendGraph(gTrendNSampTOA, Form("TrendNSampleTOACellID%d",CellID), "# sample TOA fired", kRed);
      InitTrendGraph(gTrendNTOA, Form("TrendNTOACellID%d",CellID), "# TOA fired", kRed);
      InitTrendGraph(gTrendTOT, Form("TrendTOTCellID%d",CellID), "TOT (arb. units)", kRed);
      InitTrendGraph(gTrendTOTSaturated, Form("TrendTOTSaturatedCellID%d",CellID), "# samples TOT_{sat}", kRed);
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
  bool FillHGCROCVals (double x, double tot);
  
  
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
  inline double GetMinLGped() const     {return MinLGped;};
  inline double GetMaxLGped() const     {return MaxLGped;};
  inline double GetMinHGped() const     {return MinHGped;};
  inline double GetMaxHGped() const     {return MaxHGped;};
  inline double GetMinLGpedwidth() const {return MinLGpedwidth;};
  inline double GetMaxLGpedwidth() const {return MaxLGpedwidth;};
  inline double GetMinHGpedwidth() const {return MinHGpedwidth;};
  inline double GetMaxHGpedwidth() const {return MaxHGpedwidth;};
  inline double GetMinLGscale() const   {return MinLGscale;};
  inline double GetMaxLGscale() const   {return MaxLGscale;};
  inline double GetMinHGscale() const   {return MinHGscale;};
  inline double GetMaxHGscale() const   {return MaxHGscale;};
  inline double GetMinHGLGcorr() const  {return MinHGLGcorr;};
  inline double GetMaxHGLGcorr() const  {return MaxHGLGcorr;};
  inline double GetMinLGHGcorr() const  {return MinLGHGcorr;};
  inline double GetMaxLGHGcorr() const  {return MaxLGHGcorr;};

   
  // Get minima and maxima for different graphs extended graphs
  inline double GetMinTrigg() const     {return MinTrigg;};
  inline double GetMaxTrigg() const     {return MaxTrigg;};
  inline double GetMinHGSpec() const    {return MinHGSpec;};
  inline double GetMaxHGSpec() const    {return MaxHGSpec;};
  inline double GetMinLGSpec() const    {return MinLGSpec;};
  inline double GetMaxLGSpec() const    {return MaxLGSpec;};
  inline double GetMinLGMPV() const     {return MinLGMPV;};
  inline double GetMaxLGMPV() const     {return MaxLGMPV;};
  inline double GetMinHGMPV() const     {return MinHGMPV;};
  inline double GetMaxHGMPV() const     {return MaxHGMPV;};
  inline double GetMinHGLSigma() const  {return MinHGLSigma;};
  inline double GetMaxHGLSigma() const  {return MaxHGLSigma;};
  inline double GetMinLGLSigma() const  {return MinLGLSigma;};
  inline double GetMaxLGLSigma() const  {return MaxLGLSigma;};
  inline double GetMinHGGSigma() const  {return MinHGGSigma;};
  inline double GetMaxHGGSigma() const  {return MaxHGGSigma;};
  inline double GetMinLGGSigma() const  {return MinLGGSigma;};
  inline double GetMaxLGGSigma() const  {return MaxLGGSigma;};
  inline double GetMinSBSignal() const  {return MinSBSignal;};
  inline double GetMaxSBSignal() const  {return MaxSBSignal;};
  inline double GetMinSBNoise() const   {return MinSBNoise;};
  inline double GetMaxSBNoise() const   {return MaxSBNoise;};
  inline double GetMinLGHGOffset() const {return MinLGHGOff;};
  inline double GetMaxLGHGOffset() const {return MaxLGHGOff;};
  inline double GetMaxHGLGOffset() const {return MaxHGLGOff;};
  inline double GetMinHGLGOffset() const {return MinHGLGOff;};
   
   
  // Get minima and maxima for different graphs injection related
  inline double GetMaxADCmax() const    {return MaxADCmax;};
  inline double GetMaxADCsat() const    {return MaxADCsat;};
  inline double GetMaxTOT() const       {return MaxTOT;};
  inline double GetMaxTOTsat() const    {return MaxTOTsat;};
  inline double GetMaxTOA() const       {return MaxTOA;};
  inline double GetMaxNSampTOA() const  {return MaxNSampTOA;};
  inline double GetMaxNTOA() const      {return MaxNTOA;};
  inline double GetMinADCmax() const    {return MinADCmax;};
  inline double GetMinADCsat() const    {return MinADCsat;};
  inline double GetMinTOT() const       {return MinTOT;};
  inline double GetMinTOTsat() const    {return MinTOTsat;};
  inline double GetMinTOA() const       {return MinTOA;};
  inline double GetMinNSampTOA() const  {return MinNSampTOA;};
  inline double GetMinNTOA() const      {return MinNTOA;};
   
  inline double GetMaxInjADC() const {return MaxInjADC;};
  inline double GetMaxInjTOT() const {return MaxInjTOT;};
   
   
  inline int GetNRuns() const           {return (int)runNrs.size();};
  inline int GetFirstRun() const        {if (runNrs.size()> 0) return runNrs[0]; else return -1;};
  inline int GetLastRun() const         {if (runNrs.size()> 0) return runNrs[runNrs.size()-1]; else return -1;};
  inline int GetRunNr(int i) const      {if (runNrs.size()> 0 && i < (int)runNrs.size()) return runNrs[i]; else return -1;}
  inline TString GetLabel(int i) const      {if (labels.size()> 0 && i < (int)labels.size()) return labels[i]; else return "";} 
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
  inline TGraphErrors* GetHGped()    {return &gTrendHGped;};
  inline TGraphErrors* GetLGped()    {return &gTrendLGped;};
  inline TGraphErrors* GetHGpedwidth() {return &gTrendHGpedwidth;};
  inline TGraphErrors* GetLGpedwidth() {return &gTrendLGpedwidth;};
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
   
  /**
  * Initialize a TGraphErrors instance with a consistent style.
  *
  * This keeps the repeated trend-graph setup in one place and avoids
  * repeating the same ROOT style calls for each per-cell trend.
  */
  static void InitTrendGraph(TGraphErrors& graph, const char* name, const char* yTitle, Color_t color)
  {
   graph = TGraphErrors();
   graph.SetName(name);
   graph.GetYaxis()->SetTitle(yTitle);
   graph.SetLineColor(color);
   graph.SetMarkerColor(color);
   graph.SetMarkerStyle(kFullCircle);
  }
   
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
  
  ClassDef(TileTrend,12);
};

#endif
