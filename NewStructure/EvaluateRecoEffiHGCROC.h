#ifndef EvaluateRecoEffiHGCROC_H
#define EvaluateRecoEffiHGCROC_H


#include <iostream>
#include <fstream>
#include <TROOT.h>
#include <TString.h>
#include <TObject.h>
#include <TObjString.h>
#include <TSystem.h>
#include <TChain.h>
#include <TMath.h>
#include <TVector3.h>
#include <iostream>
#include <fstream>
#include <TParticlePDG.h>
#include <TDatabasePDG.h>
#include <TRandom3.h>

#include <TCanvas.h>
#include <TPad.h>
#include <TH1.h>
#include <TH1D.h>
#include <TH1F.h>
#include <TH2.h>
#include <TH3.h>
#include <TFile.h>
#include <TH2D.h>
#include <TH2F.h>
#include <TString.h>
#include <TDatime.h>
#include <TF1.h>
#include <TF2.h>
#include <THStack.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TGraphAsymmErrors.h>
#include <TLine.h>
#include <TLatex.h>
#include <TArrow.h>
#include <TGraphErrors.h>
#include <TGaxis.h>
#include <TLegend.h>
#include <TFrame.h>
#include <TLorentzVector.h>

#include "Tile.h"
#include "CommonHelperFunctions.h"
#include "PlotHelper.h"
#include "PlotHelper_general.h"


class EvaluateRecoEffiGHCROC{

    public:
        EvaluateRecoEffiGHCROC(){}
        ~EvaluateRecoEffiGHCROC(){}

        // setter methods
        inline void EnableDebug(int i)                    {debug=i;};
        inline void SetInputFile(TString name)            {inputFilePath=name;};
        inline void SetRunListInput(TString name)         {RunListInputName=name;};
        inline void SetOutputDirectory(TString name)      {outputDir=name;}
        inline void SetUnconvertedDirectory(TString name) {unconvertedDir=name;}

        // general methods
        bool CheckAndOpenIO(void);
        bool Process(void);


        // variable members 
        int             debug           = 0;            // debug level
        TString         inputFilePath;                  // path to the input file (.txt with the list of .root files)
        TString         RunListInputName;               // file name run list 
        TString         outputDir;                      // output directory
        TString         unconvertedDir      = "";       // output directory

    protected:

    private:
        bool DoEvaluateRecoEffiHGCROC();
                            
};

#endif
