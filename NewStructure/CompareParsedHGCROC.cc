#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <utility>
#include <unistd.h> // Add for use on Mac OS -> Same goes for Analyses.cc
#include <string>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH1D.h"
#include "TObjArray.h"
#include "TObjString.h"

#include "Setup.h"
#include "Calib.h"
#include "Event.h"
#include "Tile.h"
#include "HGCROC.h"
#include "Caen.h"
#include "CompareHGCROCCalib.h"

void PrintHelp(char* exe){
  std::cout<<"Usage:"<<std::endl;
  std::cout<<exe<<" [-option (arguments)]"<<std::endl;
  std::cout<<"Options:"<<std::endl;
  std::cout<<"-d [0-n]  switch on debug info with debug level 0 to n"<<std::endl;
  std::cout<<"-i uuu    path to the input file: .txt with file list (mandatory)"<<std::endl;
  std::cout<<"-p        set the plotting directory (mandatory)"<<std::endl;
  std::cout<<"-h        print help"<<std::endl;
  std::cout<<"Examples:"<<std::endl;
  std::cout<<exe<<"-i input.csv -o output/"<<std::endl;
}

int main(int argc, char* argv[]){
    if(argc<1) {
        PrintHelp( argv[0] );
        return 0;
    }

    CompareHGCROCCalib compSamples;
    int c;
    while( (c=getopt(argc,argv,"d:i:hp::"))!=-1){
        switch(c){
            case 'd':
                std::cout << "Enable debug " << optarg << std::endl;
                compSamples.EnableDebug( atoi(optarg) );
                break;
            case 'i':
                std::cout << "Input file set to: " << optarg << std::endl;
                compSamples.SetInputFile( Form("%s",optarg) );
                break;
            case 'h':
                PrintHelp( argv[0] );
                return 0;
            case 'p':
                std::cout << "Plotting directory set to " << optarg << std::endl;
                compSamples.SetPlotDirectory( Form("%s",optarg) );
                break;
        }
    }
    if(!compSamples.CheckAndOpenIO()){
      std::cout<<"Check input and configurations, inconsistency or error with I/O detected"<<std::endl;
      PrintHelp(argv[0]);
      return -1;
    }

    compSamples.Process();
    std::cout<<"Exiting"<<std::endl;
    return 0;
}
