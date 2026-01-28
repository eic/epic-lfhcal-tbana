#ifndef HGCROC_WAVEFORM_ANALYSIS_H
#define HGCROC_WAVEFORM_ANALYSIS_H

#include <iostream>
#include <fstream>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TObjString.h"

#include "Setup.h"
#include "RootSetupWrapper.h"
#include "Calib.h"
#include "Event.h"
#include "Tile.h"
#include "HGCROC.h"
#include "Caen.h"
    
class HGCROC_Waveform_Analysis{

 public:
  HGCROC_Waveform_Analysis(){
    eventptr=&event;
    rswptr=&rsw;
    calibptr=&calib;
  }
  ~HGCROC_Waveform_Analysis(){}

  //Getter methods
  inline TString GetRootCalibInputName()    const {return RootCalibInputName;};
  inline TString GetRootCalibOutputName()   const {return RootCalibOutputName;};
  inline TString GetRootInputName()         const {return RootInputName;};
  inline TString GetRootOutputName()        const {return RootOutputName;};
  inline TString GetPlotOutputDir()         const {return OutputNameDirPlots;};
  inline TString GetExternalBadChannelMap() const {return ExternalBadChannelMap;};
  inline TString GetExternalCalibFile()     const {return ExternalCalibFile;};
  
  inline TFile* GetRootCalibInput()    {return RootCalibInput;}
  inline TFile* GetRootInput()         {return RootInput;}
  inline TFile* GetRootOutput()        {return RootOutput;}

  inline bool CanOverWrite(void)                const {return Overwrite;};
  inline bool IsToAnalysisWaveForm(void)        const {return IsAnalyseWaveForm;};
  inline bool IsHGCROC(void)                    const {return HGCROC;};
  
  inline short GetExtPlotting(void)             const {return ExtPlot;};
  inline bool GetOverWriteCalib(void)           const {return OverWriteCalib;};
  inline int GetMaxEvents(void)                 const {return maxEvents;};
  inline int GetFixedTOASample(void)            const {return fixedTOASample;};
  
  //setter methods
  //Overload method for boolean...or is it too dangerous?
  inline void CanOverWrite(bool b)               {Overwrite=b;};
  inline void IsToAnalysisWaveForm(bool b)       {IsAnalyseWaveForm=b;};
  inline void SetOverWriteCalib(bool b)          {OverWriteCalib = b;};
  inline void SetExtPlotting(short b)            {ExtPlot = b;};
  inline void EnableDebug(int i)                 {debug=i;};  
  
  inline void SetYear(int year)                  {yearData=year;};
  inline void SetRunListInput(TString name)      {RunListInputName=name;};
  inline void SetRootCalibInput(TString name)    {RootCalibInputName=name;};
  inline void SetRootCalibOutput(TString name)   {RootCalibOutputName=name;};
  inline void SetRootInput(TString name)         {RootInputName=name;};
  inline void SetRootOutput(TString name)        {RootOutputName =name;};
  inline void SetRootOutputHists(TString name)   {RootOutputNameHist =name;};
  inline void SetPlotOutputDir(TString name)     {OutputNameDirPlots =name;};
  inline void SetPlotExtension(TString name)     {plotSuffix = name;};
  inline void SetExternalBadChannelMap(TString name)     {ExternalBadChannelMap =name;};
  inline void SetExternalCalibFile(TString name)     {ExternalCalibFile =name;};
  inline void SetMaxEvents(int n)                 {maxEvents = n;};
  inline void SetFixedTOASample(int s)            {fixedTOASample = s;};
  
  //General methods
  bool CreateOutputRootFile(void);
  bool CheckAndOpenIO(void);
  bool Process(void);

  //Variable members
  TString RootOutputName;                 // file name of root output with tree
  TString RootOutputNameHist;             // file name of root output with additional histograms & fits
  TString OutputNameDirPlots;             // directory name of output for plots
  TString RootInputName;                  // file name of input root file 
  TString RootCalibInputName;             // file name of calibration root file (mip calib)
  TString RootCalibOutputName;            // file name of calibration root file (mip calib) output
  TString RunListInputName;               // file name run list 
  TString ExternalBadChannelMap;          // file name external bad channel map
  TString ExternalCalibFile;              // file name external calib file
  TString plotSuffix        = "pdf";      // plot extension
  TFile* RootOutput         =nullptr;     // root file output tree
  TFile* RootOutputHist     =nullptr;     // root file output histos
  TFile* RootInput          =nullptr;     // root file input 
  TFile* RootCalibInput     =nullptr;     // root file calib values (mip)
  TFile* RootCalibOutput    =nullptr;     // root file calib values (mip) output
  bool HGCROC                 =false;     // Flag for HGCROC data conversion
  bool IsAnalyseWaveForm      =false;     // Flag for routine with waveform analysis
  bool OverWriteCalib         =false;     // Flag to overwrite calib from text file
  short ExtPlot               =0;         // Enable extended plotting
  bool Overwrite              =false;     // Flag to overwrite outputs
  int debug                   =0;         // debug level 
  int yearData                =-1;        // data taking year externally set
  RootSetupWrapper rsw;                   // Wrapper singleton class for setup
  RootSetupWrapper* rswptr;               // Pointer to wrapper for singleton class for setup
  Setup* setup;                           // geometry setup
  Calib calib;                            // calibration object
  Calib* calibptr;                        // pointer to calib object
  Event event;
  Event* eventptr;
  int maxEvents               = -1;
  int fixedTOASample          = -1;
  
  TTree* TsetupIn=nullptr;
  TTree* TsetupOut=nullptr;
  TTree* TdataIn=nullptr;
  TTree* TdataOut=nullptr;
  TTree* TcalibIn=nullptr;
  TTree* TcalibOut=nullptr;

 protected:


 private:
  bool AnalyseWaveForm(void);
 };


#endif
