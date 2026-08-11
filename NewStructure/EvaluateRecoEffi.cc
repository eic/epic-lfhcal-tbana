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
#include "EvaluateRecoEffiHGCROC.h"


void PrintHelp(char* exe){
    std::cout<<"Usage:"<<std::endl;
    std::cout<<exe<<" [-option (arguments)]"<<std::endl;
    std::cout<<"Options:"<<std::endl;
    std::cout<<"-d [0-n]  switch on debug info with debug level 0 to n"<<std::endl;
    std::cout<<"-i uuu    path to the input file: .txt with list of .root files to process (mandatory)"<<std::endl;
    std::cout<<"-o        output directory (mandatory)"<<std::endl;
    std::cout<<"-r        path to the run list file (mandatory)"<<std::endl;
    std::cout<<"-u uuu    path to unconverted files" << std::endl; 
    std::cout<<"-h        print help"<<std::endl;
    std::cout<<"Examples:"<<std::endl;
    std::cout<<exe<<"-i input.csv -o output/"<<std::endl;
}


int main(int argc, char* argv[]){
    if(argc<1) {
        PrintHelp( argv[0] );
        return 0;
    }

    EvaluateRecoEffiGHCROC  evaluation;
    int c;
    while( (c=getopt(argc,argv,"d:i:r:o:u:h"))!=-1){
        switch(c){
            case 'd':
                std::cout << "Enable debug " << optarg << std::endl;
                evaluation.EnableDebug( atoi(optarg) );
                break;
            case 'i':
                std::cout << "Input file set to: " << optarg << std::endl;
                evaluation.SetInputFile( Form("%s",optarg) );
                break;
            case 'r':
                std::cout << "Input list file set to " << optarg << std::endl;
                evaluation.SetRunListInput( Form("%s",optarg) );
                break;
            case 'o':
                std::cout << "Output directory set to: " << optarg << std::endl;
                evaluation.SetOutputDirectory( Form("%s", optarg) );
                break;
            case 'u':
                std::cout << "path to unconverted files: " << optarg << std::endl;
                evaluation.SetUnconvertedDirectory( Form("%s", optarg) );
                break;
            case 'h':
                PrintHelp( argv[0] );
                return 0;
    }
  }
  if( !evaluation.CheckAndOpenIO() ){
    std::cout<<"Check input files and configurations, inconsistency or error with I/O detected"<<std::endl;
    return -1;
  }

  evaluation.Process();
  std::cout<<"Exiting"<<std::endl;

  return 0;
}
