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

class CalibSummary: public TObject{

 public:
 CalibSummary():TObject(){}
 CalibSummary(int id,int RunNum, double v, int ext = 0):TObject()
 {
    RunNr             = RunNum;
    Voltage           = v;
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
  }
  ~CalibSummary(){}

  int Analyse(int );
  bool Fill(const TileCalib&);
  bool Write(TFile*);
  bool SetDeltaTimeHist(TH1D*);
  
  inline TH1D* GetDeltaTime()     {return &hDeltaTime;};
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
  inline double GetVoltage()      {return Voltage;};
  inline int GetRunNumber()       {return RunNr;};
  
 protected:
  int id             ;
  int RunNr          ;
  double Voltage     ;
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
  
  ClassDef(CalibSummary,2);
};

#endif
