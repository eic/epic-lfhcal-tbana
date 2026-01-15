#ifndef TILESPECTRA_H
#define TILESPECTRA_H

#include "TObject.h"
#include "TString.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TF1.h"
#include "TPad.h" 
#include "TCanvas.h"
#include "TLegend.h"
#include "Calib.h"
#include "Tile.h"

class TileSpectra: public TObject{

 public:
  TileSpectra():TObject(){}
  TileSpectra(TString name, int id, TileCalib* cal, ReadOut::Type type, int deb=0):TObject()
  {
    TileName      = name;
    ROType        = type;
    cellID        = id;
    calib         = cal;
    debug         = deb;
    bpedHG        = false;
    bpedLG        = false;
    bmipHG        = false;
    bmipLG        = false;
    bcorrHGLG     = false;
    bcorrLGHG     = false;
    bpedWave      = false;
    bwave         = false;
    if (ROType == ReadOut::Type::Caen){
      hspectraHG    = TH1D(Form("hspectra%sHGCellID%d",name.Data(),id),Form("ADC spectrum High Gain CellID %d; HG ADC (arb. units); counts ",id),4200,-200,4000);
      hspectraHG.SetDirectory(0);
      hspectraLG    = TH1D(Form("hspectra%sLGCellID%d",name.Data(),id),Form("ADC spectrum Low  Gain CellID %d; LG ADC (arb. units); counts",id),4200,-200,4000);
      hspectraLG.SetDirectory(0);
      hTriggPrim    = TH1D(Form("hTriggerPrimitive%sCellID%d",name.Data(),id),Form("Trigger primitive CellID %d; HG ADC (arb. units); counts ",id),500,0,4000);
      hTriggPrim.SetDirectory(0);
      hspectraLGHG  = TProfile(Form("hCoorspectra%sLGHGCellID%d",name.Data(),id),Form("ADC Low  Gain/High Gain correlation CellID %d; LG ADC (arb. units); HG ADC (arb. units)",id),800,0,800);
      hspectraLGHG.SetDirectory(0); 
      hspectraHGLG  = TProfile(Form("hCoorspectra%sHGLGCellID%d",name.Data(),id),Form("ADC High  Gain/Low Gain correlation CellID %d; HG ADC (arb. units); LG ADC (arb. units)",id),4100,-100,4000);
      hspectraHGLG.SetDirectory(0);
    } else if (ROType == ReadOut::Type::Hgcroc){
      hspectraHG    = TH1D(Form("hspectra%sADCCellID%d",name.Data(),id),Form("ADC spectrum CellID %d; ADC (arb. units); counts ",id),1124,-100,1024);
      hspectraHG.SetDirectory(0);      
      hspectraTOT    = TH1D(Form("hspectra%sTOTCellID%d",name.Data(),id),Form("ToT spectrum CellID %d; TOT (arb. units); counts",id),4197,-100,4097);
      hspectraTOT.SetDirectory(0);
      hspectraTOA    = TH1D(Form("hspectra%sTOACellID%d",name.Data(),id),Form("ToA spectrum CellID %d; TOA (arb. units); counts",id),1124,-100,1024);
      hspectraTOA.SetDirectory(0);
      hTriggPrim    = TH1D(Form("hTriggerPrimitive%sCellID%d",name.Data(),id),Form("Trigger primitive CellID %d; ADC (arb. units); counts ",id),500,0,4000);
      hTriggPrim.SetDirectory(0);
      hADCTOT  = TProfile(Form("hCoorspectra%sTOTADCCellID%d",name.Data(),id),Form("TOT-ADC correlation CellID %d; ADC (arb. units); TOT (arb. units)",id),1124,-100,1024);
      hADCTOT.SetDirectory(0); 
      hTOAADC  = TProfile(Form("hCoorspectra%sTOAADCCellID%d",name.Data(),id),Form("ADC-TOA correlation CellID %d; TOA (arb. units); ADC (arb. units)",id),1024,0,1024);
      hTOAADC.SetDirectory(0);
    }
  }
  TileSpectra(TString name, int ext, int id, TileCalib* cal, ReadOut::Type type, int deb=0):TObject()
  {
    TileName      = name;
    extend        = ext;
    cellID        = id;
    calib         = cal;
    debug         = deb;
    ROType        = type;
    bpedHG        = false;
    bpedLG        = false;
    bmipHG        = false;
    bmipLG        = false;
    bcorrHGLG     = false;
    bcorrLGHG     = false;
    bpedWave      = false;
    bwave         = false;
    // Calibrated output
    if (extend == 1){
      if (type == ReadOut::Type::Caen){
        hspectraHG    = TH1D(Form("hspectra%sHGCellID%d",name.Data(),id),Form("ADC spectrum High Gain CellID %d; Corr HG ADC (arb. units); counts ",id),4200,-200,4000);
        hspectraHG.SetDirectory(0);
        hspectraLG    = TH1D(Form("hspectra%sLGCellID%d",name.Data(),id),Form("ADC spectrum Low  Gain CellID %d; Corr LG ADC (arb. units); counts",id),4200,-200,4000);
        hspectraLG.SetDirectory(0);
        hcombined     = TH1D(Form("hspectra%sCombCellID%d",name.Data(),id),Form("Energy CellID %d; E (mip eq./tile); counts",id),8000,-5,1000);
        hcombined.SetDirectory(0);
        hspectraLGHG  = TProfile(Form("hCoorspectra%sLGHGCellID%d",name.Data(),id),Form("ADC Low  Gain/High Gain correlation CellID %d; Corr LG  (arb. units); HG E (mip eq./tile)",id),800,0,800);
        hspectraLGHG.SetDirectory(0);
        hspectraHGLG  = TProfile(Form("hCoorspectra%sHGLGCellID%d",name.Data(),id),Form("ADC Low Gain converted CellID %d; LG (arb. units); LG ( HG eq.)- HG",id),700,-100,600);
        hspectraHGLG.SetDirectory(0);
      }
    // Detailed output after transfering the calibration
    } else if (extend == 2){
      if (type == ReadOut::Type::Caen){
        hspectraHG    = TH1D(Form("hspectra%sHGCellID%d",name.Data(),id),Form("ADC spectrum High Gain CellID %d; Corr HG ADC (arb. units); counts ",id),1050,-200,4000);
        hspectraHG.SetDirectory(0);
        hspectraLG    = TH1D(Form("hspectra%sLGCellID%d",name.Data(),id),Form("ADC spectrum Low  Gain CellID %d; Corr LG ADC (arb. units); counts",id),1050,-200,4000);
        hspectraLG.SetDirectory(0);
        hspectraLGHG  = TProfile(Form("hCoorspectra%sLGHGCellID%d",name.Data(),id),Form("ADC Low  Gain/High Gain correlation CellID %d; Corr LG  (arb. units); HG E (arb. units)",id),400,0,400);
        hspectraLGHG.SetDirectory(0);
        hcorr         = TH2D(Form("hCoor2D%sLGHGCellID%d",name.Data(),id),Form("2D ADC Low  Gain/High Gain correlation CellID %d; Corr LG  (arb. units); HG E (arb. units)",id),400,0,400, 525, -200, 4000 );
        hcorr.SetDirectory(0);
      } else if (type == ReadOut::Type::Hgcroc){
        hspectraHG    = TH1D(Form("hspectra%sADCCellID%d",name.Data(),id),Form("ADC spectrumCellID %d; ADC (arb. units); counts ",id),1124,-100,1024);
        hspectraHG.SetDirectory(0);
        hspectraTOA    = TH1D(Form("hspectra%sTOACellID%d",name.Data(),id),Form("TOA spectrumCellID %d; TOA (arb. units); counts",id),1024,0,1024);
        hspectraTOA.SetDirectory(0);
        hspectraTOT    = TH1D(Form("hspectra%sTOTCellID%d",name.Data(),id),Form("TOT spectrumCellID %d; TOT (arb. units); counts",id),4096,0,4096);
        hspectraTOT.SetDirectory(0);
        hADCTOT  = TProfile(Form("h%sTOTADCCellID%d",name.Data(),id),Form("TOT-ADC correlation CellID %d; ADC (arb. units); TOT  (arb. units)",id),1128/8,-100,1028);
        hADCTOT.SetDirectory(0);
        hcorr         = TH2D(Form("wafeform%sCellID%d",name.Data(),id),Form("2D wafeform CellID %d; sample ; ADC (arb. units)",id),20,0,20, 1034, -10, 1024);
        hcorr.SetDirectory(0);
        hcorrADCTOT   = TH2D(Form("hCorr2DTOTADC%sCellID%d",name.Data(),id),Form("2D TOT-ADC CellID %d;  ADC (arb. units); TOT  (arb. units)",id),1128/8,-100,1028,4096/8,0,4096);
        hcorrADCTOT.SetDirectory(0);
      }
    // Waveform analysis
    } else if (extend == 3){
      if (type == ReadOut::Type::Hgcroc){
        hspectraHG    = TH1D(Form("hspectra%sADCCellID%d",name.Data(),id),Form("ADC spectrumCellID %d; ADC (arb. units); counts ",id),1124,-100,1024);
        hspectraHG.SetDirectory(0);
        hspectraTOA    = TH1D(Form("hspectra%sTOACellID%d",name.Data(),id),Form("TOA spectrumCellID %d; TOA (arb. units); counts",id),1024,0,1024);
        hspectraTOA.SetDirectory(0);
        hspectraTOT    = TH1D(Form("hspectra%sTOTCellID%d",name.Data(),id),Form("TOT spectrumCellID %d; TOT (arb. units); counts",id),4096,0,4096);
        hWaveForm     = TProfile(Form("wafeform1D%sCellID%d",name.Data(),id),Form("1D wafeform CellID %d;  t (ps) ; ADC (arb. units)",id),525,-25000,500000);
        hWaveForm.SetDirectory(0);
        hcorr         = TH2D(Form("wafeform%sCellID%d",name.Data(),id),Form("2D wafeform CellID %d; t (ps) ; ADC (arb. units)",id),525,-25000,500000, 1034, -10, 1024);
        hcorr.SetDirectory(0);
        hcorrTOAADC   = TH2D(Form("h2DADCTOACellID%sCellID%d",name.Data(),id),Form("2D ADC vs TOA CellID %d; TOA (arb. units); ADC (arb. units)",id), 1024/4,0,1024,1124,-100,1024);
        hcorrTOAADC.SetDirectory(0);
        hcorrTOASample = TH2D(Form("h2DTOASampleCellID%sCellID%d",name.Data(),id),Form("2D Sample vs TOA CellID %d; TOA (arb. units); #sample",id), 1024/4,0,1024,20,0,20);
        hcorrTOASample.SetDirectory(0);
        hTOAADC       = TProfile(Form("h%sADCTOTCellID%d",name.Data(),id),Form("ADC-TOA correlation CellID %d; TOA (arb. units); ADC (arb. units)",id),1024,0,1024);
        hTOAADC.SetDirectory(0);
      }
    // extended pedestal output
    } else if (extend == 4){
      if (type == ReadOut::Type::Hgcroc){
        hspectraHG    = TH1D(Form("hspectra%sADCCellID%d",name.Data(),id),Form("ADC spectrumCellID %d; ADC (arb. units) 1st sample; counts ",id),1124,-100,1024);
        hspectraHG.SetDirectory(0);
        hspectraLG    = TH1D(Form("hspectra%sADCAllCellID%d",name.Data(),id),Form("ADC spectrumCellID %d; ADC (arb. units) all samples; counts ",id),1124,-100,1024);
        hspectraLG.SetDirectory(0);
        hcorr         = TH2D(Form("wafeform%sCellID%d",name.Data(),id),Form("2D wafeform CellID %d; sample ; ADC (arb. units)",id),20,0,20, 1034, -10, 1024);
        hcorr.SetDirectory(0);
      }
    }
  }
  ~TileSpectra(){}

  bool FillCAEN(double, double);
  bool FillSpectraCAEN(double, double);
  bool FillExtCAEN(double, double, double, double);
  bool FillCorrCAEN(double, double);

  bool FillHGCROC(double, double, double);
  bool FillSpectraHGCROC(double, double, double);
  bool FillExtHGCROC(double, double, double, int);
  bool FillExtHGCROCPed(std::vector<int>, double);
  bool FillCorrHGCROC(double, double, double);

  bool FillTrigger(double);
  bool FillWaveform(std::vector<int>,double);
  bool FillWaveformVsTime(std::vector<int> , double, double, int );
  bool FillWaveformVsTimeParser(std::vector<int>,double);
  
  bool FitNoise(double*, int, bool);
  void FitFixedNoise();
  void InitializeNoiseFitsFromCalib();
  bool FitMipHG(double*, double*, int, int, bool, double, double );
  bool FitMipLG(double*, double*, int, int, bool, double );
  bool FitCorrCAEN(int);
  bool FitLGHGCorr(int , bool);
  bool FitPedConstWave(int);
  bool FitNoiseWithBG(double*);
  short DetermineBadChannel();

  int GetCellID();
  void SetBadChannelInCalib(short);
  
  double GetMaxXInRangeLG(double, double);
  double GetMaxXInRangeHG(double, double);

  ReadOut::Type GetROType() {return ROType;};  
  TH1D* GetHG();
  TH1D* GetLG();
  TH1D* GetComb();
  TH1D* GetTriggPrim();
  TProfile* GetLGHGcorr();
  TProfile* GetHGLGcorr();
  TH2D* GetCorr();
  // HGCROC specific
  TH1D* GetTOT();
  TH1D* GetTOA();
  TProfile* GetWave1D();
  TProfile* GetTOAADC();
  TProfile* GetADCTOT();
  TH2D* GetCorrTOAADC();
  TH2D* GetCorrTOASample();
  TH2D* GetCorrADCTOT();
  
  TF1* GetBackModel(int);
  TF1* GetSignalModel(int);
  TF1* GetCorrModel(int);
  TileCalib* GetCalib();
  
  void Write(bool);
  void WriteExt(bool);
  
 protected:
  TString TileName      = "";
  ReadOut::Type ROType;
  int cellID            = -1;
  TileCalib* calib;
  int debug             = 0;
  int extend            = 0;
  bool resetAxisLabels  = false;
  bool bpedHG           = false;
  bool bpedLG           = false;
  bool bmipHG           = false;
  bool bmipLG           = false;
  bool bcorrHGLG        = false;
  bool bcorrLGHG        = false;
  bool bpedWave         = false;
  bool bwave            = false;
  bool bTriggPrim       = false;
  TF1 BackgroundLG;
  TF1 BackgroundHG;
  TF1 SignalLG;
  TF1 SignalHG;
  TF1 HGLGcorr;
  TF1 LGHGcorr;
  TF1 pedWave;
  TF1 wave;
  TH1D hspectraHG;
  TH1D hspectraLG;
  TH1D hspectraTOT;     // only in HGCROC case
  TH1D hspectraTOA;     // only in HGCROC case
  TH1D hTriggPrim;
  TH1D hcombined;
  TProfile hspectraLGHG;
  TProfile hspectraHGLG;
  TProfile hWaveForm;   // only in HGCROC case
  TProfile hADCTOT;
  TProfile hTOAADC;
  TH2D hcorr; 
  TH2D hcorrTOAADC;     // only in HGCROC case
  TH2D hcorrTOASample;  // only in HGCROC case
  TH2D hcorrADCTOT;     // only in HGCROC case
  static double langaufun(double */*x*/, double */*par*/);
  static int langaupro(double */*params*/, double &/*maxx*/, double &/*FWHM*/);

  ClassDef(TileSpectra,3);
};


#endif
