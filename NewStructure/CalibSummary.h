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
    rotype            = optHGCROC;
    hHGped            = TH1D(Form("hMeanPedHG_%i",id),"; #mu_{noise, HG} (arb. units); counts ", 500, -0.5, 500-0.5);
    hHGpedwidth       = TH1D(Form("hMeanPedHGwidth_%i",id),"; #sigma_{noise, HG} (arb. units); counts ", 400, -0.5*50/400, 50-(0.5*50/400));
    hLGped            = TH1D(Form("hMeanPedLG_%i",id),"; #mu_{noise, LG} (arb. units); counts ", 500, -0.5, 500-0.5);
    hLGpedwidth       = TH1D(Form("hMeanPedLGwidth_%i",id),"; #sigma_{noise, LG} (arb. units); counts ", 400, -0.5*20/400, 20-(0.5*20/400));
    hHGscale          = TH1D(Form("hHGScale_%i",id),";Max_{HG} (arb. units) ; counts ", 2000, -0.25, 1000-0.25);
    hHGscalewidth     = TH1D(Form("hHGScalewidth_%i",id),";Width_{HG} (arb. units) ; counts ", 2000, -0.25, 1000-0.25);
    hHGpedDiffRef     = TH1D(Form("hDiffPedvsRefHG_%i",id),"; #mu_{noise, HG}-#mu_{noise, HG, ref run} (arb. units); counts ", 501, -100, 100);
    hHGpedwidthDiffRef= TH1D(Form("hDiffPedWidthvsRefHG_%i",id),"; #sigma_{noise, HG}-#sigma_{noise, HG, ref run} (arb. units); counts ", 501, -100, 100);
    hLGpedDiffRef     = TH1D(Form("hDiffPedvsRefLG_%i",id),"; #mu_{noise, LG}-#mu_{noise, LG, ref run} (arb. units); counts ", 501, -100, 100);
    hHGscaleDiffRef   = TH1D(Form("hDiffHGScalevsRefHG_%i",id),"; Max_{HG}-Max_{HG,ref run} (arb. units); counts ", 1001, -250, 250);

    if (optHGCROC > 0){
      hHGscaleCorrRef   = TH2D(Form("hHGscaleCorrRef_%i",id),";Max_{ADC} (arb. units); Max_{ADC, ref run} (arb. units); Max_{ADC, ref run} (arb. units) ; counts ", 350, -0.25, 350-0.25, 350, -0.25, 350-0.25);
      pHGscaleCorrRef   = TProfile(Form("pHGscaleCorrRef_%i",id),";Max_{ADC} (arb. units); Max_{ADC, ref run} (arb. units)", 350, -0.25, 350-0.25);
    } else {
      hLGscale          = TH1D(Form("hLGScale_%i",id),";Max_{LG} (arb. units) ; counts ", 2000, -0.5*250/2000, 250-(0.5*250/2000));
      hLGscaleCalc      = TH1D(Form("hLGScaleCalc_%i",id),";Max_{LG,calc} (arb. units) ; counts ", 2000, -0.5*250/2000, 250-(0.5*250/2000));
      hLGscalewidth     = TH1D(Form("hHGScalewidth_%i",id),";Width_{LG} (arb. units) ; counts ", 2000, -0.5*250/2000, 250-(0.5*250/2000));
      hLGHGcorr         = TH1D(Form("hLGHGCorr_%i",id),"; a_{LG-HG} (arb. units) ; counts ", 400, 0, 20);
      hLGHGOffcorr      = TH1D(Form("hLGHGOffCorr_%i",id),"; b_{LG-HG} (arb. units) ; counts ", 1000, -200, 100);
      hHGLGcorr         = TH1D(Form("hHGLGCorr_%i",id),"; a_{HG-LG} (arb. units) ; counts ", 400, 0., 1.);
      hHGLGOffcorr      = TH1D(Form("hHGLGOffCorr_%i",id),"; b_{HG-LG} (arb. units) ; counts ", 1000, -100., 100.);
    
      hHGscaleCorrRef   = TH2D(Form("hHGscaleCorrRef_%i",id),";Max_{HG} (arb. units); Max_{HG, ref run} (arb. units) ; counts ", 1000, -0.25, 1000-0.25, 1000, -0.25, 1000-0.25);
      pHGscaleCorrRef   = TProfile(Form("pHGscaleCorrRef_%i",id), ";Max_{HG} (arb. units); Max_{HG, ref run} (arb. units)", 1000, -0.25, 1000-0.25);
      hLGscaleCorrRef   = TH2D(Form("hLGscaleCorrRef_%i",id),";Max_{LG} (arb. units); Max_{LG, ref run} (arb. units) ; counts ", 250, -0.25, 250-0.25, 250, -0.25, 250-0.25);
      pLGscaleCorrRef   = TProfile(Form("pLGscaleCorrRef_%i",id), ";Max_{LG} (arb. units); Max_{lG, ref run} (arb. units)", 250, -0.25, 250-0.25);
      hLGscaleDiffRef   = TH1D(Form("hDiffLGScalevsRefHG_%i",id),"; Max_{LG}-Max_{LG,ref run} (arb. units); counts ", 501, -100, 100);
      hLGscaleCalcDiffRef   = TH1D(Form("hDiffLGScaleCalcvsRefHG_%i",id),"; Max_{LG,calc}-Max_{LG,calc,ref run} (arb. units); counts ", 501, -100, 100);
      hLGHGcorrDiffRef  = TH1D(Form("hDiffLGScalevsRefHG_%i",id),"; Max_{LG}-Max_{LG,ref run} (arb. units); counts ", 501, -10, 10);

      hLGscaleCalcDiffLGscale       = TH1D(Form("hLGscaleCalcDiffLGscale%i",id),";Max_{LG,calc}/Max_{LG}; counts ", 200, 0, 2);
      hLGscaleCalcAlterDiffLGscale  = TH1D(Form("hLGscaleCalcAlterDiffLGscale%i",id),";Max_{LG,calc,alter}/Max_{LG}; counts ", 200, 0, 2);
      hLGscaleDiffHGscale           = TH1D(Form("hLGscaleDiffHGscale%i",id),";Max_{LG}/Max_{HG}; counts ", 200, 0, 1);
    }
    
  }

  CalibSummary(int id, int RunNum, int layers, double v, int p = 0,  int optHGCROC = 0):TObject()
  {
    std::cout << "Initializing with layer histos: " << RunNum << "\t" << layers << std::endl;
    
    RunNr             = RunNum;
    RunNrRef          = RunNum;
    Voltage           = v;
    pdg               = p;
    rotype            = optHGCROC;
    hHGped            = TH1D(Form("hMeanPedHG_%i",id),"; #mu_{noise, HG} (arb. units); counts ", 500, -0.5, 500-0.5);
    hHGpedwidth       = TH1D(Form("hMeanPedHGwidth_%i",id),"; #sigma_{noise, HG} (arb. units); counts ", 400, -0.5*50/400, 50-(0.5*50/400));
    hLGped            = TH1D(Form("hMeanPedLG_%i",id),"; #mu_{noise, LG} (arb. units); counts ", 500, -0.5, 500-0.5);
    hLGpedwidth       = TH1D(Form("hMeanPedLGwidth_%i",id),"; #sigma_{noise, LG} (arb. units); counts ", 400, -0.5*20/400, 20-(0.5*20/400));
    hHGscale          = TH1D(Form("hHGScale_%i",id),";Max_{HG} (arb. units) ; counts ", 2000, -0.25, 1000-0.25);
    hHGscalewidth     = TH1D(Form("hHGScalewidth_%i",id),";Width_{HG} (arb. units) ; counts ", 2000, -0.25, 1000-0.25);
    
    hHGpedDiffRef     = TH1D(Form("hDiffPedvsRefHG_%i",id),"; #mu_{noise, HG}-#mu_{noise, HG, ref run} (arb. units); counts ", 501, -100, 100);
    hHGpedwidthDiffRef= TH1D(Form("hDiffPedWidthvsRefHG_%i",id),"; #sigma_{noise, HG}-#sigma_{noise, HG, ref run} (arb. units); counts ", 501, -100, 100);
    hLGpedDiffRef     = TH1D(Form("hDiffPedvsRefLG_%i",id),"; #mu_{noise, LG}-#mu_{noise, LG, ref run} (arb. units); counts ", 501, -100, 100);
    hHGscaleDiffRef   = TH1D(Form("hDiffHGScalevsRefHG_%i",id),"; Max_{HG}-Max_{HG,ref run} (arb. units); counts ", 1001, -250, 250);
    
    if (optHGCROC > 0){
      hHGscaleCorrRef   = TH2D(Form("hHGscaleCorrRef_%i",id),";Max_{ADC} (arb. units); Max_{ADC, ref run} (arb. units); Max_{ADC, ref run} (arb. units) ; counts ", 350, -0.25, 350-0.25, 350, -0.25, 350-0.25);
      pHGscaleCorrRef   = TProfile(Form("pHGscaleCorrRef_%i",id),";Max_{ADC} (arb. units); Max_{ADC, ref run} (arb. units)", 350, -0.25, 350-0.25);
    } else {
      hLGscale          = TH1D(Form("hLGScale_%i",id),";Max_{LG} (arb. units) ; counts ", 2000, -0.5*250/2000, 250-(0.5*250/2000));
      hLGscaleCalc      = TH1D(Form("hLGScaleCalc_%i",id),";Max_{LG,calc} (arb. units) ; counts ", 2000, -0.5*250/2000, 250-(0.5*250/2000));
      hLGscalewidth     = TH1D(Form("hHGScalewidth_%i",id),";Width_{LG} (arb. units) ; counts ", 2000, -0.5*250/2000, 250-(0.5*250/2000));
      hLGHGcorr         = TH1D(Form("hLGHGCorr_%i",id),"; a_{LG-HG} (arb. units) ; counts ", 400, 0, 20);
      hLGHGOffcorr      = TH1D(Form("hLGHGOffCorr_%i",id),"; b_{LG-HG} (arb. units) ; counts ", 1000, -200, 100);
      hHGLGcorr         = TH1D(Form("hHGLGCorr_%i",id),"; a_{HG-LG} (arb. units) ; counts ", 400, 0., 1.);
      hHGLGOffcorr      = TH1D(Form("hHGLGOffCorr_%i",id),"; b_{HG-LG} (arb. units) ; counts ", 1000, -100., 100.);

      hHGscaleCorrRef     = TH2D(Form("hHGscaleCorrRef_%i",id),";Max_{HG} (arb. units); Max_{HG, ref run} (arb. units) ; counts ", 1000, -0.25, 1000-0.25, 1000, -0.25, 1000-0.25);
      pHGscaleCorrRef     = TProfile(Form("pHGscaleCorrRef_%i",id), ";Max_{HG} (arb. units); Max_{HG, ref run} (arb. units)", 1000, -0.25, 1000-0.25);
      hLGscaleCorrRef     = TH2D(Form("hLGscaleCorrRef_%i",id),";Max_{LG} (arb. units); Max_{LG, ref run} (arb. units) ; counts ", 250, -0.25, 250-0.25, 250, -0.25, 250-0.25);
      pLGscaleCorrRef     = TProfile(Form("pLGscaleCorrRef_%i",id), ";Max_{LG} (arb. units); Max_{LG, ref run} (arb. units)", 250, -0.25, 250-0.25);
      hLGscaleDiffRef     = TH1D(Form("hDiffLGScalevsRefHG_%i",id),"; Max_{LG}-Max_{LG,ref run} (arb. units); counts ", 501, -100, 100);
      hLGscaleCalcDiffRef = TH1D(Form("hDiffLGScaleCalcvsRefHG_%i",id),"; Max_{LG,calc}-Max_{LG,calc,ref run} (arb. units); counts ", 501, -100, 100);
      hLGHGcorrDiffRef    = TH1D(Form("hDiffLGScalevsRefHG_%i",id),"; Max_{LG}-Max_{LG,ref run} (arb. units); counts ", 501, -10, 10);
      
      hLGscaleCalcDiffLGscale       = TH1D(Form("hLGscaleCalcDiffLGscale%i",id),";Max_{LG,calc}/Max_{LG}; counts ", 800, 0, 2);
      hLGscaleCalcAlterDiffLGscale  = TH1D(Form("hLGscaleCalcAlterDiffLGscale%i",id),";Max_{LG,calc,alter}/Max_{LG}; counts ", 800, 0, 2);
      hLGscaleDiffHGscale           = TH1D(Form("hLGscaleDiffHGscale%i",id),";Max_{LG}/Max_{HG}; counts ", 400, 0, 0.5);
    }    
    
    for (int l = 0; l < layers; l++){
      TH1D tempHScale   = TH1D(Form("hHGScale_layer_%d_%i",l,id),
                               ";Max_{HG} (arb. units) ; counts ", 1000, -0.25, 1000-0.25);
      hHGscaleLayer[l]  = tempHScale;
      TH1D tempHScaleWidth   =  TH1D(Form("hHGScalewidth_layer_%d_%i",l,id),
                                     ";Width_{HG} (arb. units) ; counts ", 1000, -0.25, 1000-0.25);
      hHGscalewidthLayer[l]  = tempHScaleWidth;
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
  
  bool FillLayerProps(const TileCalib&, int);
  bool FillLayerProps(TileCalib*, int);
  
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
  
  // Getters for CAEN only
  inline TH1D* GetLGScale()       {return &hLGscale;};
  inline TH1D* GetLGScaleCalc()   {return &hLGscaleCalc;};
  inline TH1D* GetLGScalewidth()  {return &hLGscalewidth;};
  inline TH1D* GetLGHGcorr()      {return &hLGHGcorr;};
  inline TH1D* GetLGHGOffcorr()   {return &hLGHGOffcorr;};
  inline TH1D* GetHGLGcorr()      {return &hHGLGcorr;};
  inline TH1D* GetHGLGOffcorr()   {return &hHGLGOffcorr;};
  inline TH1D* GetLGScaleCalcDiffLGScale()      {return &hLGscaleCalcDiffLGscale;};
  inline TH1D* GetLGScaleCalcAlterDiffLGScale() {return &hLGscaleCalcAlterDiffLGscale;};
  inline TH1D* GetLGScaleDiffHGScale()          {return &hLGscaleDiffHGscale;};
  
  // Getters for Comparisons to Ref run hists
  inline TH1D* GetHGpedDiffRef()        {return &hHGpedDiffRef;};
  inline TH1D* GetHGpedwidthDiffRef()   {return &hHGpedwidthDiffRef;};
  inline TH1D* GetHGscaleDiffRef()      {return &hHGscaleDiffRef;};
  inline TH2D* Get2DHGscaleCorrRef()    {return &hHGscaleCorrRef; };
  inline TProfile* GetProfHGscaleCorrRef()    {return &pHGscaleCorrRef; };
  TH1D* GetHGScaleLayer(int )  ;
  TH1D* GetHGScalewidthLayer(int );

  // Getters for Comparisons to Ref run hists CAEN only
  inline TH1D* GetLGpedDiffRef()        {return &hLGpedDiffRef;};
  inline TH1D* GetLGscaleDiffRef()      {return &hLGscaleDiffRef;};
  inline TH1D* GetLGscaleCalcDiffRef()  {return &hLGscaleCalcDiffRef;};
  inline TH1D* GetLGHGcorrDiffRef()     {return &hLGHGcorrDiffRef;};
  inline TH2D* Get2DLGscaleCorrRef()    {return &hLGscaleCorrRef; };
  inline TProfile* GetProfLGscaleCorrRef()    {return &pLGscaleCorrRef; };
  
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
  int rotype         ;
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
  TH1D hHGscale      ;
  TH1D hHGscalewidth ;
  TH1D hHGpedDiffRef        ;
  TH1D hHGpedwidthDiffRef   ;
  TH1D hHGscaleDiffRef      ;
  TH2D hHGscaleCorrRef      ;
  TProfile pHGscaleCorrRef  ;

  TH1D hDeltaTime    ;
  
  // CAEN only
  TH1D hLGscale      ;
  TH1D hLGscaleCalc  ;
  TH1D hLGscaleCalcDiffLGscale  ;
  TH1D hLGscaleCalcAlterDiffLGscale  ;
  TH1D hLGscaleDiffHGscale  ;
  TH1D hLGscalewidth ;
  TH1D hHGLGcorr     ;
  TH1D hHGLGOffcorr  ;
  TH1D hLGHGcorr     ;
  TH1D hLGHGOffcorr  ;

  TH1D hLGpedDiffRef        ;
  TH1D hLGscaleDiffRef      ;
  TH1D hLGscaleCalcDiffRef  ;
  TH1D hLGHGcorrDiffRef     ;
  TH2D hLGscaleCorrRef      ;
  TProfile pLGscaleCorrRef  ;
  
  
  std::map<int, TH1D> hHGscaleLayer;
  std::map<int, TH1D> hHGscalewidthLayer;
  
  ClassDef(CalibSummary,6);
};

#endif
