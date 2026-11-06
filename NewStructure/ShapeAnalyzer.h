#ifndef ShapeAnalyzer_H
#define ShapeAnalyzer_H

#include <iostream>
#include <fstream>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TObjString.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3F.h"

#include "Setup.h"
#include "RootSetupWrapper.h"
#include "Calib.h"
#include "Event.h"
#include "Tile.h"
#include "HGCROC.h"
#include "Caen.h"
#include "CommonHelperFunctions.h"
    
class ShapeAnalyzer{

 public:
  ShapeAnalyzer(){
    eventptr=&event;
    rswptr=&rsw;
    calibptr=&calib;
  }
  ~ShapeAnalyzer(){}

  //Getter methods
  inline TString GetRootInputName()         const {return RootInputName;};
  inline TString GetPlotOutputDir()         const {return OutputNameDirPlots;};
  
  inline TFile* GetRootInput()         {return RootInput;}
  inline TFile* GetRootOutput()        {return RootOutput;}

  inline bool CanOverWrite(void)                const {return Overwrite;};
  inline bool IsCalibSaveToFile(void)           const {return SaveCalibToFile;};
  inline short GetExtPlotting(void)             const {return ExtPlot;};
  inline bool IsToRunQA(void)                   const {return RunQA;};
  
  //setter methods
  //Overload method for boolean...or is it too dangerous?
  inline void CanOverWrite(bool b)                {Overwrite=b;};
  inline void IsCalibSaveToFile(bool b)           {SaveCalibToFile=b;};
  inline void SetExtPlotting(short b)             {ExtPlot = b;};
  inline void EnableDebug(int i)                  {debug=i;};

  inline void SetYear(int year)                   {yearData=year;};
  inline void SetRunListInput(TString name)       {RunListInputName=name;};
  inline void SetRootInput(TString name)          {RootInputName=name;};
  inline void SetPlotOutputDir(TString name)      {OutputNameDirPlots =name;};
  inline void SetPlotExtension(TString name)      {plotSuffix = name;};
  inline void SetEventToPlot(int e)               {plotEvt = e;};
  inline void SetNumberOfEventsToPlot(int n)      {nEvts = n;};
  inline void PlotMuonTriggeredEvents(bool y)     {plotMuonEvts=y;};
  inline void SetMinTilesHit(int n)               {minTilesHit = n;};
  inline void PlotHGCROC_ADCData()                {plotHGCROC = true; dataTypeHGCROC = 0; }
  inline void PlotHGCROC_ADCwPedData()            {plotHGCROC = true; dataTypeHGCROC = 1; }
  inline void PlotHGCROC_TOTData()                {plotHGCROC = true; dataTypeHGCROC = 2; }
  inline void PlotHGCROC_ADCmipCorr()             {plotHGCROC = true; dataTypeHGCROC = 3; }
  inline void PlotHGCROC_E()                      {plotHGCROC = true; dataTypeHGCROC = 4; }

  // clustering thresholds (defaults in ClusteringSettings, CommonHelperFunctions.h)
  inline void SetClusterCellThreshold(double e)   {clusterSettings.cellThreshold = e;};
  inline void SetClusterSeedValue(double e)       {clusterSettings.seedValue = e;};
  inline void SetClusterGrowFraction(double f)    {clusterSettings.growFraction = f;};
  inline void SetClusterMinEnergy(double e)       {clusterSettings.minClusterE = e;};
  inline void SetClusterWindowSize(int n)         {clusterSettings.windowSize = n;};
  inline void SetClusterMaxClusters(int n)        {clusterSettings.maxClusters = n;};
  inline void SetClusteringSettings(const ClusteringSettings& s) {clusterSettings = s;};
  inline const ClusteringSettings& GetClusteringSettings() const {return clusterSettings;};
  
  //General methods
  bool CheckAndOpenIO(void);
  bool Process(void);

  //Variable members
  TString OutputNameDirPlots;             // directory name of output for plots
  TString RootInputName;                  // file name of input root file 
  TString RunListInputName;               // file name run list 
  TString plotSuffix          = "pdf";    // plot extension
  TFile* RootOutput           =nullptr;   // root file output tree
  TFile* RootOutputHist       =nullptr;   // root file output histos
  TFile* RootInput            =nullptr;   // root file input 
  bool RunQA                  = false;    // Flag to run QA routine
  bool SaveCalibToFile        =false;     // Flag to save calib objects to text file
  short ExtPlot               =0;         // Enable extended plotting
  bool Overwrite              =true;      // Flag to overwrite outputs
  int debug                   =0;         // debug level 
  int yearData                =-1;        // data taking year externally set
  RootSetupWrapper rsw;                   // Wrapper singleton class for setup
  RootSetupWrapper* rswptr;               // Pointer to wrapper for singleton class for setup
  Setup* setup;                           // geometry setup
  Calib calib;                            // calibration object
  Calib* calibptr;                        // pointer to calib object
  Event event;
  Event* eventptr;
  int plotEvt                 = 0;
  int nEvts                   = 1;
  bool plotMuonEvts           = false;
  int minTilesHit             = 0;
  bool plotHGCROC             = false;
  int dataTypeHGCROC          = 0; // 0 - ADC, 1 - ADC-pedestal, 2 - ToT, 3 - ADC/mip-scale
  ClusteringSettings clusterSettings;     // clustering thresholds used throughout ShapeAnalyzer
  
  TTree* TsetupIn=nullptr;
  TTree* TdataIn=nullptr;
  TTree* TcalibIn=nullptr;

 protected:


 private:
  bool Plot();
  TH2D* Normalize2D(TH2D* hist);
  // All plot functions share the style defined in ShapeAnalyzer.cc (namespace PlotStyle); axis titles
  // are taken from the histograms themselves. 'label' is printed below the run header.
  void Plot1D(TH1D* hist, std::string Name, int sample, bool logY = true, std::string label = "");
  void Plot1D_Compare(TH1D* hist, std::string label, TH1D* hist2, std::string label2, std::string Name);
  void Plot2D(TH2D* hist, std::string Name, int sample, std::string label = "");
  void Plot1D_Comparison(TH1D* h1, TH1D* h2, std::string label1, std::string label2,
                         std::string prefix, std::string Name, bool logY, std::string label = "");
  void Plot1D_Comparison(std::vector<TH1D*> hists, std::vector<std::string> labels,
                         std::string prefix, std::string Name, bool logY, std::string label = "");
  void Plot2D_Comparison(TH2D* h1, TH2D* h2, std::string label1, std::string label2,
                         std::string prefix, std::string Name, std::string countLabel = "clusters");
  void Plot2D_Correlation(TH2D* hist, std::string Name, std::string label = "");

  TString plotHeader;   // run description drawn on top of every plot, set in Plot()
 };



#endif
