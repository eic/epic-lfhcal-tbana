#ifndef ANALYSES_H
#define ANALYSES_H

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
    
class Analyses{

 public:
  Analyses(){
    eventptr=&event;
    rswptr=&rsw;
    calibptr=&calib;
  }
  ~Analyses(){}

  //Getter methods
  inline TString GetASCIIinputName()        const {return ASCIIinputName;};
  inline TString GetMapInputName()          const {return MapInputName;};
  inline TString GetRootCalibInputName()    const {return RootCalibInputName;};
  inline TString GetRootCalibOutputName()   const {return RootCalibOutputName;};
  inline TString GetRootInputName()         const {return RootInputName;};
  inline TString GetRootPedestalInputName() const {return RootPedestalInputName;};
  inline TString GetRootOutputName()        const {return RootOutputName;};
  inline TString GetPlotOutputDir()         const {return OutputNameDirPlots;};
  inline TString GetExternalBadChannelMap() const {return ExternalBadChannelMap;};
  inline TString GetExternalCalibFile()     const {return ExternalCalibFile;};
  
  inline std::fstream* GetASCIIinput() {return &ASCIIinput;};
  inline std::fstream* GetMapInput()   {return &MapInput;};

  inline TFile* GetRootCalibInput()    {return RootCalibInput;}
  inline TFile* GetRootInput()         {return RootInput;}
  inline TFile* GetRootPedestalInput() {return RootPedestalInput;}
  inline TFile* GetRootOutput()        {return RootOutput;}

  inline bool CanOverWrite(void)                const {return Overwrite;};
  inline bool IsToTransferCalib(void)           const {return ApplyTransferCalib;};
  inline bool IsToApplyCalibration(void)        const {return ApplyCalibration;};
  inline bool IsToConvert(void)                 const {return Convert;};
  inline bool IsToExtractPedestal(void)         const {return ExtractPedestal;};
  inline bool IsToExtractIntegPedestal(void)    const {return ExtractIntegPedestal;};
  inline bool IsToExtractScaling(void)          const {return ExtractScaling;};
  inline bool IsToExtractScalingImproved(void)  const {return ExtractScalingImproved;};
  inline bool IsHGCROC(void)                    const {return HGCROC;};
  
  inline bool IsToReextractNoise(void)          const {return ReextractNoise;};
  inline bool IsToSaveNoiseOnly(void)           const {return SaveNoiseOnly;};
  inline bool IsToSaveMipsOnly(void)            const {return SaveMipsOnly;};
  inline bool IsToEvalLocalTrigg(void)          const {return EvalLocalTriggers;};
  inline bool UseLocTriggFromFile(void)         const {return LocTriggFile;};
  inline bool IsToSaveCalibOnly(void)           const {return SaveCalibOnly;};
  inline bool IsCalibSaveToFile(void)           const {return SaveCalibToFile;};
  inline bool IsToSkimHGCROC(void)              const {return SkimHGCROC;};
  inline short GetCalcBadChannel(void)          const {return CalcBadChannel;};
  inline short GetExtPlotting(void)             const {return ExtPlot;};
  inline bool GetOverWriteCalib(void)           const {return OverWriteCalib;};
  inline int GetMaxEvents(void)                 const {return maxEvents;};

  //setter methods
  //Overload method for boolean...or is it too dangerous?
  inline void CanOverWrite(bool b)               {Overwrite=b;};
  inline void IsCalibSaveToFile(bool b)          {SaveCalibToFile=b;};
  inline void IsToTransferCalib(bool b)          {ApplyTransferCalib=b;};
  inline void IsToApplyCalibration(bool b)       {ApplyCalibration=b;};
  inline void IsToConvert(bool b)                {Convert=b;};
  inline void IsToExtractPedestal(bool b)        {ExtractPedestal=b;};
  inline void IsToExtractIntegPedestal(bool b)   {ExtractIntegPedestal=b;};
  inline void IsToExtractScaling(bool b)         {ExtractScaling=b;};
  inline void IsToExtractScalingImproved(bool b) {ExtractScalingImproved=b;};
  inline void IsHGCROC(bool b)                   {HGCROC=b;};
  inline void IsToReextractNoise(bool b)         {ReextractNoise=b;};
  inline void IsToSaveNoiseOnly(bool b)          {SaveNoiseOnly = b;};
  inline void IsToSaveMipsOnly(bool b)           {SaveMipsOnly = b;};
  inline void IsToEvalLocalTrigg(bool b)         {EvalLocalTriggers = b;};
  inline void IsToSaveCalibOnly(bool b)          {SaveCalibOnly = b;};
  inline void IsToSkimHGCROC(bool b)             {SkimHGCROC = b;};
  inline void IsCreateSummedTree(bool b)         {CreateSummedTree=b;}; 
  inline void UseLocTriggFromFile(bool b)        {LocTriggFile = b;};
  inline void SetCalcBadChannel(short b)         {CalcBadChannel = b;};
  inline void SetOverWriteCalib(bool b)          {OverWriteCalib = b;};
  inline void SetExtPlotting(short b)            {ExtPlot = b;};
  inline void EnableDebug(int i)                 {debug=i;};
  
  inline void SetYear(int year)                  {yearData=year;};
  inline void SetASCIIinput(TString name)        {ASCIIinputName=name;};
  inline void SetMapInput(TString name)          {MapInputName=name;};
  inline void SetRunListInput(TString name)      {RunListInputName=name;};
  inline void SetRootCalibInput(TString name)    {RootCalibInputName=name;};
  inline void SetRootCalibOutput(TString name)   {RootCalibOutputName=name;};
  inline void SetRootInput(TString name)         {RootInputName=name;};
  inline void SetRootPedestalInput(TString name) {RootPedestalInputName=name;};
  inline void SetRootOutput(TString name)        {RootOutputName =name;};
  inline void SetRootOutputHists(TString name)   {RootOutputNameHist =name;};
  inline void SetPlotOutputDir(TString name)     {OutputNameDirPlots =name;};
  inline void SetPlotExtension(TString name)     {plotSuffix = name;};
  inline void SetExternalBadChannelMap(TString name)     {ExternalBadChannelMap =name;};
  inline void SetExternalCalibFile(TString name)     {ExternalCalibFile =name;};
  inline void SetMaxEvents(int n)                 {maxEvents = n;};
  inline void SetIntegLayersN(int n)              {nLayersInteg = n;};
  
  //General methods
  bool CreateOutputRootFile(void);
  bool CheckAndOpenIO(void);
  bool Process(void);

  //Variable members
  TString ASCIIinputName;                 // CAEN ASCII file input name (only needed for converting data)
  TString RootOutputName;                 // file name of root output with tree
  TString RootOutputNameHist;             // file name of root output with additional histograms & fits
  TString OutputNameDirPlots;             // directory name of output for plots
  TString RootInputName;                  // file name of input root file 
  TString RootCalibInputName;             // file name of calibration root file (mip calib)
  TString RootCalibOutputName;            // file name of calibration root file (mip calib) output
  TString RootPedestalInputName;          // file name of pedestal root file (pedestal values)
  TString MapInputName;                   // file name geometry mapping
  TString RunListInputName;               // file name run list 
  TString ExternalBadChannelMap;          // file name external bad channel map
  TString ExternalCalibFile;              // file name external calib file
  TString plotSuffix        = "pdf";      // plot extension
  int nLayersInteg          = 1;           // number of intergated layers
  TFile* RootOutput         =nullptr;     // root file output tree
  TFile* RootOutputHist     =nullptr;     // root file output histos
  TFile* RootInput          =nullptr;     // root file input 
  TFile* RootPedestalInput  =nullptr;     // root file pedestal values
  TFile* RootCalibInput     =nullptr;     // root file calib values (mip)
  TFile* RootCalibOutput    =nullptr;     // root file calib values (mip) output
  std::fstream ASCIIinput;                // ASCII file streamer for CAEN data
  std::fstream MapInput;                  // file streamer for geometry mapping
  bool Convert                =false;     // Flag for data conversion 
  bool HGCROC                 =false;     // Flag for HGCROC data conversion
  bool ExtractPedestal        =false;     // Flag for pedestal extraction
  bool ExtractIntegPedestal   =false;     // Flag for intergated over layers pedestal extraction
  bool ExtractScaling         =false;     // Flag for mip scaling extraction
  bool ExtractScalingImproved =false;     // Flag for mip scaling extraction 2nd pass
  bool ReextractNoise         =false;     // Flag to enable noise trigger extraction and alternative pass for pedestals
  bool ApplyTransferCalib     =false;     // Flag for application of pedestals
  bool ApplyCalibration       =false;     // Flag for aplication of calibration
  bool SaveNoiseOnly          =false;     // Flag to reduce file to noise/pedestal only
  bool SaveMipsOnly           =false;     // Flag to reduce file to mips only
  bool SaveCalibOnly          =false;     // Flag to reduce file to mips only
  bool SaveCalibToFile        =false;     // Flag to save calib objects to text file
  bool SkimHGCROC             =false;     // Flag to skim HGCROC data from pure noise events
  bool EvalLocalTriggers      =false;     // Flag to run local trigger eval
  bool CreateSummedTree       =false;     // Flag to run summing of layers
  bool LocTriggFile           =false;     // Flag to use already evaluated triggeres
  short CalcBadChannel        =0;         // Flag to create bad channel map
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
  Event eventInt;                 
  
  TTree* TsetupIn=nullptr;
  TTree* TsetupOut=nullptr;
  TTree* TdataIn=nullptr;
  TTree* TdataOut=nullptr;
  TTree* TcalibIn=nullptr;
  TTree* TcalibOut=nullptr;

 protected:


 private:
  bool ConvertASCII2Root(void);
  bool ConvertOldRootFile2Root(void);
  bool IntegrateOvenNLayers(void);
  bool GetPedestal(void);
  bool TransferCalib(void);
  bool GetScaling(void);
  bool GetImprovedScaling(void);
  bool GetNoiseSampleAndRefitPedestal(void);
  bool RunEvalLocalTriggers(void);
  bool Calibrate(void);
  bool SaveNoiseTriggersOnly(void);
  bool SaveMuonTriggersOnly(void);
  bool SaveCalibToOutputOnly(void);
  bool SkimHGCROCData(void);
  std::map<int,short> ReadExternalBadChannelMap(void);
 };


#endif
