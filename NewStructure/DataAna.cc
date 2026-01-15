#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <utility>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // Add for use on Mac OS -> Same goes for Analyses.cc
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
#include "DataAnalysis.h"

void PrintHelp(char* exe){
  std::cout<<"Usage:"<<std::endl;
  std::cout<<exe<<" [-option (arguments)]"<<std::endl;
  std::cout<<"Options:"<<std::endl;
  std::cout<<"-a       printing calib object to file (using name of output root or calib root file ending in txt)"<<std::endl;
  std::cout<<"-d [0-n] switch on debug info with debug level 0 to n"<<std::endl;
  std::cout<<"-f       Force to write output if already exist"<<std::endl;
  std::cout<<"-F fff   set plot extension explicitly, default is pdf "<<std::endl;
  std::cout<<"-i uuu   Input file in root format"<<std::endl;
  std::cout<<"-n nnn Input the maximum number of events you would like to run"<<std::endl;
  std::cout<<"-o vvv   Output file name "<<std::endl;
  std::cout<<"-O kkk   Output Histos saved in"<<std::endl;
  std::cout<<"-P zzz   Plots directory path"<<std::endl;
  std::cout<<"-Q       Run basic QA routine"<<std::endl;
  std::cout<<"-r rrr   Name of run list file  2024 PS TB [../configs/DataTakingDB_202409_CAEN.csv] "<<std::endl;
  std::cout<<"-y yyyy  setting year externally to narrow parameters"<<std::endl;
  std::cout<<"-t       Create Deltatime histos (Use -M and -m for Max and min)"<<std::endl;
  std::cout<<"Examples:"<<std::endl;
  std::cout<<exe<<" -Q (-f) -P plotDir/ -O OutputHist.root -i input.root (-f to overwrite existing output)"<<std::endl;
}
  

int main(int argc, char* argv[]){
  if(argc<4) {
    PrintHelp(argv[0]);
    return 0;
  }
  DataAnalysis AnAnalysis;
  int c;
  while((c=getopt(argc,argv,"ad:e:fF:hi:n:o:O:P:qQr:y:tM:m:L:l:"))!=-1){
    switch(c){
    case 'a':
      std::cout<<"printing calib object to file"<<std::endl;
      AnAnalysis.IsCalibSaveToFile(true);
      break;
    case 'd':
      std::cout<<"enable debug " << optarg <<std::endl;
      AnAnalysis.EnableDebug(atoi(optarg));
      break;
    case 'e':
      std::cout<<"enabling extended plotting: "<< optarg<<std::endl;
      AnAnalysis.SetExtPlotting(atoi(optarg));
      break;
    case 'f':
      std::cout<<"If output already exists it will be overwritten"<<std::endl;
      AnAnalysis.CanOverWrite(true);
      break;
    case 'F':
      std::cout<<"DataPrep: Set Plot extension to: "<< optarg<<std::endl;
      AnAnalysis.SetPlotExtension(optarg);
      break;      
    case 'i':
      std::cout<<"Root input file is: "<<optarg<<std::endl;
      AnAnalysis.SetRootInput(Form("%s",optarg));
      break;
    case 'n':
      std::cout<<"Maximum number of events is changed to "<<optarg<<std::endl;
      AnAnalysis.SetMaximumEvents(atoi(optarg));
      break;
    case 'o':
      std::cout<<"Output to be saved in: "<<optarg<<std::endl;
      AnAnalysis.SetRootOutput(Form("%s",optarg));
      break;
    case 'O':
      std::cout<<"Output  Histsto be saved in: "<<optarg<<std::endl;
      AnAnalysis.SetRootOutputHists(Form("%s",optarg));
      break;
    case 'P':
      std::cout<<"Outputdir plots to be saved in: "<<optarg<<std::endl;
      AnAnalysis.SetPlotOutputDir(Form("%s",optarg));
      break;
    case 'q':
      std::cout<<"running very simple QA"<<std::endl;
      AnAnalysis.IsToSimpleRunQA(true);
      break;
    case 'Q':
      std::cout<<"running QA"<<std::endl;
      AnAnalysis.IsToRunQA(true);
      break;
    case 'r':
      std::cout<<"run list file from: "<<optarg<<std::endl;
      AnAnalysis.SetRunListInput(Form("%s",optarg));
      break;
    case 'y':
      std::cout<<"Setting year externally: "<<optarg<<std::endl;
      AnAnalysis.SetYear(atoi(optarg));
      break;
    case '?':
      std::cout<<"Option "<<optarg <<" not supported, will be ignored "<<std::endl;
      break;
    case 't':
      std::cout<<"Making DeltaTime Histos"<<std::endl;
      AnAnalysis.SetDeltaTimePlotting(true);
      break;
    case 'M':
      std::cout<<"Setting max for DeltaTime histos"<<std::endl;
      AnAnalysis.SetTimeMax(atoi(optarg));
      break;
    case 'm':
      std::cout<<"Setting min for DeltaTime histos"<<std::endl;
      AnAnalysis.SetTimeMin(atoi(optarg));
      break;
    case 'L':
      std::cout<<"Setting max percent for DeltaTime histos"<<std::endl;
      AnAnalysis.SetPercentMax(atoi(optarg));
      break;
    case 'l':
      std::cout<<"Setting min percent for DeltaTime histos"<<std::endl;
      AnAnalysis.SetPercentMin(atoi(optarg));
      break;    
    case 'h':
      PrintHelp(argv[0]);
      return 0;
    }
  }
  if(!AnAnalysis.CheckAndOpenIO()){
    std::cout<<"Check input and configurations, inconsistency or error with I/O detected"<<std::endl;
    PrintHelp(argv[0]);
    return -1;
  }

  AnAnalysis.Process();
  std::cout<<"Exiting"<<std::endl;
  return 0;
}
