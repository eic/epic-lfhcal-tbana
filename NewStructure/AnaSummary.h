#ifndef ANASUMMARY_H
#define ANASUMMARY_H

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

class AnaSummary: public TObject{

 public:
 AnaSummary():TObject(){}
 AnaSummary(int id, int RunNum, double v, double e, int pdg, int ext = 0):TObject()
 {
    RunNr             = RunNum;
    Voltage           = v;
    Pdg               = pdg;
    Energy            = e;
  }
  ~AnaSummary(){}

  bool Analyse();
  bool Fill(const TileCalib&);
  bool Write(TFile*);
  bool SetDeltaTimeHist(TH1D*);
  bool SetEnergyHist(TH1D*);
  bool SetNCellsHist(TH1D*);

  inline TH1D* GetDeltaTimeHist()     {return &hDeltaTime;};
  inline TH1D* GetEnergyHist()        {return &hEnergy;};
  inline TH1D* GetNCellsHist()        {return &hNCells;};
  
  inline double GetVoltage()      {return Voltage;};
  inline double GetEnergy()       {return Energy;};
  inline int GetPDG()             {return Pdg;};
  inline int GetRunNumber()       {return RunNr;};
  
 protected:
  int id             ;
  int RunNr          ;
  double Voltage     ;
  double Energy      ;
  int Pdg            ;
  TH1D hDeltaTime    ;
  TH1D hEnergy       ;
  TH1D hNCells       ;

  ClassDef(AnaSummary,2);
};

#endif
