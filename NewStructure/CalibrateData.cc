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
#include "Analyses.h"

void PrintHelp(char* exe){
  std::cout<<"Usage:"<<std::endl;
  std::cout<<exe<<" [-option (arguments)]"<<std::endl;
  std::cout<<"Options:"<<std::endl;
  std::cout<<"-a       printing calib object to file (using name of output root or calib root file ending in txt)"<<std::endl;
  std::cout<<"-A aaa   stripping only calib and setup object to external file"<<std::endl;
  std::cout<<"-B lll   apply external bad channel map during transfer of calibs"<<std::endl;
  std::cout<<"-c [0-3] set calibration option:" <<std::endl;
  std::cout<<"          -> 0 - based on LG only & LG mip,"<<std::endl;
  std::cout<<"          -> 1 - based on LG only & LG calc mip,"<<std::endl;
  std::cout<<"          -> 2 - based on HG & LG & HG mip, LG mip, "<<std::endl;
  std::cout<<"          -> 3 - based on HG & LG & HG mip, LG calc mip"<<std::endl;
  std::cout<<"-C yyy   Apply calibrations stored in yyy root file to the input uncalibrated file"<<std::endl;
  std::cout<<"-d [0-n] switch on debug info with debug level 0 to n"<<std::endl;
  std::cout<<"-D [0-2] switch on event rejection due to data corruption "<<std::endl;
  std::cout<<"-E [1-3] extended plotting set to whatever value you specify"<<std::endl;
  std::cout<<"-f       Force to write output if already exist"<<std::endl;
  std::cout<<"-F fff   set explicit plot extension explicitly, default is pdf "<<std::endl;
  std::cout<<"-G GGG   use external ToA phase calib from GGG file "<<std::endl;
  std::cout<<"-i uuu   Input file in root format"<<std::endl;
  std::cout<<"-k kkk   enabling overwriting of calib file using external calib txt file"<<std::endl;
  std::cout<<"-l [0-n] skip plotting of single layers except multiples of defined number"<<std::endl;
  std::cout<<"-L LLL   enable testing with only limited number of events"<<std::endl;
  std::cout<<"-m mmm   Name of mapping file i.e 2024 PS TB [../configs/mappingFile_202409_CAEN.txt] "<<std::endl;
  std::cout<<"-M MMM   overwriting min cut off for calibrated data filtering with MMM. Experts only!"<<std::endl;
  std::cout<<"-o vvv   Output file name (mandatory)"<<std::endl;
  std::cout<<"-O kkk   Output directory name for plots (mandatory)"<<std::endl;
  std::cout<<"-q q     switch HGCROC signal to $q samples surrounding max sample "<<std::endl;
  std::cout<<"-Q q     switch HGCROC signal to $q samples surrounding max sample, force signal reevaluation "<<std::endl;
  std::cout<<"-r rrr   Name of run list file  2024 PS TB [../configs/DataTakingDB_202409_CAEN.csv] "<<std::endl;
  std::cout<<"-R mmm   Replace setup with mapping file specified as argument "<<std::endl;
  std::cout<<"-t       use local trigger eval from existing input, don't redo in calibrate"<<std::endl;
  std::cout<<"-T ttt   evaluate local triggers before calibrating, use external calib file ttt"<<std::endl;
  std::cout<<"-u       disable trigger primitive calc"<<std::endl;
  std::cout<<"-h       this help"<<std::endl<<std::endl;
  std::cout<<"Examples:"<<std::endl;
  std::cout<<exe<<" -C Calibration.root (-f) -o CalibratedOutput.root -i Input.root (-f to overwrite existing output)"<<std::endl;
}
  

int main(int argc, char* argv[]){
  if(argc<4) {
    PrintHelp(argv[0]);
    return 0;
  }
  Analyses AnAnalysis;
  int c;
  while((c=getopt(argc,argv,"aA:B:c:C:d:D:E:fF:G:hi:k:l:L:m:Mo:O:q:Q:r:R:tT:u"))!=-1){
    switch(c){
    case 'a':
      std::cout<<"CaibrateData: printing calib object to file"<<std::endl;
      AnAnalysis.IsCalibSaveToFile(true);
      break;
    case 'A':
      std::cout<<"CaibrateData: stripping calib object to file: " << optarg<<std::endl;
      AnAnalysis.IsToSaveCalibOnly(true);
      AnAnalysis.SetRootCalibOutput(optarg);
      break;
    case 'B':
      std::cout<<"CaibrateData: read bad channel map from external file: "<<optarg<<std::endl;
      AnAnalysis.SetExternalBadChannelMap(Form("%s",optarg));
      AnAnalysis.SetCalcBadChannel(1);
      break;
    case 'c':
      std::cout<<"CaibrateData: set calibration option "<<optarg<<std::endl;
      AnAnalysis.SetCalibOption(atoi(optarg));
      break;
    case 'C':
      std::cout<<"CaibrateData: Apply calibration (pedestal correction and scaling factor) from: "<<optarg<<std::endl;
      AnAnalysis.SetRootCalibInput(Form("%s",optarg));
      AnAnalysis.IsToApplyCalibration(true);
      break;
    case 'd':
      std::cout<<"CaibrateData: enable debug " << optarg <<std::endl;
      AnAnalysis.EnableDebug(atoi(optarg));
      break;
    case 'D':
      std::cout<<"CaibrateData: enable event rejection due to data corruption" << optarg<<std::endl;
      AnAnalysis.SetCleanupEvents(1);
      break;
    case 'E':
      std::cout<<"CaibrateData: enabling more extended plotting: "<< optarg<<std::endl;
      AnAnalysis.SetExtPlotting(atoi(optarg));
      break;
    case 'f':
      std::cout<<"CaibrateData: If output already exists it will be overwritten"<<std::endl;
      AnAnalysis.CanOverWrite(true);
      break;
    case 'F':
      std::cout<<"CaibrateData: Set Plot extension to: "<< optarg<<std::endl;
      AnAnalysis.SetPlotExtension(optarg);
      break;
    case 'G':
      std::cout<<"CaibrateData: use external TOA phase calib file: "<< optarg<<std::endl;
      AnAnalysis.SetExternalToACalibOffSetFile(optarg);
      break;
    case 'i':
      std::cout<<"CaibrateData: Root input file is: "<<optarg<<std::endl;
      AnAnalysis.SetRootInput(Form("%s",optarg));
      break;
    case 'k':
      std::cout<<"CaibrateData: enable overwrite from external text file: "<< optarg <<std::endl;
      AnAnalysis.SetExternalCalibFile(optarg);
      AnAnalysis.SetOverWriteCalib(true);
      break;
    case 'l':
      std::cout<<"CaibrateData: SetSkipLayer plotting processed:"<<optarg<<std::endl;
      AnAnalysis.SetPlotSkipLayer(atoi(optarg));
      break;
    case 'L':
      std::cout<<"CaibrateData: SetMaxEvents processed:"<<optarg<<std::endl;
      AnAnalysis.SetMaxEvents(atoi(optarg));
      break;
    case 'm':
      std::cout<<"CaibrateData: Mapping file from: "<<optarg<<std::endl;
      AnAnalysis.SetMapInput(Form("%s",optarg));
      break;
    case 'M':
      std::cout<<"CaibrateData: overwrite mip min cutoff (only for experts): "<<optarg<<std::endl;
      AnAnalysis.OverwriteMinMipFrac(atoi(optarg));
      break;
    case 'o':
      std::cout<<"CaibrateData: Output to be saved in: "<<optarg<<std::endl;
      AnAnalysis.SetRootOutput(Form("%s",optarg));
      break;
    case 'O':
      std::cout<<"CaibrateData: Outputdir plots to be saved in: "<<optarg<<std::endl;
      AnAnalysis.SetPlotOutputDir(Form("%s",optarg));
      break;
    case 'q':
      std::cout<<"CaibrateData: Switch HGCROC waveform integration, integrate " << optarg << " samples"<<std::endl;
      AnAnalysis.SetHGCROCNSampleInteg(atoi(optarg));
      break;
    case 'Q':
      std::cout<<"CaibrateData: Switch HGCROC waveform integration, forced, integrate " << optarg << " samples"<<std::endl;
      AnAnalysis.SetHGCROCNSampleIntegForced(atoi(optarg));
      break;
    case 'r':
      std::cout<<"CaibrateData: run list file from: "<<optarg<<std::endl;
      AnAnalysis.SetRunListInput(Form("%s",optarg));
      break;
    case 'R':
      std::cout<<"CaibrateData: overwrite mapping file from: "<<optarg<<std::endl;
      std::cout<<"CaibrateData: EXPERT USE ONLY!"<<std::endl;
      AnAnalysis.SetMapInput(Form("%s",optarg));
      AnAnalysis.SetOverWriteSetup(true);
      break;
    case 't':
      std::cout<<"CaibrateData: run without trigger eval, use from file" <<std::endl;
      AnAnalysis.UseLocTriggFromFile(true);
      break;
    case 'T':
      std::cout<<"CaibrateData: run local trigger, with calib file:" << optarg<<std::endl;
      AnAnalysis.IsToEvalLocalTrigg(true);
      AnAnalysis.SetRootCalibInput(Form("%s",optarg));
      break;
    case 'u':
      std::cout<<"CaibrateData: disable trigger primitive calculation" <<std::endl;
      AnAnalysis.DisableRecalcTriggPrimitives();
      break;
    case '?':
      std::cout<<"CaibrateData: Option "<<optarg <<" not supported, will be ignored "<<std::endl;
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

  bool status = AnAnalysis.Process();
  if(!AnAnalysis.CheckOutputWriteStatus()) status = false;
  if(!status){
    std::cerr<<"CaibrateData: processing or output write failed"<<std::endl;
    return -1;
  }
  std::cout<<"Exiting"<<std::endl;
  return 0;
}
