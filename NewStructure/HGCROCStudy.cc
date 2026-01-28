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
#include "HGCROC_Waveform_Analysis.h"

void PrintHelp(char* exe){
  std::cout<<"Usage:"<<std::endl;
  std::cout<<exe<<" [-option (arguments)]"<<std::endl;
  std::cout<<"Options:"<<std::endl;
  std::cout<<"-B lll   apply external bad channel map during transfer of calibs"<<std::endl;
  std::cout<<"-d [0-n] switch on debug info with debug level 0 to n"<<std::endl;
  std::cout<<"-E [1-3] extended plotting set to whatever value you specify"<<std::endl;
  std::cout<<"-f       Force to write output if already exist"<<std::endl;
  std::cout<<"-F fff   set explicit plot extension explicitly, default is pdf "<<std::endl;
  std::cout<<"-i uuu   Input file in root format"<<std::endl;
  std::cout<<"-k kkk   enabling overwriting of calib file using external calib txt file"<<std::endl;
  std::cout<<"-L LLL   enable testing with only limited number of events"<<std::endl;
  std::cout<<"-o vvv   Output file name (mandatory)"<<std::endl;
  std::cout<<"-O kkk   Output directory name for plots (mandatory)"<<std::endl;
  std::cout<<"-w       Analyse waveform of HGCROC data"<<std::endl;
  std::cout<<"-s sss   Plot correlation plots TOA only for TOA sample sss "<<std::endl;
  std::cout<<"-h       this help"<<std::endl<<std::endl;
  std::cout<<"Examples:"<<std::endl;
}
  

int main(int argc, char* argv[]){
  if(argc<4) {
    PrintHelp(argv[0]);
    return 0;
  }
  HGCROC_Waveform_Analysis AnAnalysis;
  int c;
  while((c=getopt(argc,argv,"B:d:E:fF:i:hk:L:o:O:r:s:w"))!=-1){
    switch(c){
    case 'B':
      std::cout<<"HGCROCStudy: read bad channel map from external file: "<<optarg<<std::endl;
      AnAnalysis.SetExternalBadChannelMap(Form("%s",optarg));
      break;
    case 'd':
      std::cout<<"HGCROCStudy: enable debug " << optarg <<std::endl;
      AnAnalysis.EnableDebug(atoi(optarg));
      break;
    case 'E':
      std::cout<<"HGCROCStudy: enabling more extended plotting: "<< optarg<<std::endl;
      AnAnalysis.SetExtPlotting(atoi(optarg));
      break;
    case 'f':
      std::cout<<"HGCROCStudy: If output already exists it will be overwritten"<<std::endl;
      AnAnalysis.CanOverWrite(true);
      break;
    case 'F':
      std::cout<<"HGCROCStudy: Set Plot extension to: "<< optarg<<std::endl;
      AnAnalysis.SetPlotExtension(optarg);
      break;
    case 'i':
      std::cout<<"HGCROCStudy: Root input file is: "<<optarg<<std::endl;
      AnAnalysis.SetRootInput(Form("%s",optarg));
      break;
    case 'k':
      std::cout<<"HGCROCStudy: enable overwrite from external text file: "<< optarg <<std::endl;
      AnAnalysis.SetExternalCalibFile(optarg);
      AnAnalysis.SetOverWriteCalib(true);
      break;
    case 'L':
      std::cout<<"HGCROCStudy: SetMaxEvents processed:"<<optarg<<std::endl;
      AnAnalysis.SetMaxEvents(atoi(optarg));
      break;
    case 'o':
      std::cout<<"HGCROCStudy: Output to be saved in: "<<optarg<<std::endl;
      AnAnalysis.SetRootOutput(Form("%s",optarg));
      break;
    case 'O':
      std::cout<<"HGCROCStudy: Outputdir plots to be saved in: "<<optarg<<std::endl;
      AnAnalysis.SetPlotOutputDir(Form("%s",optarg));
      break;
    case 'r':
      std::cout<<"HGCROCStudy: run list file from: "<<optarg<<std::endl;
      AnAnalysis.SetRunListInput(Form("%s",optarg));
      break;
    case 's':
      std::cout<<"HGCROCStudy: sample number: "<<optarg<<std::endl;
      AnAnalysis.SetFixedTOASample(atoi(optarg));
      break;
    case 'w':
      std::cout<<"HGCROCStudy: analyse HGCROC waveform"<<std::endl;
      AnAnalysis.IsToAnalysisWaveForm(true);
      break;
    case '?':
      std::cout<<"HGCROCStudy: Option "<<optarg <<" not supported, will be ignored "<<std::endl;
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
