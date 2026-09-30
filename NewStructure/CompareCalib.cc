#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef __APPLE__
#include <unistd.h>
#endif
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
#include "ComparisonCalib.h"

void PrintHelp(char* exe){
  std::cout<<"Usage:"<<std::endl;
  std::cout<<exe<<" [-option (arguments)]"<<std::endl;
  std::cout<<"Options:"<<std::endl;
  std::cout<<"-c ccc   list of cell to be plotted separately"<<std::endl;
  std::cout<<"-d [0-3] Debugging mode"<<std::endl;
  std::cout<<"-e [0-1] extended plotting"<<std::endl;
  std::cout<<"-E [1-X] histo reading options for expanded file list"<<std::endl;
  std::cout<<"-f       Force to write output if already exist"<<std::endl;
  std::cout<<"-F fff   set explicit plot extension explicitly, default is pdf "<<std::endl;
  std::cout<<"-H       switch to HGCROC output" << std::endl;
  std::cout<<"-i uuu   Input file list"<<std::endl;
  std::cout<<"-I uuu   expanded input file list"<<std::endl;
  std::cout<<"-L [1-63]restrict max layer plotting"<<std::endl;
  std::cout<<"-o vvv   Output file name (mandatory)"<<std::endl;
  std::cout<<"-O kkk   Output directory name for plots (mandatory)"<<std::endl;
  std::cout<<"-r rrr   Name of run list file  2024 PS TB [../configs/DataTakingDB_202409_CAEN.csv] "<<std::endl;
  std::cout<<"-s       Flag HV scan (put Vop in legend)"<<std::endl;
  std::cout<<"-x XXXX  Give axis option with XXXX, defined: run, ite, integ, Vop, intensity"<<std::endl;
  std::cout<<"-h       this help"<<std::endl<<std::endl;
  std::cout<<"Examples:"<<std::endl;
  std::cout<<exe<<" (-f) -o TrendingOutput.root -i input_list.txt (-f to overwrite existing output)"<<std::endl;
  std::cout<<exe<<" (-f) -o TrendingOutput.root InputName*.root InputName2.root (-f to overwrite existing output)"<<std::endl;
}
  

int main(int argc, char* argv[]){
  if(argc<4) {
    PrintHelp(argv[0]);
    return 0;
  }
  std::vector<std::string> RootRegexp;
  std::vector<std::string>::iterator it;
  for(int i=1; i<argc; i++){
    RootRegexp.push_back(argv[i]);
  }
  ComparisonCalib CompAnalysis;
  int c;
  while((c=getopt(argc,argv,"ac:d:e:E:fF:Hi:I:L:o:O:r:sSx:h"))!=-1){
    switch(c){
    case 'a':
      std::cout<<"Compare: plot as function of laser intensity " <<std::endl;
      CompAnalysis.SetTrendingAxis(5);
      it=std::find(RootRegexp.begin(),RootRegexp.end(),"-a");
      RootRegexp.erase(it);
      break;
    case 'c':
      std::cout<<"Compare: set list of cells for detailed plotting " << optarg <<std::endl;
      CompAnalysis.SetCellList(optarg);
      it=std::find(RootRegexp.begin(),RootRegexp.end(),"-c");
      RootRegexp.erase(it);
      it=std::find(RootRegexp.begin(),RootRegexp.end(),Form("%s",optarg));
      RootRegexp.erase(it);
      break;
    case 'd':
      std::cout<<"Compare: enable debug " << optarg <<std::endl;
      CompAnalysis.EnableDebug(atoi(optarg));
      it=std::find(RootRegexp.begin(),RootRegexp.end(),"-d");
      RootRegexp.erase(it);
      it=std::find(RootRegexp.begin(),RootRegexp.end(),Form("%s",optarg));
      RootRegexp.erase(it);
      break;
    case 'e':
      std::cout<<"Compare: enabling extended plotting"<<std::endl;
      CompAnalysis.SetExtPlotting(atoi(optarg));
      it=std::find(RootRegexp.begin(),RootRegexp.end(),"-e");
      RootRegexp.erase(it);
      it=std::find(RootRegexp.begin(),RootRegexp.end(),Form("%s",optarg));
      RootRegexp.erase(it);
      break;
    case 'E':
      std::cout<<"Compare: set histo reading option"<<std::endl;
      CompAnalysis.ExpandedList(atoi(optarg));
      it=std::find(RootRegexp.begin(),RootRegexp.end(),"-E");
      RootRegexp.erase(it);
      it=std::find(RootRegexp.begin(),RootRegexp.end(),Form("%s",optarg));
      RootRegexp.erase(it);
      break;
    case 'f':
      std::cout<<"Compare: If output already exists it will be overwritten"<<std::endl;
      CompAnalysis.CanOverWrite(true);
      it=std::find(RootRegexp.begin(),RootRegexp.end(),"-f");
      RootRegexp.erase(it);
      break;
    case 'F':
      std::cout<<"Compare: Set Plot extension to: "<< optarg<<std::endl;
      CompAnalysis.SetPlotExtension(optarg);
      it=std::find(RootRegexp.begin(),RootRegexp.end(),"-F");
      RootRegexp.erase(it);
      it=std::find(RootRegexp.begin(),RootRegexp.end(),Form("%s",optarg));
      RootRegexp.erase(it);
      break;
    case 'i':
      std::cout<<"Compare: Root input file is: "<<optarg<<std::endl;
      CompAnalysis.SetInputList(Form("%s",optarg));
      it=std::find(RootRegexp.begin(),RootRegexp.end(),"-i");
      RootRegexp.erase(it);
      it=std::find(RootRegexp.begin(),RootRegexp.end(),Form("%s",optarg));
      RootRegexp.erase(it);
      break;
    case 'I':
      std::cout<<"Compare: Expanded Root input file is: "<<optarg<<std::endl;
      CompAnalysis.SetInputList(Form("%s",optarg));
      if (CompAnalysis.GetExpandedList() == 0){
        CompAnalysis.ExpandedList(1);
      }
      it=std::find(RootRegexp.begin(),RootRegexp.end(),"-I");
      RootRegexp.erase(it);
      it=std::find(RootRegexp.begin(),RootRegexp.end(),Form("%s",optarg));
      RootRegexp.erase(it);
      break;
    case 'H':
      std::cout<<"Compare: HGCROC output "<<std::endl;
      CompAnalysis.SetIsHGCROC(true);
      it=std::find(RootRegexp.begin(),RootRegexp.end(),"-H");
      RootRegexp.erase(it);
      break;
    case 'L':
      std::cout<<"Compare: restrict max layer plotting: "<<optarg<<std::endl;
      CompAnalysis.SetMaxPlotLayer(atoi(optarg));
      it=std::find(RootRegexp.begin(),RootRegexp.end(),"-L");
      RootRegexp.erase(it);
      it=std::find(RootRegexp.begin(),RootRegexp.end(),Form("%s",optarg));
      RootRegexp.erase(it);
      break;
    case 'o':
      std::cout<<"Compare: Output to be saved in: "<<optarg<<std::endl;
      CompAnalysis.SetRootOutput(Form("%s",optarg));
      it=std::find(RootRegexp.begin(),RootRegexp.end(),"-o");
      RootRegexp.erase(it);
      it=std::find(RootRegexp.begin(),RootRegexp.end(),Form("%s",optarg));
      RootRegexp.erase(it);
      break;
    case 'O':
      std::cout<<"Compare: Outputdir plots to be saved in: "<<optarg<<std::endl;
      CompAnalysis.SetPlotOutputDir(Form("%s",optarg));
      it=std::find(RootRegexp.begin(),RootRegexp.end(),"-O");
      RootRegexp.erase(it);
      it=std::find(RootRegexp.begin(),RootRegexp.end(),Form("%s",optarg));
      RootRegexp.erase(it);
      break;
    case 'r':
      std::cout<<"Compare: run list file from: "<<optarg<<std::endl;
      CompAnalysis.SetRunListInput(Form("%s",optarg));
      it=std::find(RootRegexp.begin(),RootRegexp.end(),"-r");
      RootRegexp.erase(it);
      it=std::find(RootRegexp.begin(),RootRegexp.end(),Form("%s",optarg));
      RootRegexp.erase(it);
      break;
    case 's':
      std::cout<<"Compare: Plotting for HV scan"<<std::endl;
      CompAnalysis.SetLegendLabelOpt(3);
      //CompAnalysis.SetPlotColors(2);
      it=std::find(RootRegexp.begin(),RootRegexp.end(),"-s");
      RootRegexp.erase(it);
      break;
    case 'S':
      std::cout<<"Compare: Enable single layer plotting, careful very slow!"<<std::endl;
      CompAnalysis.SetEnableSingleLayer(1);
      it=std::find(RootRegexp.begin(),RootRegexp.end(),"-S");
      RootRegexp.erase(it);
      break;
    case 'x': { 
      std::cout<<"Compare: Setting x-axis option: "<<optarg<<std::endl;
      std::string xAxisOpt = Form("%s",optarg);
      it=std::find(RootRegexp.begin(),RootRegexp.end(),"-x");
      RootRegexp.erase(it);
      it=std::find(RootRegexp.begin(),RootRegexp.end(),Form("%s",optarg));
      RootRegexp.erase(it);
      if (xAxisOpt.compare("run") == 0 || xAxisOpt.compare("Run") == 0 ) {
        std::cout<<"Compare: Trending plots versus run #, opt " << 0 <<std::endl;
        CompAnalysis.SetTrendingAxis(0);
      } else if (xAxisOpt.compare("Vop") == 0 || xAxisOpt.compare("VOP") == 0 || xAxisOpt.compare("vop") == 0 ) {
        std::cout<<"Compare: Trending plots versus Vop, opt " << 1 <<std::endl;
        CompAnalysis.SetTrendingAxis(1);
      } else if (xAxisOpt.compare("ite") == 0 || xAxisOpt.compare("iteration") == 0 || xAxisOpt.compare("Ite") == 0 ) {
        std::cout<<"Compare: Trending plots versus Iteration, opt " << 3 <<std::endl;
        CompAnalysis.SetTrendingAxis(3);
      } else if (xAxisOpt.compare("integ") == 0 || xAxisOpt.compare("Integ") == 0 || xAxisOpt.compare("Integration") == 0 ) {
        std::cout<<"Compare: Trending plots versus Iteration, opt " << 4 <<std::endl;
        CompAnalysis.SetTrendingAxis(4);
      } else if (xAxisOpt.compare("intensity") == 0 || xAxisOpt.compare("laser") == 0 || xAxisOpt.compare("Intensity") == 0 ) {
        std::cout<<"Compare: plot as function of laser intensity " <<std::endl;
        CompAnalysis.SetTrendingAxis(5);
      } else {
        std::cout << "Compare: axis option unknown will plot against run # " << 0 <<std::endl;
        CompAnalysis.SetTrendingAxis(0);
      }
      break; 
    }
    case '?':
      std::cout<<"Option "<<optarg <<" not supported, will be ignored "<<std::endl;
      break;
    case 'h':
      PrintHelp(argv[0]);
      return 0;
    }
  }
  std::cout<<"begin extra list"<<std::endl;
  for(it=RootRegexp.begin(); it!=RootRegexp.end(); ++it){
    std::cout<<*it<<std::endl;
    CompAnalysis.AddInputFile(*it);
  }
  std::cout<<"end extra list"<<std::endl;
  if(!CompAnalysis.CheckAndOpenIO()){
    std::cout<<"Check input and configurations, inconsistency or error with I/O detected"<<std::endl;
    PrintHelp(argv[0]);
    return -1;
  }

  CompAnalysis.ProcessCalib();
  //CompAnalysis.Close();
  std::cout<<"Exiting"<<std::endl;
  
  return 0;
}
