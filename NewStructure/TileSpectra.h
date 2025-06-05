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
      hspectraHG    = TH1D(Form("hspectra%sADCCellID%d",name.Data(),id),Form("ADC spectrum CellID %d; ADC (arb. units); counts ",id),1125,-100,1025);
      hspectraHG.SetDirectory(0);      
      hspectraLG    = TH1D(Form("hspectra%sTotCellID%d",name.Data(),id),Form("ToT spectrum CellID %d; TOT (arb. units); counts",id),4197,-100,4097);
      hspectraLG.SetDirectory(0);
      hTriggPrim    = TH1D(Form("hTriggerPrimitive%sCellID%d",name.Data(),id),Form("Trigger primitive CellID %d; ADC (arb. units); counts ",id),500,0,4000);
      hTriggPrim.SetDirectory(0);
      hspectraLGHG  = TProfile(Form("hCoorspectra%sTOTADCCellID%d",name.Data(),id),Form("TOT-ADC correlation CellID %d; TOT (arb. units); ADC (arb. units)",id),800,0,800);
      hspectraHGLG  = TProfile(Form("hCoorspectra%sADCTOTCellID%d",name.Data(),id),Form("ADC-TOT correlation CellID %d; ADC (arb. units); TOT (arb. units)",id),4097,0,4097);
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
    if (extend == 1){
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
    } else if (extend == 2){
      if (type == ReadOut::Type::Caen){
        hspectraHG    = TH1D(Form("hspectra%sHGCellID%d",name.Data(),id),Form("ADC spectrum High Gain CellID %d; Corr HG ADC (arb. units); counts ",id),1050,-200,4000);
        hspectraHG.SetDirectory(0);
        hspectraLG    = TH1D(Form("hspectra%sLGCellID%d",name.Data(),id),Form("ADC spectrum Low  Gain CellID %d; Corr LG ADC (arb. units); counts",id),1050,-200,4000);
        hspectraLG.SetDirectory(0);
        hspectraLGHG  = TProfile(Form("hCoorspectra%sLGHGCellID%d",name.Data(),id),Form("ADC Low  Gain/High Gain correlation CellID %d; Corr LG  (arb. units); HG E (arb. units)",id),400,0,400);
        hspectraLGHG.SetDirectory(0);
        hcorr         = TH2D(Form("hCoor2D%sLGHGCellID%d",name.Data(),id),Form("2D ADC Low  Gain/High Gain correlation CellID %d; Corr LG  (arb. units); HG E (arb. units)",id),400,0,400, 525, -200, 4000 );
      } else if (type == ReadOut::Type::Hgcroc){
        hspectraHG    = TH1D(Form("hspectra%sADCCellID%d",name.Data(),id),Form("ADC spectrumCellID %d; ADC (arb. units); counts ",id),1024,0,1024);
        hspectraHG.SetDirectory(0);
        hspectraLG    = TH1D(Form("hspectra%sTOTCellID%d",name.Data(),id),Form("TOT spectrumCellID %d; TOT ADC (arb. units); counts",id),4096,0,4096);
        hspectraLG.SetDirectory(0);
        hspectraLGHG  = TProfile(Form("hCoorspectra%sTOTADCGCellID%d",name.Data(),id),Form("TOT-ADC correlation CellID %d; TOT  (arb. units); ADC (arb. units)",id),400,0,400);
        hspectraLGHG.SetDirectory(0);
        hcorr         = TH2D(Form("wafeform%sCellID%d",name.Data(),id),Form("2D wafeform CellID %d; sample ; ADC (arb. units)",id),10,0,10, 1024, 0, 1024);
        hcorr.SetDirectory(0);
      }
    }
  }
  ~TileSpectra(){}

  bool Fill(double, double);
  bool FillSpectra(double, double);
  bool FillExt(double, double, double, double);
  bool FillCorr(double, double);
  bool FillTrigger(double);
  bool FillWaveform(std::vector<int>);
  
  bool FitNoise(double*, int, bool);
  void FitFixedNoise();
  void InitializeNoiseFitsFromCalib();
  bool FitMipHG(double*, double*, int, int, bool, double, double );
  bool FitMipLG(double*, double*, int, int, bool, double );
  bool FitCorr(int);
  bool FitLGHGCorr(int , bool);
  bool FitNoiseWithBG(double*);
  short DetermineBadChannel();

  int GetCellID();
  void SetBadChannelInCalib(short);
  
  double GetMaxXInRangeLG(double, double);
  double GetMaxXInRangeHG(double, double);

  TH1D* GetHG();
  TH1D* GetLG();
  TH1D* GetComb();
  TH1D* GetTriggPrim();
  TProfile* GetLGHGcorr();
  TProfile* GetHGLGcorr();
  TH2D* GetCorr();
  
  TF1* GetBackModel(int);
  TF1* GetSignalModel(int);
  TF1* GetCorrModel(int);
  TileCalib* GetCalib();
  
  void Write(bool);
  void WriteExt(bool);
  
 protected:
  TString TileName;
  ReadOut::Type ROType;
  int cellID;
  TileCalib* calib;
  int debug;
  int extend = 0;
  bool bpedHG;
  bool bpedLG;
  bool bmipHG;
  bool bmipLG;
  bool bcorrHGLG;
  bool bcorrLGHG;
  bool bTriggPrim;
  TF1 BackgroundLG;
  TF1 BackgroundHG;
  TF1 SignalLG;
  TF1 SignalHG;
  TF1 HGLGcorr;
  TF1 LGHGcorr;
  TH1D hspectraHG;
  TH1D hspectraLG;
  TH1D hTriggPrim;
  TH1D hcombined;
  TProfile hspectraLGHG;
  TProfile hspectraHGLG;
  TH2D hcorr;
  static double langaufun(double */*x*/, double */*par*/);
  static int langaupro(double */*params*/, double &/*maxx*/, double &/*FWHM*/);

  ClassDef(TileSpectra,2);
};


#endif
