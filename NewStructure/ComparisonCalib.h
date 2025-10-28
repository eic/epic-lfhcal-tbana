#ifndef COMPARISONCALIB_H
#define COMPARISONCALIB_H

#include <iostream>
#include <fstream>
#include "TString.h"
#include "TFile.h"
#include "TChain.h"
#include "TObjString.h"

#include "Setup.h"
#include "RootSetupWrapper.h"
#include "Calib.h"
#include "Event.h"
#include "Tile.h"
#include "HGCROC.h"
#include "Caen.h"
    
class ComparisonCalib{

 public:
  ComparisonCalib(){
    TsetupIn = new TChain("Setup");
    TcalibIn = new TChain("Calib");
    rswptr=&rsw;
    calibptr=&calib;
  }
  ~ComparisonCalib(){
    if(RootOutput) RootOutput->Close();
  }

  //Getter methods
  inline TString GetRootCalibInputName()    const {return TcalibIn->GetFile()->GetName();};
  inline TString GetInputListName()         const {return InputListName;};
  inline TString GetRootOutputName()        const {return RootOutputName;};
  inline TString GetPlotOutputDir()         const {return OutputNameDirPlots;};
  inline short GetExtPlotting(void)         const {return ExtPlot;};
  inline bool GetIsHGCROC(void)             const {return isHGCROC;};
  inline int GetExpandedList()              const {return expandedList;}
  inline bool CanOverWrite(void)            const {return Overwrite;};
  
  inline TFile* GetRootOutput()                   {return RootOutput;}
  
  //setter methods
  //Overload method for boolean...or is it too dangerous?
  inline void CanOverWrite(bool b)               {Overwrite=b;};
  inline void EnableDebug(int i)                 {debug=i;};
  inline void ExpandedList(int i)               {expandedList=i;};  
  
  inline void AddInputFile(TString name)         {RootInputNames.push_back(name);};
  inline void SetInputList(TString name)         {InputListName=name;};
  inline void SetRootOutput(TString name)        {RootOutputName =name;};
  inline void SetPlotOutputDir(TString name)     {OutputNameDirPlots =name;};
  inline void SetPlotExtension(TString name)     {plotSuffix = name;};
  inline void SetExtPlotting(short b)            {ExtPlot = b;};
  inline void SetTrendingAxis(int i)             {Xaxis=i;};
  inline void SetRunListInput(TString name)      {RunListInputName=name;};
  inline void SetMaxPlotLayer(int i)             {maxLayerPlot=i;};
  inline void SetIsHGCROC(bool b)                {isHGCROC=b;};
  
  //General methods
  bool CreateOutputRootFile(void);
  bool CheckAndOpenIO(void);
  bool ProcessCalib(void);

  //Variable members
  TString RootOutputName;                 // file name of root output with tree
  std::vector<TString> RootInputNames;    // file name of root output with tree
  //TString RootOutputNameHist;             // file name of root output with additional histograms & fits
  TString OutputNameDirPlots;             // directory name of output for plots
  TString plotSuffix        = "pdf";      // output file format for plots
  TString InputListName;                  // file name of input file list 
  TString RunListInputName;               // file name run list 
  TFile* RootOutput         =nullptr;     // root file output tree
  bool Overwrite              =false;     // Flag to overwrite outputs
  int expandedList            =0;         // Expanded input list option: 1 - ImprovedScaling histo output, 2 - GetScaling histo output
  int debug                   =0;         // debug level 
  short ExtPlot               =0;         // Enable extended plotting
  int yearData                =-1;        // data taking year externally set
  int Xaxis                   =0;         // Trending dependence 0: run nr, 1: Voltage
  int maxLayerPlot            =100;       // option restrict individual layer plotting
  bool isHGCROC               =false;     // option to switch to HGCROC comparison
  RootSetupWrapper rsw;                   // Wrapper singleton class for setup
  RootSetupWrapper* rswptr;               // Pointer to wrapper for singleton class for setup
  Setup* setup;                           // geometry setup
  Calib calib;                            // calibration object
  Calib* calibptr;                        // pointer to calib object
  
  TChain* TsetupIn=nullptr;
  TChain* TcalibIn=nullptr;

 protected:


 private:
   
};

#endif
