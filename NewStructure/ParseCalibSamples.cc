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
#include "CalibSampleParser.h"

// read .csv files from H2GCROC calibration, and parse them into TTree

void PrintHelp(char* exe){
  std::cout<<"Usage:"<<std::endl;
  std::cout<<exe<<" [-option (arguments)]"<<std::endl;
  std::cout<<"Options:"<<std::endl;
  std::cout<<"-d [0-n]  switch on debug info with debug level 0 to n"<<std::endl;
  std::cout<<"-i uuu    path to the input file: either .csv or config file with .json files for pedestal calib (mandatory)"<<std::endl;
  std::cout<<"-m        path to the mapping file (mandatory)"<<std::endl;
  std::cout<<"-r        path to the run list file (mandatory)"<<std::endl;
  std::cout<<"-n        run number to analyze (mandatory)"<<std::endl;
  std::cout<<"-p        setting the plot directory (for .csv file) (mandatory for plotting)"<<std::endl;
  std::cout<<"-o        output root filename, default same as input file"<<std::endl;
  std::cout<<"-c        path to .root file with calib object (mandatory for pedestal calib)"<<std::endl;
  std::cout<<"-h        print help"<<std::endl;
  std::cout<<"Examples:"<<std::endl;
  std::cout<<exe<<"-i input.csv -o output/"<<std::endl;
}

// inj_adc_samples_208_24_300.csv
// channel, time, phase, ADC, TOT, ToA
// time - 1.5625 increments, 0 - 248.4375, 160 steps
// tree with tiles as branches, each tile - a different channel

// two modes -
//  1) read in .csv calib files, parse it into HGCROC tile; 
//          required input: path to .csv file, mapping file, run list file and the run number, potentially plotting directory
//  2) read in .json files and the pedestal values from HGCROC calibration, overwrite the pedestal values in existing calib object (preferably taken from pedestal calibration done with software)
//          required input: mapping file, run list file and the run number, config file with "dead channels" (calib channels) and list of .json files with respective KCUs, .root file with calib object

int main(int argc, char* argv[]){
    if(argc<1) {
        PrintHelp( argv[0] );
        return 0;
    }

    CalibSampleParser calibParser;
    int c;
    while( (c=getopt(argc,argv,"d:i:m:r:n:p:o:c:h:"))!=-1){
        switch(c){
            case 'd':
                std::cout << "Enable debug " << optarg << std::endl;
                calibParser.EnableDebug( atoi(optarg) );
                break;
            case 'i':
                std::cout << "Input file set to: " << optarg << std::endl;
                calibParser.SetInputFile( Form("%s",optarg) );
                break;
            case 'm':
                std::cout << "Mapping file set to " << optarg << std::endl;
                calibParser.SetMappingFile( Form("%s",optarg) );
                break;
            case 'r':
                std::cout << "Input list file set to " << optarg << std::endl;
                calibParser.SetRunListInput( Form("%s",optarg) );
                break;
            case 'n':
                std::cout << "Run number set to " << optarg << std::endl;
                calibParser.SetRunNumber ( atoi(optarg) );
                break;
            case 'p':
                std::cout << "Plotting directory set to " << optarg << std::endl;
                calibParser.EnablePlotting();
                calibParser.SetPlotDirectory( Form("%s",optarg) );
                break;
            case 'o':
                std::cout << "Output root filename set to: " << optarg << std::endl;
                calibParser.SetOutputFilename( Form("%s", optarg) );
                break;
            case 'c':
                std::cout << "Reading in .csv file with pedestal values from H2GCalib"<<std::endl; // add path to the calib object from the other run
                std::cout << "File with the calib object to switch the pedestal values: " << optarg << std::endl;
                calibParser.SwitchPedestalCalib();
                calibParser.SetInputCalibFile( Form("%s",optarg) );
                break;
            case 'h':
                PrintHelp( argv[0] );
                return 0;
        }
    }
    if(!calibParser.CheckAndOpenIO()){
      std::cout<<"Check input and configurations, inconsistency or error with I/O detected"<<std::endl;
      PrintHelp(argv[0]);
      return -1;
    }

    calibParser.Process();
    std::cout<<"Exiting"<<std::endl;
    return 0;
}



