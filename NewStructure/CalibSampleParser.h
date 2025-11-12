#ifndef CalibSampleParser_H
#define CalibSampleParser_H

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

class CalibSampleParser{
    public:
        CalibSampleParser(){
            rswptr=&rsw;
            calibptr=&calib;
        }
        ~CalibSampleParser(){}

        // getter methods


        // setter methods
        inline void EnableDebug(int i)                  {debug=i;};
        inline void SetInputFile(TString name)          {inputFilePath=name;};
        inline void SetMappingFile(TString name)        {MapInputName=name;};
        inline void SetOutputFilename(TString name)     {OutputFilename=name;};
        inline void SetPlotDirectory(TString name)      {outputDirPlots=name;};
        inline void SetRunListInput(TString name)       {RunListInputName=name;};
        inline void SetRunNumber(int i)                 {RunNr=i;};
        inline void EnablePlotting()                    {doPlotting=true;};
        inline void SwitchPedestalCalib()               {optParse=1;};
        inline void SetInputCalibFile(TString name)     {calibInputFile=name;};


        // general methods
        bool CheckAndOpenIO(void);
        bool Process(void);

        // variable members 
        TString         inputFilePath;                  // path to the input file
        TString         MapInputName;                   // input mapping file name
        TString         calibInputFile;                 // input .root file with calib object to 'hack' with pedestal values - to use with SwitchPedestalCalib on
        TString         RunListInputName;               // file name run list 
        int             RunNr           = -1;           // run number to analyze
        std::fstream    MapInput;                       // file streamer for geometry mapping
        TString         OutputFilename;                 // output root file name, by default the same name as .csv file
        TString         OutputHistFilename;             // output root file name with histograms / plots / spectra
        TString         outputDirPlots;                 // directory for plots
        TFile*          RootCalibInput  = nullptr;      // root file input with calib object
        TFile*          RootOutput      = nullptr;      // root file output 
        TFile*          RootOutputHist  = nullptr;      // root file output for histos
        int             debug           = 0;            // debug level
        int             optParse        = 0;            // 0 - default .csv file from H2GCalib, 1 - pedestal extracted with .py script

        Event       event;                          // basically the tree branch
        RootSetupWrapper rsw;                       // Wrapper singleton class for setup
        RootSetupWrapper* rswptr;                   // Pointer to wrapper for singleton class for setup
        Setup*      setup;                          // geometry setup - read from the mapping file
        Calib       calib;                          // calibration object
        Calib*      calibptr;                       // pointer to calib object

        TTree*      tOutTree        = nullptr;      // output tree
        TTree*      TsetupOut       = nullptr;      // output geometry tree
        TTree*      TcalibIn        = nullptr;      
        TTree*      TcalibOut       = nullptr;

        bool        doPlotting      = false;

        std::vector<int>  calibChannelsSet; // #of channels, channels listed
        int         kcu             = 0;

    protected:

    private:
        bool Parse();
        bool ProcessAndPlotWaveforms();
        bool ParsePedestalCalib();

        bool IsCalibChannel(int channel){
            for(int i=0; i < calibChannelsSet.size(); i++){
                if( calibChannelsSet.at(i) == channel ) return true;
            }
            return false;
        }

};

#endif