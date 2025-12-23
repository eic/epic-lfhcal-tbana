#ifndef CompareHGCROCCalib_H
#define CompareHGCROCCalib_H

#include <iostream>
#include <fstream>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TObjString.h"

#include "Setup.h"
#include "RootSetupWrapper.h"
#include "Calib.h"
#include "Event.h"
#include "Tile.h"
#include "HGCROC.h"
#include "Caen.h"

// compare ADC and TOT from .csv outputs from H2GCROCCalib as function of injection 

class CompareHGCROCCalib{
    public:
        CompareHGCROCCalib(){
            TsetupIn    = new TChain("Setup");
            TdataIn     = new TChain("CalibSample");
            rswptr=&rsw;
            eventptr=&event;
        }
        ~CompareHGCROCCalib(){}

    //     // getter methods


    //     // setter methods
        inline void EnableDebug(int i)                  {debug=i;};
        inline void SetInputFile(TString name)          {inputFileList=name;};
        inline void SetPlotDirectory(TString name)      {outputDirPlots=name;};


        // general methods
        bool CheckAndOpenIO(void);
        bool Process(void);

    //     // variable members 
        TString         inputFileList;                  // path to the input file
        std::fstream    MapInput;                       // file streamer for geometry mapping
        TString         outputDirPlots;                 // directory for plots
        TFile*          RootCalibInput  = nullptr;      // root file input with calib object
        TFile*          RootOutput      = nullptr;      // root file output 
        TFile*          RootOutputHist  = nullptr;      // root file output for histos
        int             debug           = 0;            // debug level
        int             optParse        = 0;            // 0 - default .csv file from H2GCalib, 1 - pedestal extracted with .py script

        Event       event;                              // basically the tree branch
        Event*      eventptr;
        RootSetupWrapper    rsw;                        // Wrapper singleton class for setup
        RootSetupWrapper*   rswptr;                     // Pointer to wrapper for singleton class for setup
        Setup*      setup;                              // geometry setup - read from the mapping file
        TTree*      dataTree;   

        TChain*     TdataIn     = nullptr;
        TChain*     TsetupIn    = nullptr;

    protected:

    private:
        bool CompareTOT();
};

#endif