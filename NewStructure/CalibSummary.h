#ifndef CALIBSUMMARY_H
#define CALIBSUMMARY_H

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

class CalibSummary: public TObject{

 public:
 CalibSummary():TObject(){}
 CalibSummary(int id,int RunNum, double v, int p = 0, int optHGCROC = 0):TObject()
 {
    RunNr             = RunNum;
    RunNrRef          = RunNum;
    Voltage           = v;
    pdg               = p;
    hHGped            = TH1D(Form("hMeanPedHG_%i",id),"; #mu_{noise, HG} (arb. units); counts ", 500, -0.5, 500-0.5);
    hHGpedwidth       = TH1D(Form("hMeanPedHGwidth_%i",id),"; #sigma_{noise, HG} (arb. units); counts ", 400, -0.5*50/400, 50-(0.5*50/400));
    hLGped            = TH1D(Form("hMeanPedLG_%i",id),"; #mu_{noise, LG} (arb. units); counts ", 500, -0.5, 500-0.5);
    hLGpedwidth       = TH1D(Form("hMeanPedLGwidth_%i",id),"; #sigma_{noise, LG} (arb. units); counts ", 400, -0.5*20/400, 20-(0.5*20/400));
    hHGscale          = TH1D(Form("hHGScale_%i",id),";Max_{HG} (arb. units) ; counts ", 2000, -0.25, 1000-0.25);
    hHGscalewidth     = TH1D(Form("hHGScalewidth_%i",id),";Width_{HG} (arb. units) ; counts ", 2000, -0.25, 1000-0.25);
    hLGscale          = TH1D(Form("hLGScale_%i",id),";Max_{LG} (arb. units) ; counts ", 2000, -0.5*250/2000, 250-(0.5*250/2000));
    hLGscaleCalc      = TH1D(Form("hLGScaleCalc_%i",id),";Max_{LG,calc} (arb. units) ; counts ", 2000, -0.5*250/2000, 250-(0.5*250/2000));
    hLGscalewidth     = TH1D(Form("hHGScalewidth_%i",id),";Width_{LG} (arb. units) ; counts ", 2000, -0.5*250/2000, 250-(0.5*250/2000));
    hLGHGcorr         = TH1D(Form("hLGHGCorr_%i",id),"; a_{LG-HG} (arb. units) ; counts ", 400, 0, 20);
    hLGHGOffcorr      = TH1D(Form("hLGHGOffCorr_%i",id),"; b_{LG-HG} (arb. units) ; counts ", 1000, -200, 100);
    hHGLGcorr         = TH1D(Form("hHGLGCorr_%i",id),"; a_{HG-LG} (arb. units) ; counts ", 400, 0., 1.);
    hHGLGOffcorr      = TH1D(Form("hHGLGOffCorr_%i",id),"; b_{HG-LG} (arb. units) ; counts ", 1000, -100., 100.);
    
    hHGpedDiffRef     = TH1D(Form("hDiffPedvsRefHG_%i",id),"; #mu_{noise, HG}-#mu_{noise, HG, ref run} (arb. units); counts ", 501, -100, 100);
    hHGpedwidthDiffRef= TH1D(Form("hDiffPedWidthvsRefHG_%i",id),"; #sigma_{noise, HG}-#sigma_{noise, HG, ref run} (arb. units); counts ", 501, -100, 100);
    hLGpedDiffRef     = TH1D(Form("hDiffPedvsRefLG_%i",id),"; #mu_{noise, LG}-#mu_{noise, LG, ref run} (arb. units); counts ", 501, -100, 100);
    hHGscaleDiffRef   = TH1D(Form("hDiffHGScalevsRefHG_%i",id),"; Max_{HG}-Max_{HG,ref run} (arb. units); counts ", 1001, -250, 250);
    hLGscaleDiffRef   = TH1D(Form("hDiffLGScalevsRefHG_%i",id),"; Max_{LG}-Max_{LG,ref run} (arb. units); counts ", 501, -100, 100);
    hLGscaleCalcDiffRef   = TH1D(Form("hDiffLGScaleCalcvsRefHG_%i",id),"; Max_{LG,calc}-Max_{LG,calc,ref run} (arb. units); counts ", 501, -100, 100);
    hLGHGcorrDiffRef  = TH1D(Form("hDiffLGScalevsRefHG_%i",id),"; Max_{LG}-Max_{LG,ref run} (arb. units); counts ", 501, -10, 10);

    if (optHGCROC > 0){
      hHGscaleCorrRef   = TH2D(Form("hHGscaleCorrRef_%i",id),";Max_{ADC} (arb. units); Max_{ADC, ref run} (arb. units); Max_{ADC, ref run} (arb. units) ; counts ", 350, -0.25, 350-0.25, 350, -0.25, 350-0.25);
      pHGscaleCorrRef   = TProfile(Form("pHGscaleCorrRef_%i",id),";Max_{ADC} (arb. units); Max_{ADC, ref run} (arb. units)", 350, -0.25, 350-0.25);
    } else {
      hHGscaleCorrRef   = TH2D(Form("hHGscaleCorrRef_%i",id),";Max_{HG} (arb. units); Max_{ADC, ref run} (arb. units) ; counts ", 1000, -0.25, 1000-0.25, 1000, -0.25, 1000-0.25);
      pHGscaleCorrRef   = TProfile(Form("pHGscaleCorrRef_%i",id), ";Max_{ADC} (arb. units); Max_{ADC, ref run} (arb. units)", 1000, -0.25, 1000-0.25);
    }
    
  }
  ~CalibSummary(){}

  int Analyse(int );
  bool Fill(const TileCalib&);
	bool Fill(TileCalib*); // -EP
  bool Write(TFile*);
  bool SetDeltaTimeHist(TH1D*);
  
  bool FillRefRunProps( const TileCalib&, const TileCalib&);
  bool FillRefRunProps( TileCalib*, TileCalib*);
  
  inline void SetRefRunNr(int runNr)  { RunNrRef = runNr; };
  inline void SetLabel(TString lab)   { label = lab; }
  void SetRunProperties (RunInfo);
  
  
  // Getter for delta time hist
  inline TH1D* GetDeltaTime()     {return &hDeltaTime;};
  // Getter for default calib summary histograms
  inline TH1D* GetHGped()         {return &hHGped;};
  inline TH1D* GetHGpedwidth()    {return &hHGpedwidth;};
  inline TH1D* GetLGped()         {return &hLGped;};
  inline TH1D* GetLGpedwidth()    {return &hLGpedwidth;};
  inline TH1D* GetHGScale()       {return &hHGscale;};
  inline TH1D* GetHGScalewidth()  {return &hHGscalewidth;};
  inline TH1D* GetLGScale()       {return &hLGscale;};
  inline TH1D* GetLGScaleCalc()   {return &hLGscaleCalc;};
  inline TH1D* GetLGScalewidth()  {return &hLGscalewidth;};
  inline TH1D* GetLGHGcorr()      {return &hLGHGcorr;};
  inline TH1D* GetLGHGOffcorr()   {return &hLGHGOffcorr;};
  inline TH1D* GetHGLGcorr()      {return &hHGLGcorr;};
  inline TH1D* GetHGLGOffcorr()   {return &hHGLGOffcorr;};
  
  // Getters for Comparisons to Ref run hists
  inline TH1D* GetHGpedDiffRef()        {return &hHGpedDiffRef;};
  inline TH1D* GetHGpedwidthDiffRef()   {return &hHGpedwidthDiffRef;};
  inline TH1D* GetLGpedDiffRef()        {return &hLGpedDiffRef;};
  inline TH1D* GetHGscaleDiffRef()      {return &hHGscaleDiffRef;};
  inline TH1D* GetLGscaleDiffRef()      {return &hLGscaleDiffRef;};
  inline TH1D* GetLGscaleCalcDiffRef()  {return &hLGscaleCalcDiffRef;};
  inline TH1D* GetLGHGcorrDiffRef()     {return &hLGHGcorrDiffRef;};
  inline TH2D* Get2DHGscaleCorrRef()    {return &hHGscaleCorrRef; };
  inline TProfile* GetProfHGscaleCorrRef()    {return &pHGscaleCorrRef; };
  
  inline double GetVoltage()      {return Voltage;};
  inline int GetRunNumber()       {return RunNr;};
  inline int GetRunRefNumber()    {return RunNrRef;};
  inline int GetPdg()             {return pdg;};
  inline TString GetLabel()       {return label; }
  
  TString GetLabelLegend( RunInfo commonRunInfo, int nSameSettings);
  
 protected:
  int id             ;
  int RunNr          ;
  int RunNrRef       ;
  int pdg            ;
  double Voltage     ;
  double rf          ;
  double cf          ;
  double cc          ;
  double cfcomp      ;
  double energy      ;
  double injDAC      ;
  double temp        ;
  TString label      = "";
  TH1D hLGped        ;
  TH1D hLGpedwidth   ;
  TH1D hHGped        ;
  TH1D hHGpedwidth   ;
  TH1D hLGscale      ;
  TH1D hLGscaleCalc  ;
  TH1D hLGscalewidth ;
  TH1D hHGscale      ;
  TH1D hHGscalewidth ;
  TH1D hHGLGcorr     ;
  TH1D hHGLGOffcorr  ;
  TH1D hLGHGcorr     ;
  TH1D hLGHGOffcorr  ;
  TH1D hDeltaTime    ;
  TH1D hHGpedDiffRef        ;
  TH1D hHGpedwidthDiffRef   ;
  TH1D hLGpedDiffRef        ;
  TH1D hHGscaleDiffRef      ;
  TH1D hLGscaleDiffRef      ;
  TH1D hLGscaleCalcDiffRef  ;
  TH1D hLGHGcorrDiffRef     ;
  TH2D hHGscaleCorrRef      ;
  TProfile pHGscaleCorrRef  ;
  
  ClassDef(CalibSummary,5);
};

#endif
