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
#include "EventDisplay.h"

// Setup* Setup::instancePtr=nullptr; // Remove for use on Mac OS -> Add to Setup.cc instead
//Calib* Calib::instancePtr=nullptr;

void PrintHelp(char* exe){
  std::cout<<"Usage:"<<std::endl;
  std::cout<<exe<<" [-option (arguments)]"<<std::endl;
  std::cout<<"Options:"<<std::endl;
  std::cout<<"-d [0-n] switch on debug info with debug level 0 to n"<<std::endl;
  std::cout<<"-F fff   set plot extension explicitly, default is pdf "<<std::endl;
  std::cout<<"-i uuu   Input file in root format"<<std::endl;
  std::cout<<"-c       Switches on to display HGCROC testbeam data: ADC"<<std::endl;
  std::cout<<"-p       Switches on to display HGCROC testbeam data: ADC - pedestal"<<std::endl;
  std::cout<<"-o       Switches on to display HGCROC testbeam data: TOT"<<std::endl;
  std::cout<<"-P zzz   Plots directory path"<<std::endl;
  std::cout<<"-r rrr   Name of run list file  2024 PS TB [../configs/DataTakingDB_202409_CAEN.csv] "<<std::endl;
  std::cout<<"-y yyyy  setting year externally to narrow parameters"<<std::endl;
  std::cout<<"-e       setting event number to plot, default: 0" <<std::endl;
  std::cout<<"-N       setting number of events to plot, default:1"<<std::endl;
  std::cout<<"-M       plot only muon triggered events"<<std::endl;
  std::cout<<"-t qqqqq minumum number of hit tiles to generate a plot"<<std::endl;
  std::cout<<"Examples:"<<std::endl;
  std::cout<<exe<<" -Q (-f) -P plotDir/ -i input.root"<<std::endl;
}
  

int main(int argc, char* argv[]){
  if(argc<4) {
    PrintHelp(argv[0]);
    return 0;
  }
  EventDisplay EvtDisplay;
  int c;
  while((c=getopt(argc,argv,"F:P:r:d:i:y:e:N:cpohMt:"))!=-1){
    switch(c){
    case 'd':
      std::cout<<"enable debug " << optarg <<std::endl;
      EvtDisplay.EnableDebug(atoi(optarg));
      break;
    case 'F':
      std::cout<<"DataPrep: Set Plot extension to: "<< optarg<<std::endl;
      EvtDisplay.SetPlotExtension(optarg);
      break;      
    case 'i':
      std::cout<<"Root input file is: "<<optarg<<std::endl;
      EvtDisplay.SetRootInput(Form("%s",optarg));
      break;
    case 'P':
      std::cout<<"Outputdir plots to be saved in: "<<optarg<<std::endl;
      EvtDisplay.SetPlotOutputDir(Form("%s",optarg));
      break;
    case 'r':
      std::cout<<"run list file from: "<<optarg<<std::endl;
      EvtDisplay.SetRunListInput(Form("%s",optarg));
      break;
    case 'y':
      std::cout<<"Setting year externally: "<<optarg<<std::endl;
      EvtDisplay.SetYear(atoi(optarg));
      break;
    case 'e':
      std::cout<<"Setting event to plot: " << optarg << std::endl;
      EvtDisplay.SetEventToPlot(atoi(optarg));
      break;
    case 'N':
      std::cout<<"Setting how many events to plot: "<<optarg<<std::endl;
      EvtDisplay.SetNumberOfEventsToPlot(atoi(optarg));
      break;
    case 'c':
      std::cout<<"Switching to plotting HGCROC testbeam data."<<std::endl;
      std::cout<<"Plotting ADC"<<std::endl;
      EvtDisplay.PlotHGCROC_ADCData();
      break;
    case 'p':
      std::cout<<"Switching to plotting HGCROC testbeam data."<<std::endl;
      std::cout<<"Plotting (ADC - pedestal)"<<std::endl;
      EvtDisplay.PlotHGCROC_ADCwPedData();
      break;
    case 'o':
      std::cout<<"Switching to plotting HGCROC testbeam data."<<std::endl;
      std::cout<<"Plotting TOT"<<std::endl;
      EvtDisplay.PlotHGCROC_TOTData();
      break;
    case 'M':
      std::cout<<"Plot only muon-triggered events in the range"<<std::endl;
      EvtDisplay.PlotMuonTriggeredEvents(true);
      break;
    case 't':
      std::cout<<"Requiring minimum number of hit tiles: " << optarg << std::endl;
      EvtDisplay.SetMinTilesHit(atoi(optarg));
      break;
    case '?':
      std::cout<<"Option "<<optarg <<" not supported, will be ignored "<<std::endl;
      break;
    case 'h':
      PrintHelp(argv[0]);
      return 0;
    }
  }
  if(!EvtDisplay.CheckAndOpenIO()){
    std::cout<<"Check input and configurations, inconsistency or error with I/O detected"<<std::endl;
    PrintHelp(argv[0]);
    return -1;
  }

  EvtDisplay.Process();
  std::cout<<"Exiting"<<std::endl;
  return 0;
}
