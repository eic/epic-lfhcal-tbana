#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <utility>
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
#include "Analyses.h"

void PrintHelp(char* exe){
  std::cout<<"Usage:"<<std::endl;
  std::cout<<exe<<" [-option (arguments)]"<<std::endl;
  std::cout<<"Options:"<<std::endl;
  std::cout<<"-a       printing calib object to file (using name of output root or calib root file ending in txt)"<<std::endl;
  std::cout<<"-c xxx   Convert ASCII input file xxx into root format output"<<std::endl;
  std::cout<<"-w       Run HGCROC data convserion.  Omit to process CAEN data"<<std::endl;
  std::cout<<"-d [0-n] switch on debug info with debug level 0 to n"<<std::endl;
  std::cout<<"-f       Force to write output if already exist"<<std::endl;
  std::cout<<"-i uuu   Input file in root format"<<std::endl;
  std::cout<<"-m www   Name of mapping file  2024 PS TB [../configs/mappingFile_202409_CAEN.txt] "<<std::endl;
  std::cout<<"-o vvv   Output file name (mandatory)"<<std::endl;
  std::cout<<"-O kkk   Output directory name for plots (mandatory)"<<std::endl;
  std::cout<<"-r rrr   Name of run list file  2024 PS TB [../configs/DataTakingDB_202409_CAEN.csv] "<<std::endl;
  std::cout<<"-y yyyy  setting year externally to narrow parameters"<<std::endl;
  std::cout<<"-h       this help"<<std::endl<<std::endl;
  std::cout<<"Examples:"<<std::endl;
  std::cout<<exe<<" -c (-f) input.txt -o output.root (Convert ASCII to root) (-f to overwrite existing output)"<<std::endl;
}
  

int main(int argc, char* argv[]){
  if(argc<4) {
    PrintHelp(argv[0]);
    return 0;
  }
  Analyses AnAnalysis;
  int c;
  while((c=getopt(argc,argv,"ac:wd:fi:m:o:O:r:y:l:h"))!=-1){
    switch(c){
    case 'a':
      std::cout<<"Convert: printing calib object to file"<<std::endl;
      AnAnalysis.IsCalibSaveToFile(true);
      break;
    case 'c':
      std::cout<<"Convert: Convert ASCII input '"<<optarg<<"' to root format"<<std::endl;
      AnAnalysis.SetASCIIinput(Form("%s",optarg));
      AnAnalysis.IsToConvert(true);
      break;
    case 'w':
      std::cout<<"Convert HGCROC data"<<std::endl;
      AnAnalysis.IsHGCROC(true);
      break;
    case 'd':
      std::cout<<"Convert: enable debug " << optarg <<std::endl;
      AnAnalysis.EnableDebug(atoi(optarg));
      break;
    case 'f':
      std::cout<<"Convert: If output already exists it will be overwritten"<<std::endl;
      AnAnalysis.CanOverWrite(true);
      break;
    case 'i':
      std::cout<<"Convert: Root input file is: "<<optarg<<std::endl;
      AnAnalysis.SetRootInput(Form("%s",optarg));
      break;
    case 'm':
      std::cout<<"Convert: Mapping file from: "<<optarg<<std::endl;
      AnAnalysis.SetMapInput(Form("%s",optarg));
      break;
    case 'o':
      std::cout<<"Convert: Output to be saved in: "<<optarg<<std::endl;
      AnAnalysis.SetRootOutput(Form("%s",optarg));
      break;
    case 'O':
      std::cout<<"Convert: Outputdir plots to be saved in: "<<optarg<<std::endl;
      AnAnalysis.SetPlotOutputDir(Form("%s",optarg));
      break;
    case 'r':
      std::cout<<"Convert: run list file from: "<<optarg<<std::endl;
      AnAnalysis.SetRunListInput(Form("%s",optarg));
      break;
    case 'y':
      std::cout<<"Convert: Setting year externally: "<<optarg<<std::endl;
      AnAnalysis.SetYear(atoi(optarg));
      break;
    case '?':
      std::cout<<"Convert: Option "<<optarg <<" not supported, will be ignored "<<std::endl;
      break;
    case 'h':
      PrintHelp(argv[0]);
      return 0;
    case 'l':
      std::cout<<"Integrate over "<< atoi(optarg) <<" layers"<<std::endl;
      AnAnalysis.IsCreateSummedTree(true);
      AnAnalysis.SetIntegLayersN(atoi(optarg));//atoi(optarg));
      break;
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
