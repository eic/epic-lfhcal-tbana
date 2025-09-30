#pragma once
#ifndef COMMONHELPERFUNCTIONS_H
#define COMMONHELPERFUNCTIONS_H

  #include <iostream>
  #include <fstream>
  #include "TString.h"
  #include "TObjString.h"
  #include <vector>
  #include <map>
  #include <utility>

  struct Layer{
    Layer(): nCells(0), energy(0.), avX(0.), avY(0.) {}
    double nCells;
    double energy;
    double avX;
    double avY;
  } ;

  inline int GetMaxLayer(std::map<int,Layer> layers){
    int maxLayer      = -1;
    double maxELayer  = 0;
    std::map<int, Layer>::iterator ithLayer;
    for(ithLayer=layers.begin(); ithLayer!=layers.end(); ++ithLayer){
      if (maxELayer < ithLayer->second.energy ){
        maxELayer = ithLayer->second.energy;
        maxLayer  = ithLayer->first;
      }
    }
    return maxLayer;
  }
  inline double GetAverageLayer(std::map<int,Layer> layers){
    double avLayer    = 0;
    double totE  = 0;
    std::map<int, Layer>::iterator ithLayer;
    for(ithLayer=layers.begin(); ithLayer!=layers.end(); ++ithLayer){
      avLayer +=   ithLayer->first*ithLayer->second.energy;
      totE    += ithLayer->second.energy;
    }
    avLayer = avLayer/totE;
    return avLayer;
  }
  
  inline double GetXAverage(std::map<int,Layer> layers, int layerMax = -100){
    double avLayer    = 0;
    double totE  = 0;
    std::map<int, Layer>::iterator ithLayer;
    for(ithLayer=layers.begin(); ithLayer!=layers.end(); ++ithLayer){
      if ((layerMax != -100) && (ithLayer->first > layerMax) )
        continue;
      avLayer += ithLayer->second.avX*ithLayer->second.energy;
      totE    += ithLayer->second.energy;
    }
    avLayer = avLayer/totE;
    return avLayer;
  }

  inline double GetYAverage(std::map<int,Layer> layers, int layerMax = -100){
    double avLayer    = 0;
    double totE  = 0;
    std::map<int, Layer>::iterator ithLayer;
    for(ithLayer=layers.begin(); ithLayer!=layers.end(); ++ithLayer){
      if ( (layerMax !=-100) && (ithLayer->first > layerMax) )
        continue;
      avLayer += ithLayer->second.avY*ithLayer->second.energy;
      totE    += ithLayer->second.energy;
    }
    avLayer = avLayer/totE;
    return avLayer;
  }
  
  struct RunInfo{
    RunInfo(): runNr(0), species(""), pdg(0), energy(0), vop(0), vbr(0), lgSet(0), hgSet(0), posX(0), posY(0), shapetime(0), assemblyNr(0), year(-1), month(-1), readout(""), facility(""), beamline(""), samples(0), trigDelay(0) {}
    int runNr;
    TString species;
    int pdg;
    float energy;
    float vop;
    float vbr;
    int lgSet;
    int hgSet;
    float posX;
    float posY;
    float shapetime;
    int assemblyNr;
    int year;
    int month;
    TString readout;
    TString facility;
    TString beamline;
    int samples;
    int trigDelay;
    int trigDead;
    int phase;
  } ;

  TString GetStringFromRunInfo(RunInfo, Int_t);

  //__________________________________________________________________________________________________________
  //__________________ Read run infos from text file _________________________________________________________
  //__________________________________________________________________________________________________________    
  // specialData: 0 - std. TB, 1 - SPE data ORNL
  inline std::map<int,RunInfo> readRunInfosFromFile(TString runListFileName, int debug, int specialData = 0 ){
    std::map<int,RunInfo> runs;
    //std::cout << "INFO: You have given the following run list file: " << runListFileName.Data() << std::endl;
    std::ifstream runListFile;
    runListFile.open(runListFileName,std::ios_base::in);
    if (!runListFile) {
      std::cout << "ERROR: file " << runListFileName.Data() << " not found!" << std::endl;
      return runs;
    }

    TString facility="";
    TString beamline="";
    TString readout="";
    int year = -1;
    int month = -1;
    for( TString tempLine; tempLine.ReadLine(runListFile, kTRUE); ) {
      // check if line should be considered
      if (tempLine.BeginsWith("%") || tempLine.BeginsWith("#")){
        continue;
      }
      if (debug > 1) std::cout << tempLine.Data() << std::endl;
      
      TObjArray *tempArr2  = tempLine.Tokenize(" ");
      if(tempArr2->GetEntries()>0){
        if (tempLine.BeginsWith("year")){
          year=((TString)((TObjString*)tempArr2->At(1))->GetString()).Atoi();
          continue;
        } else if (tempLine.BeginsWith("month")){
          month=((TString)((TObjString*)tempArr2->At(1))->GetString()).Atoi();
          continue;
        } else if (tempLine.BeginsWith("readout")){
          readout=((TString)((TObjString*)tempArr2->At(1))->GetString());
          continue;
        } else if (tempLine.BeginsWith("facility")){
          facility=((TString)((TObjString*)tempArr2->At(1))->GetString());
          continue;
        } else if (tempLine.BeginsWith("beam-line")){ 
          beamline=((TString)((TObjString*)tempArr2->At(1))->GetString());
          continue;
        }
      }
      // Separate the string according to tabulators
      TObjArray *tempArr  = tempLine.Tokenize(",");
      if(tempArr->GetEntries()<1){
        if (debug > 1) std::cout << "nothing to be done" << std::endl;
        delete tempArr;
        continue;
      } 

      // Put them to the correct variables    
      RunInfo tempRun;
      tempRun.facility= facility;
      tempRun.beamline= beamline;
      tempRun.readout = readout;
      tempRun.year    = year; 
      tempRun.month    = month; 
      tempRun.runNr    = ((TString)((TObjString*)tempArr->At(0))->GetString()).Atoi();
      tempRun.species  =  (TString)((TObjString*)tempArr->At(1))->GetString();
      tempRun.pdg      = ((TString)((TObjString*)tempArr->At(2))->GetString()).Atoi();
      tempRun.energy   = ((TString)((TObjString*)tempArr->At(3))->GetString()).Atof();
      tempRun.vop      = ((TString)((TObjString*)tempArr->At(4))->GetString()).Atof();
      tempRun.vbr      = ((TString)((TObjString*)tempArr->At(5))->GetString()).Atof();
      
      if (readout.CompareTo("CAEN") == 0){
        tempRun.hgSet    = ((TString)((TObjString*)tempArr->At(6))->GetString()).Atoi();
        tempRun.lgSet    = ((TString)((TObjString*)tempArr->At(7))->GetString()).Atoi();
        if (tempArr->GetEntries() > 10){
         tempRun.shapetime = ((TString)((TObjString*)tempArr->At(10))->GetString()).Atof();
        }
      } else {
        tempRun.trigDelay = ((TString)((TObjString*)tempArr->At(6))->GetString()).Atoi();
        tempRun.samples   = ((TString)((TObjString*)tempArr->At(7))->GetString()).Atoi();
        tempRun.trigDead  = ((TString)((TObjString*)tempArr->At(10))->GetString()).Atoi();
        tempRun.phase     = ((TString)((TObjString*)tempArr->At(11))->GetString()).Atoi();
      }
      tempRun.posX    = ((TString)((TObjString*)tempArr->At(8))->GetString()).Atoi();
      tempRun.posY    = ((TString)((TObjString*)tempArr->At(9))->GetString()).Atoi();
      if (specialData == 1) tempRun.assemblyNr = ((TString)((TObjString*)tempArr->At(10))->GetString()).Atoi();
                  
      if (debug > 1) std::cout << "Run " << tempRun.runNr << "\t species: " << tempRun.species << "\t energy: "  << tempRun.energy << "\t Vop: " << tempRun.vop << "\t Vov: " << tempRun.vop-tempRun.vbr << "\t Xbeam: " << tempRun.posX<< "\t Ybeam: " << tempRun.posY << "\t shaping time: " << tempRun.shapetime << std::endl;
      runs[tempRun.runNr]=tempRun;
    }
    std::cout << year << "-" << month << "\t:\t" << facility.Data() << "-" << beamline.Data() << "\t Readout: " << readout.Data() << std::endl;
    std::cout << "registered " << runs.size() << " runs from  "<< runListFileName.Data() << std::endl;
    
    return runs;
  };

  inline int GetSpeciesIntFromRunInfo(RunInfo currRunInfo){
      if (currRunInfo.species.Contains("cosmics")){
          return  0; // cosmics
      } else if (currRunInfo.species.CompareTo("g") == 0){
          return  1; // gamma
      } else if (currRunInfo.species.Contains("muon") || currRunInfo.species.Contains("Muon") || currRunInfo.species.CompareTo("mu-") == 0){
          return  2; // muon
      } else if (currRunInfo.species.Contains("Electron") || currRunInfo.species.Contains("electron") || currRunInfo.species.CompareTo("e-") == 0 ){
          return  3; // electron
      } else if (currRunInfo.species.Contains("Pion") || currRunInfo.species.Contains("pion") || currRunInfo.species.CompareTo("pi-") == 0 || currRunInfo.species.CompareTo("pi+") == 0 ){
          return  4; // pion
      } else if (currRunInfo.species.Contains("Hadron") || currRunInfo.species.Contains("hadron") || currRunInfo.species.CompareTo("h+") == 0 || currRunInfo.species.CompareTo("h-") == 0 ){
          return  5; // hadron/proton
      }
      
      return -1;
  }
  
  inline Double_t ReturnMipPlotRangeDepVov(double Vov, bool isHG, ReadOut::Type type){
    if (type == ReadOut::Type::Caen){
      if (isHG){
        if (Vov < 2)
          return 550.;
        else if (Vov < 3)
          return 750.;
        else if (Vov < 4)
          return 950.;
        else if (Vov < 5)
          return 1150.;
        else
          return 1350.;
      } else {
        if (Vov < 2)
          return 85.;
        else if (Vov < 3)
          return 105.;
        else if (Vov < 4)
          return 125.;
        else if (Vov < 5)
          return 145.;
        else
          return 165.;      
      }
    } else {
      return 410.;
    }
  }
  

  
  
#endif
