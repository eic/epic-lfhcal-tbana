#include "CompareHGCROCCalib.h"
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <cstdlib>
#include "TROOT.h"
#include "TPaletteAxis.h"
#include "TProfile.h"
#include "TChain.h"
#include "TileSpectra.h"
#include "PlotHelper.h"
#include "CommonHelperFunctions.h"

#include "waveform_fitting/waveform_fit_base.h"
#include "waveform_fitting/crystal_ball_fit.h"
#include "waveform_fitting/max_sample_fit.h"


// ****************************************************************************
// Checking and opening input files
// ****************************************************************************
bool CompareHGCROCCalib::CheckAndOpenIO(void){
    int matchingbranch;

    // reading files from a text file
    if( !inputFileList.IsNull() ){
        // text file with one .root file per line
        std::cout << "******************************" << std::endl;
        std::cout << "Reading from the file list: " << inputFileList.Data() << std::endl;
        std::cout << "******************************" << std::endl;

        std::fstream dummyTxt;
        dummyTxt.open( inputFileList.Data(), std::ios::in );
        if( !dummyTxt.is_open() ){
            std::cout << "Error opening " << inputFileList.Data() << ", does the file exist?" << std::endl;
            return false;
        }

        //initialize root file name
        std::string dummyRootName;
        //set first root file name
        dummyTxt>>dummyRootName;

        int goodsetup; int gooddata;
        while( dummyTxt.good() ){
            std::cout << dummyRootName.data() << std::endl;
            //check that file exists and can be opened 
            TFile dummyRoot = TFile( dummyRootName.data() , "READ");
            if( dummyRoot.IsZombie() ){
                std::cout << "Error opening " << dummyRootName.data() << ", does the file exists?" << std::endl;
                return false;
            }
            dummyRoot.Close();
            //add file name to setup and data chain
            goodsetup   = TsetupIn->Add( dummyRootName.c_str() );
            gooddata    = TdataIn->Add( dummyRootName.c_str() );
            if( goodsetup == 0 ){
                std::cout << "Issues retrieving Setup tree from " << dummyRootName.data() <<", file is ignored" << std::endl;
            }
            if( gooddata == 0 ){
                std::cout << "Issues retrieving Data tree from " << dummyRootName.data() <<", file is ignored" << std::endl;
            }
            //set next root file name
            dummyTxt>>dummyRootName;
        }

        // Setup TChain of setups and data
        // initialize global variable setup
        setup = Setup::GetInstance();
        std::cout<<"Setup add " << setup << std::endl;
        matchingbranch=TsetupIn->SetBranchAddress("setup",&rswptr);
        if( matchingbranch < 0){
            std::cout << "Error retrieiving Setup info from the tree" << std::endl;
            return false;
        }
        std::cout << "Entries: " << TsetupIn->GetEntries() << std::endl;
        TsetupIn->GetEntry(0);
        setup->Initialize(*rswptr);

        // initialize data with correct branch
        matchingbranch=TdataIn->SetBranchAddress("event",&eventptr);
        if( matchingbranch < 0){
            std::cout << "Error retrieiving data from the tree" << std::endl;
            return false;
        }

    }

    std::cout <<"=============================================================" << std::endl;
    std::cout <<" Basic setup complete" << std::endl;
    std::cout <<"=============================================================" << std::endl;
    return true;
}


// ****************************************************************************
// Primary process function 
// ****************************************************************************
bool CompareHGCROCCalib::Process(void){
    bool status;

    status = CompareTOT();

    return status;
}

bool CompareHGCROCCalib::CompareTOT(void){

    int nentries    = TsetupIn->GetEntries();

    std::vector<int>    *values     = new std::vector<int>[5]; // 0 - tile nr (from 0 to n_channels-1), 1 - inj val (run number), 2 - TOT, 3 - ADC, 4 - cellID
    int     n_channels = 0; // will be read in the loop
    int pointCounter = 0;

    // for plotting
    int max_inj = 0; 
    int max_adc = 0; 
    int max_tot = 0; 

// ******************************************************************************************
// Iterate over all entries (runs) in the calib tree
// ******************************************************************************************
    for(int ientry = 0; ientry<nentries; ientry++){
        TsetupIn->GetEntry(ientry);
        TdataIn->GetEntry(ientry);

        if( ientry == 0 ){
            n_channels = event.GetNTiles();
        }


        for(int i = 0; i<event.GetNTiles(); i++){
            Hgcroc*     aTile = (Hgcroc*)event.GetTile(i);
            int tmpTileID   = i;
            int tmpInjVal   = event.GetRunNumber();
            int tmpTOT      = aTile->GetRawTOT();
            int tmpADC      = aTile->GetIntegratedADC();
            int tmpCellID   = aTile->GetCellID();
            
            if( tmpInjVal > max_inj )   max_inj = tmpInjVal; 
            if( tmpTOT  > max_tot )     max_tot = tmpTOT;
            if( tmpADC  > max_adc )     max_adc = tmpADC;

            if(debug > 1) std::cout << "Inj val: " << tmpInjVal << "\t cellID: " << aTile->GetCellID() << "\t i-th tile: " << i << "\t TOT: " << tmpTOT << "\t ADC: " << tmpADC << std::endl;

            values[0].push_back( tmpTileID );
            values[1].push_back( tmpInjVal );
            values[2].push_back( tmpTOT );
            values[3].push_back( tmpADC );
            values[4].push_back( tmpCellID );
            pointCounter++;
        }

        if( debug > 1 ) std::cout << "Point counter: " << pointCounter << "\t n_points in one graph: " << pointCounter / n_channels<< std::endl;
    }

    // we have all the values "flattened", so we will need to detangle it into n_channels TGraphs - separated based on cellID
    TGraph* graphTOT[1000];
    TGraph* graphADC[1000];
    int    cellID[1000];
    
    for(int i=0; i<n_channels; i++){
        graphTOT[i] = new TGraph( pointCounter / n_channels );
        graphADC[i] = new TGraph( pointCounter / n_channels );
        for(int j=0; j<pointCounter; j++){
            if( values[0].at(j) == i ){
                graphTOT[i]->SetPoint(j / n_channels, values[1].at(j), values[2].at(j) );
                graphADC[i]->SetPoint(j / n_channels, values[1].at(j), values[3].at(j) );
                cellID[i]   = values[4].at(j);
            }
        }
    }

    Int_t textSizePixel = 30;

    for(int i=0; i<n_channels; i++){
        TCanvas*    canvas  = new TCanvas("canvas", "canvas", 800, 600);
        DefaultCanvasSettings( canvas, 0.13, 0.02, 0.045, 0.11);

        SetMarkerDefaultsTGraph( graphTOT[i], 24, 0.8, kRed+1, kRed+1);        
        SetMarkerDefaultsTGraph( graphADC[i], 24, 0.8, kBlue+2, kBlue+2);    

        TH2F*   hDummy  = new TH2F("hDummy", "hDummy", 100, 0, max_inj+50, 100, 0, (max_tot > max_adc ) ? max_tot * 1.2 : max_adc * 1.2 );
        SetStyleHistoTH2ForGraphs(hDummy, "inj val", "TOT/ADC", 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.5, 510, 510, 43, 63);  

        hDummy->Draw();
        graphTOT[i]->Draw("same,p");
        graphADC[i]->Draw("same,p");

        TLatex  latexCellID     = TLatex(0.94, 0.89, Form("#bf{Cell %d}", cellID[i]));
        latexCellID.SetNDC();
        latexCellID.SetTextFont(42);
        latexCellID.SetTextSize(0.045);
        latexCellID.SetTextAlign(32);
        latexCellID.Draw("same");

        TLegend* legend = GetAndSetLegend(0.15, 0.82, 2);
        legend->AddEntry( graphTOT[i], "TOT", "p");
        legend->AddEntry( graphADC[i], "ADC", "p");
        legend->Draw("same");
        

        canvas->SaveAs( Form("%s/TOT_ADCvsInjVal_Cell%d.pdf", outputDirPlots.Data(), cellID[i] ) );
        delete canvas; delete hDummy;
    }

    return true;
}
